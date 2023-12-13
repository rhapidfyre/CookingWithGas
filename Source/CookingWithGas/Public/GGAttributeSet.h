// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Delegates/Delegate.h"
#include "AbilitySystemComponent.h"

#include "GGAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE_FourParams(FGGAttributeEvent,
	AActor*,						// Effect Instigator (what called the effect)
	AActor*,						// Effect Causer (what dun it)
	const FGameplayEffectSpec&, 	// the effect spec
	float							// Effect magnitude
	);

DECLARE_MULTICAST_DELEGATE_SixParams(FGGAttributeDamageEvent,
	AActor*,						// Effect Instigator (what called the effect)
	AActor*,						// Effect Causer (what dun it)
	const FGameplayTagContainer&,	// Tag Container
	float,							// Effect magnitude
	bool,							// bIsCriticalHit
	bool							// bIsLuckyHit
	);

/**
 * 
 */
UCLASS()
class COOKINGWITHGAS_API UGGAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	
	UGGAttributeSet();

	// This attribute is for tracking the entity's current health value
	UPROPERTY(BlueprintReadOnly, Category = "Attributes",
		ReplicatedUsing=OnRep_Health, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UGGAttributeSet, Health);

	// Tracking the entity's maximum health value; Used for clamping.
	UPROPERTY(BlueprintReadOnly, Category = "Attributes",
		ReplicatedUsing=OnRep_HealthMax, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData HealthMax;
	ATTRIBUTE_ACCESSORS(UGGAttributeSet, HealthMax);
	
	// This attribute is for tracking the entity's current armor value
	UPROPERTY(BlueprintReadOnly, Category = "Attributes",
		ReplicatedUsing=OnRep_Armor, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UGGAttributeSet, Armor);

	// Tracking the entity's maximum armor value; Used for clamping.
	UPROPERTY(BlueprintReadOnly, Category = "Attributes",
		ReplicatedUsing=OnRep_ArmorMax, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ArmorMax;
	ATTRIBUTE_ACCESSORS(UGGAttributeSet, ArmorMax);

	// Tracks the damage coming in, so it can be manipulated before being applied
	UPROPERTY(BlueprintReadOnly, Category = "Attributes",
		ReplicatedUsing=OnRep_InDamage, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData InDamage;
	ATTRIBUTE_ACCESSORS(UGGAttributeSet, InDamage);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes",
		ReplicatedUsing=OnRep_CriticalChance, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CriticalChance;
	ATTRIBUTE_ACCESSORS(UGGAttributeSet, CriticalChance);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes",
		ReplicatedUsing=OnRep_CriticalMultiplier, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData CriticalMultiplier;
	ATTRIBUTE_ACCESSORS(UGGAttributeSet, CriticalMultiplier);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes",
		ReplicatedUsing=OnRep_LuckyChance, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData LuckyChance;
	ATTRIBUTE_ACCESSORS(UGGAttributeSet, LuckyChance);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes",
		ReplicatedUsing=OnRep_DamageAdd, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DamageAdd;
	ATTRIBUTE_ACCESSORS(UGGAttributeSet, DamageAdd);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes",
		ReplicatedUsing=OnRep_DamageMulti, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DamageMulti;
	ATTRIBUTE_ACCESSORS(UGGAttributeSet, DamageMulti);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes",
		ReplicatedUsing=OnRep_Ammo, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Ammo;
	ATTRIBUTE_ACCESSORS(UGGAttributeSet, Ammo);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes",
		ReplicatedUsing=OnRep_Chilled, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Chilled;
	ATTRIBUTE_ACCESSORS(UGGAttributeSet, Chilled);

	UPROPERTY(BlueprintReadOnly, Category = "Attributes",
		ReplicatedUsing=OnRep_DeChill, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData DeChill;
	ATTRIBUTE_ACCESSORS(UGGAttributeSet, DeChill);

	mutable FGGAttributeEvent OnOutOfHealth; // Used to bind listeners for when health runs out
	mutable FGGAttributeEvent OnOutOfArmor;  // Used to bind listeners for when armor runs out
	mutable FGGAttributeDamageEvent OnDamageTaken; // Used to bind listeners for when health runs out
	
protected:

	// Triggers notification after health has been changed via network replication
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealthData);

	// Triggers notification after health maximum has been changed via network replication
	UFUNCTION()
	virtual void OnRep_HealthMax(const FGameplayAttributeData& OldHealthMaxData);
	
	// Triggers notification after armor has been changed via network replication
	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldArmorData);

	// Triggers notification after armor maximum has been changed via network replication
	UFUNCTION()
	virtual void OnRep_ArmorMax(const FGameplayAttributeData& OldArmorMaxData);

	// Triggers notification after damage coming in has been changed via network replication
	UFUNCTION()
	virtual void OnRep_InDamage(const FGameplayAttributeData& OldInDamage);

	// Triggers notification after critical chance has been changed via network replication
	UFUNCTION()
	virtual void OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance);

	// Triggers notification after critical multiplier has been changed via network replication
	UFUNCTION()
	virtual void OnRep_CriticalMultiplier(const FGameplayAttributeData& OldCriticalMultiplier);

	// Triggers notification after lucky chance has been changed via network replication
	UFUNCTION()
	virtual void OnRep_LuckyChance(const FGameplayAttributeData& OldLuckyChance);

	// Triggers notification after additional damage has been changed via network replication
	UFUNCTION()
	virtual void OnRep_DamageAdd(const FGameplayAttributeData& OldDamageAdd);

	// Triggers notification after damage multiplier has been changed via network replication
	UFUNCTION()
	virtual void OnRep_DamageMulti(const FGameplayAttributeData& OldDamageMulti);

	// Triggers notification after ammo value has been changed via network replication
	UFUNCTION()
	virtual void OnRep_Ammo(const FGameplayAttributeData& OldAmmo);

	// Triggers notification after chilled value has been changed via network replication
	UFUNCTION()
	virtual void OnRep_Chilled(const FGameplayAttributeData& OldChilled);

	// Triggers notification after de-chill value has been changed via network replication
	UFUNCTION()
	virtual void OnRep_DeChill(const FGameplayAttributeData& OldDeChill);
	

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void ClampAttributeOnChange(const FGameplayAttribute& Attribute, float& NewValue) const;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	
	bool bOutOfHealth = false;

	bool bOutOfArmor = false;
	
};
