// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GGGameplayAbility.h"
#include "InputActionValue.h"
#include "Delegates/Delegate.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "GGCharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCharacterBase, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeUpdated,
	const FGameplayAttribute&, AttributeData, const float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthDepleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnArmorDepleted);

UCLASS(Blueprintable, BlueprintType)
class COOKINGWITHGAS_API AGGCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	
	AGGCharacterBase();

	UPROPERTY(BlueprintAssignable) FOnAttributeUpdated	OnAttributeUpdated;
	UPROPERTY(BlueprintAssignable) FOnHealthDepleted	OnHealthDepleted;
	UPROPERTY(BlueprintAssignable) FOnArmorDepleted		OnArmorDepleted;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	// Returns the AbilitySystemComponent so it can be made private
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// The actual ability system component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	class UAbilitySystemComponent* AbilitySystemComponent;

	// The set of attributes to be used by this actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	class UGGAttributeSet* AttributeSet;

	// An array of default abilities on spawn, set within blueprint
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf <class UGGGameplayAbility> > DefaultAbilities;

	// An array of default effects on spawn, set within blueprint
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS")
	TArray<TSubclassOf <class UGameplayEffect> > DefaultEffects;
	

protected:

	virtual void PossessedBy(AController* NewController) override;

	virtual void InitializeAbilities(); // Sets up default abilities
	virtual void InitializeEffects();	// Sets up default effects

	virtual void OnRep_PlayerState() override;
	

	// CPP-only function that triggers after the health value changes
	// Internally calls OnHealthChanged (Blueprint Event)
	virtual void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);

	// Event to allow blueprints to implement their own logic when health value has been changed
	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float OldValue, float NewValue);

	
	// CPP-only function that triggers after the armor value changes
	// Internally calls OnArmorChanged (Blueprint Event)
	virtual void OnArmorAttributeChanged(const FOnAttributeChangeData& Data);
	
	// Event to allow blueprints to implement their own logic when armor value has been changed
	UFUNCTION(BlueprintImplementableEvent)
	void OnArmorChanged(float OldValue, float NewValue);


	// CPP function that triggers after out-of-health has toggled true or false
	// Internally calls OnOutOfHealth (Blueprint Event)
	virtual void OnOutOfHealthChanged(AActor* DamageInstigator,
									  AActor* DamageCauser,
									  const FGameplayEffectSpec& DamageSpec,
									  float DamageMagnitude);

	// Event to allow blueprints to implement their own logic when health has been changed
	UFUNCTION(BlueprintImplementableEvent)
	void OnOutOfHealth(AActor* DamageInstigator,
					   AActor* DamageCauser,
					   const FGameplayEffectSpec& DamageSpec,
					   float DamageMagnitude);
	

	// CPP function that triggers after out-of-armor has toggled true or false
	// Internally calls OnOutOfArmor (Blueprint Event)
	virtual void OnOutOfArmorChanged(AActor* DamageInstigator,
									 AActor* DamageCauser,
									 const FGameplayEffectSpec& DamageSpec,
									 float DamageMagnitude);

	// Event to allow blueprints to implement their own logic when health has been changed
	UFUNCTION(BlueprintImplementableEvent)
	void OnOutOfArmor(AActor* DamageInstigator,
					  AActor* DamageCauser,
					  const FGameplayEffectSpec& DamageSpec,
					  float DamageMagnitude);
	
	// To add mapping context & set up game attribute listeners
	virtual void BeginPlay() override;

	// Called when an ability input has been triggered
	void OnFireAbility(const FInputActionValue& Value);

	// Sends the input action to the AbilitySystemComponent
	virtual void SendAbilityLocalInput(const FInputActionValue& Value, int32 InputID);

	// True when the inputs have been bound for the AbilitySystemComponent
	// False indicates the input for abilities has not initialized
	bool bIsInputBound = false;

public:
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

