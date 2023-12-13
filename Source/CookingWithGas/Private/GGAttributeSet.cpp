// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/GGAttributeSet.h"
#include "GameplayEffectExtension.h"	// For:		const FGameplayEffectModCallbackData& Data
#include "GGGameplayEffectContext.h"
#include "Logging/StructuredLog.h"
#include "Net/UnrealNetwork.h"			// Replication

UGGAttributeSet::UGGAttributeSet()
	: Health(65.f), HealthMax(100.f),
	  Armor(20.f), ArmorMax(100.f),
      CriticalChance(0.f), CriticalMultiplier(3.f), LuckyChance(5.f),
	  DamageAdd(0.f), DamageMulti(1.f),
	  Ammo(100.f),
	  Chilled(0.f), DeChill(10.f)
{
	
}

void UGGAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealthData)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGGAttributeSet, Health, OldHealthData);
}

void UGGAttributeSet::OnRep_HealthMax(const FGameplayAttributeData& OldHealthMaxData)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGGAttributeSet, HealthMax, OldHealthMaxData);
}

void UGGAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmorData)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGGAttributeSet, Armor, OldArmorData);
}

void UGGAttributeSet::OnRep_ArmorMax(const FGameplayAttributeData& OldArmorMaxData)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGGAttributeSet, ArmorMax, OldArmorMaxData);
}

void UGGAttributeSet::OnRep_InDamage(const FGameplayAttributeData& OldInDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGGAttributeSet, InDamage, OldInDamage);
}

void UGGAttributeSet::OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGGAttributeSet, CriticalChance, OldCriticalChance);
}

void UGGAttributeSet::OnRep_CriticalMultiplier(const FGameplayAttributeData& OldCriticalMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGGAttributeSet, CriticalMultiplier, OldCriticalMultiplier);
}

void UGGAttributeSet::OnRep_LuckyChance(const FGameplayAttributeData& OldLuckyChance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGGAttributeSet, LuckyChance, OldLuckyChance);
}

void UGGAttributeSet::OnRep_DamageAdd(const FGameplayAttributeData& OldDamageAdd)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGGAttributeSet, DamageAdd, OldDamageAdd);
}

void UGGAttributeSet::OnRep_DamageMulti(const FGameplayAttributeData& OldDamageMulti)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGGAttributeSet, DamageMulti, OldDamageMulti);
}

void UGGAttributeSet::OnRep_Ammo(const FGameplayAttributeData& OldAmmo)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGGAttributeSet, Ammo, OldAmmo);
}

void UGGAttributeSet::OnRep_Chilled(const FGameplayAttributeData& OldChilled)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGGAttributeSet, Chilled, OldChilled);
}

void UGGAttributeSet::OnRep_DeChill(const FGameplayAttributeData& OldDeChill)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGGAttributeSet, DeChill, OldDeChill);
}

/**
 *	This is called just before any modification happens to an attribute's base value when an attribute aggregator exists.
 *	This function should enforce clamping (presuming you wish to clamp the base value along with the final value in PreAttributeChange)
 *	This function should NOT invoke gameplay related events or callbacks. Do those in PreAttributeChange() which will be called prior to the
 *	final value of the attribute actually changing.
 * @param Attribute the attribute about to be modified
 * @param NewValue the new value that is about to be modified
 */
void UGGAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
	ClampAttributeOnChange(Attribute, NewValue);
}

/**
 *  Called just before any modification happens to an attribute. This is lower level than PreAttributeModify/PostAttribute modify.
 *	There is no additional context provided here since anything can trigger this. Executed effects, duration based effects, effects being removed, immunity being applied, stacking rules changing, etc.
 *	This function is meant to enforce things like "Health = Clamp(Health, 0, MaxHealth)" and NOT things like "trigger this extra thing if damage is applied, etc".
 * @param Attribute the attribute about to be modified
 * @param NewValue the new value that is about to be modified, passed by reference
 */
void UGGAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	ClampAttributeOnChange(Attribute, NewValue);
}

/**
 *  Performs clamping for specific attributes to be within a certain range.
 * @param Attribute The attribute that is being checked
 * @param NewValue The value being proposed, by reference (to be modified)
 */
void UGGAttributeSet::ClampAttributeOnChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetHealthMax());
	}
	else if (Attribute == GetArmorAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetArmorMax());
	}
	else if (Attribute == GetChilledAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 100.f);
	}
	else if (Attribute == GetDeChillAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, 100.f);
	}
}

/**
 *  Called just before a GameplayEffect is executed to modify the base value
 *  of an attribute. No more changes can be made.
 * @param Data Contains the data from the effect post execution
 */
void UGGAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if (Data.EvaluatedData.Attribute == GetInDamageAttribute())
	{
		// Saves the damage to local variable, then clears the class member
		float inDamage = GetInDamage();
		SetInDamage(0.f);

		// If any damage was done
		if (inDamage > 0.f)
		{
			
			// Apply damage to armor
			if (GetArmor() > 0.f)
			{
				float inDamageToArmor = inDamage;
				const FGameplayTag AcidDamageTag = FGameplayTag::RequestGameplayTag(FName("Damage.Type.Acid"), false);
				bool isAcidDamage = Data.EffectSpec.CapturedSourceTags.GetSpecTags().HasTagExact(AcidDamageTag);
				if (isAcidDamage)
				{
					inDamageToArmor *= 1.5;
				}

				float newArmor = GetArmor();
				const float armorDiff = FMath::Min(newArmor, inDamageToArmor);
				inDamage -= armorDiff;
				newArmor -= armorDiff;
				SetArmor(FMath::Clamp(newArmor, 0.f, GetArmorMax()));

				// If the armor just ran out, trigger listeners
				if (GetArmor() <= 0.f && !bOutOfArmor)
				{
					// Gets the elements related to the effect
					const FGameplayEffectContextHandle EffectContext = Data.EffectSpec.GetEffectContext();
					AActor* actorInstigator = EffectContext.GetOriginalInstigator();
					AActor* actorCauser     = EffectContext.GetEffectCauser();

					// Fires listeners
					OnOutOfArmor.Broadcast(actorInstigator, actorCauser, Data.EffectSpec, Data.EvaluatedData.Magnitude);
				}
				bOutOfArmor = (GetArmor() <= 0.f);
			}

			// Same process, now for health
			if (inDamage > 0.f)
			{
				float inDamageToHealth = inDamage;
				const FGameplayTag FireDamageTag = FGameplayTag::RequestGameplayTag(FName("Damage.Type.Fire"), false);
				bool isFireDamage = Data.EffectSpec.CapturedSourceTags.GetSpecTags().HasTagExact(FireDamageTag);
				if (isFireDamage)
				{
					inDamageToHealth *= 1.5;
				}
				
				const float newHealth = GetHealth() - inDamageToHealth;
				SetHealth(FMath::Clamp(newHealth , 0.f, GetHealthMax()));
				if (((GetHealth() <= 0.f) && !bOutOfHealth))
				{
					if (OnOutOfHealth.IsBound())
					{
						const FGameplayEffectContextHandle EffectContext = Data.EffectSpec.GetEffectContext();
						AActor* actorInstigator = EffectContext.GetOriginalInstigator();
						AActor* actorCauser     = EffectContext.GetEffectCauser();
					
						OnOutOfHealth.Broadcast(actorInstigator, actorCauser, Data.EffectSpec, Data.EvaluatedData.Magnitude);
					}
				}
				bOutOfHealth = (GetHealth() <= 0.f);
			}

			if (OnDamageTaken.IsBound())
			{
				const FGameplayEffectContextHandle& ContextHandle = Data.EffectSpec.GetEffectContext();
				AActor* InstigatingActor = ContextHandle.GetOriginalInstigator();
				AActor* CausingActor     = ContextHandle.GetEffectCauser();

				bool isCrit	 = false;
				bool isLucky = false;
				const FGGGameplayEffectContext* EffectContext =
					static_cast<FGGGameplayEffectContext*>(Data.EffectSpec.GetContext().Get());
				
				if (EffectContext != nullptr)
				{
					isCrit = EffectContext->IsCriticalHit();
					isLucky = EffectContext->IsLuckyHit();
				}
				OnDamageTaken.Broadcast(InstigatingActor, CausingActor,
					Data.EffectSpec.CapturedSourceTags.GetSpecTags(),
					Data.EvaluatedData.Magnitude, isCrit, isLucky);
			}
		}
	}
}

void UGGAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Vitality Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UGGAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGGAttributeSet, HealthMax, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGGAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGGAttributeSet, ArmorMax, COND_None, REPNOTIFY_Always);

	// Critical Hit System
	DOREPLIFETIME_CONDITION_NOTIFY(UGGAttributeSet, CriticalChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGGAttributeSet, CriticalMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGGAttributeSet, LuckyChance, COND_None, REPNOTIFY_Always);

	// Ammo Count
	DOREPLIFETIME_CONDITION_NOTIFY(UGGAttributeSet, Ammo, COND_None, REPNOTIFY_Always);

	// Freeze/Thaw
	DOREPLIFETIME_CONDITION_NOTIFY(UGGAttributeSet, Chilled, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGGAttributeSet, DeChill, COND_None, REPNOTIFY_Always);
}
