// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "GGDestructible.generated.h"

UCLASS()
class COOKINGWITHGAS_API AGGDestructible : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGGDestructible();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, Meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", Meta = (AllowPrivateAccess = true))
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", Meta = (AllowPrivateAccess = true))
	class UGGAttributeSet* AttributeSet;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	virtual void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);

	UFUNCTION(BlueprintImplementableEvent, Category = "GAS")
	void OnHealthChanged(float OldValue, float NewValue);
};
