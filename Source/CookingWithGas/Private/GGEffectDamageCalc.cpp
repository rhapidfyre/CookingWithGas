// Fill out your copyright notice in the Description page of Project Settings.


#include "GGEffectDamageCalc.h"
#include "GGAttributeSet.h"

#include "Logging/StructuredLog.h"

// Allows manipulation of the captured values, such as resistances and bonuses
struct FDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(InDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalMultiplier);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LuckyChance);
	FDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGGAttributeSet, InDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGGAttributeSet, CriticalChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGGAttributeSet, CriticalMultiplier, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGGAttributeSet, LuckyChance, Source, false);
	}
};

static FDamageStatics DamageStatics()
{
	static FDamageStatics DmgStatics;
	return DmgStatics;
}

UGGEffectDamageCalc::UGGEffectDamageCalc()
{
	RelevantAttributesToCapture.Add(DamageStatics().InDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalMultiplierDef);
	RelevantAttributesToCapture.Add(DamageStatics().LuckyChanceDef);
}

void UGGEffectDamageCalc::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                 FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	
	UAbilitySystemComponent* SourceComponent = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* TargetActor = IsValid(TargetComponent) ? TargetComponent->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Retrieve the InDamage value passed to Assign Tag Set by Caller Magnitude
	//		(on output of Make Outgoing Spec)
	
	//const FGameplayTag SetCallerTag = FGameplayTag::RequestGameplayTag(InDamageTag,false);
	float InDamage = 0.f;//FMath::Max(EffectSpec.GetSetByCallerMagnitude(SetCallerTag, false, -1.f), 0.f);
	
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().InDamageDef, EvaluationParameters, InDamage);

	FGameplayEffectSpec* MutableSpec = ExecutionParams.GetOwningSpecForPreExecuteMod();
	float CriticalChance	 = 0.f;
	float CriticalMultiplier = 0.f;
	float LuckyChance		 = 0.f;

	// Set critical chance to 100% if the hit bone is the head
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().CriticalChanceDef, EvaluationParameters, CriticalChance);
	const FHitResult* HitResult = EffectSpec.GetContext().GetHitResult();
	if (HitResult)
	{
		if (HitResult->BoneName == "head")
		{
			CriticalChance = 100.f;
		}
	}

	// Multiply the damage if the hit was a critical hit
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().CriticalMultiplierDef, EvaluationParameters, CriticalMultiplier);
	if (CriticalMultiplier > 1.f)
	{
		const float criticalCheck = FMath::RandRange(0.01f, 100.f);
		if (criticalCheck <= CriticalChance)
		{
			InDamage *= CriticalMultiplier;
		}
	}

	// For every time the random number is below the lucky chance,
	//	the damage multiplier will be increased by *1 and the lucky chance
	//	gets lowered by 100%
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		DamageStatics().LuckyChanceDef, EvaluationParameters, LuckyChance);
	if (LuckyChance > 0.f)
	{
		bool isLucky = false;
		float LuckyMulti = 1.f;
		
		while (FMath::RandRange(0.01f, 100.f) <= LuckyChance)
		{
			LuckyChance -= 100.f;
			LuckyMulti  += 1.f;
			isLucky		 = true;
		}
		
		if (isLucky)
		{
			InDamage *= LuckyMulti;
		}
	}

	UE_LOGFMT(LogTemp, Log, "Damage After Modification: {DamageValue}", InDamage);
	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(DamageStatics().InDamageProperty,
										EGameplayModOp::Additive, InDamage));
	
}
