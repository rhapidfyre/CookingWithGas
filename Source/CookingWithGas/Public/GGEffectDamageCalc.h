// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"

#include "GGEffectDamageCalc.generated.h"

/**
 * To utilize this in the project, add this UGGEffectDamageCalc to your
 * Gameplay Effect execution array.
 */
UCLASS()
class COOKINGWITHGAS_API UGGEffectDamageCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UGGEffectDamageCalc();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName InDamageTag = FName("Damage.SetByCaller");
	
};
