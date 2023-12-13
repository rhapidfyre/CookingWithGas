// Copyright Epic Games, Inc. All Rights Reserved.

// ReSharper disable CppUEBlueprintImplementableEventNotImplemented
#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystems.h"
#include "GGCharacterBase.h"

#include "CookingWithGasCharacter.generated.h"


UCLASS(config=Game)
class ACookingWithGasCharacter : public AGGCharacterBase
{
	GENERATED_BODY()
public:
	
	ACookingWithGasCharacter() {};
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext = nullptr;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction = nullptr;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction = nullptr;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction = nullptr;

	/** Fire Ability Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAbilityAction = nullptr;

protected:

	virtual void BeginPlay() override;
	
	virtual void OnRep_PlayerState() override;

	// From APawn.h
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// Binds inputs specifically set for abilities & effects
	virtual void BindInput();
	
};

