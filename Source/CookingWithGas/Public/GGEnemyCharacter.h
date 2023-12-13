// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GGCharacterBase.h"

#include "GGEnemyCharacter.generated.h"

UCLASS(Blueprintable, BlueprintType)
class COOKINGWITHGAS_API AGGEnemyCharacter : public AGGCharacterBase
{
	GENERATED_BODY()

public:

	AGGEnemyCharacter();

	virtual void OnChilledAttributeChanged(const FOnAttributeChangeData& Data);

	UFUNCTION(BlueprintImplementableEvent, Category = "GAS")
	void OnChilledChanged(float OldValue, float NewValue);

protected:

	virtual void BeginPlay() override;

};
