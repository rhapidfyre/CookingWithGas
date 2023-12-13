
#include "CookingWithGasCharacter.h"

#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Controller.h"


void ACookingWithGasCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
}

void ACookingWithGasCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	BindInput();
}



//////////////////////////////////////////////////////////////////////////
// Input

void ACookingWithGasCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction,
			ETriggerEvent::Started, this, &ACookingWithGasCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction,
			ETriggerEvent::Completed, this, &ACookingWithGasCharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction,
			ETriggerEvent::Triggered, this, &ACookingWithGasCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction,
			ETriggerEvent::Triggered, this, &ACookingWithGasCharacter::Look);

		// Ability Actions
		EnhancedInputComponent->BindAction(FireAbilityAction,
			ETriggerEvent::Triggered, this, &ACookingWithGasCharacter::OnFireAbility);
	}
	
	// Call the function that handles ability system bindings
	BindInput();
	
}

void ACookingWithGasCharacter::BindInput()
{
	if (bIsInputBound || !AbilitySystemComponent || !IsValid(InputComponent))
		return;

	FTopLevelAssetPath EnumAssetPath = FTopLevelAssetPath(
		FName("/Script/CookingWithGas"), FName("EAbilityInputID"));
	
	GetAbilitySystemComponent()->BindAbilityActivationToInputComponent(InputComponent,
		FGameplayAbilityInputBinds(
			FString("Confirm"),
			FString("Cancel"),
			EnumAssetPath,
		static_cast<int32>(EAbilityInputID::Confirm),
		static_cast<int32>(EAbilityInputID::Cancel)));
	bIsInputBound = true;
}

void ACookingWithGasCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ACookingWithGasCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}