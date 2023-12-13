// Copyright Epic Games, Inc. All Rights Reserved.

#include "GGCharacterBase.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "AbilitySystemComponent.h"
#include "GGAttributeSet.h"

DEFINE_LOG_CATEGORY(LogCharacterBase);

//////////////////////////////////////////////////////////////////////////
// AGGCharacterBase

AGGCharacterBase::AGGCharacterBase()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComp");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UGGAttributeSet>("AttributeSet");

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AGGCharacterBase::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Sets up "OnHealthAttributeChanged" to be called whenever the HEALTH
	// attribute changes within the AbilitySystemComponent
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AttributeSet->GetHealthAttribute()).AddUObject(
		this, &AGGCharacterBase::OnHealthAttributeChanged);

	// Sets up "OnArmorAttributeChanged" to be called whenever the ARMOR
	// attribute changes within the AbilitySystemComponent
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AttributeSet->GetArmorAttribute()).AddUObject(
		this, &AGGCharacterBase::OnArmorAttributeChanged);

	// Triggers 'OnOutOfHealthChanged' listeners when the attribute set
	// broadcasts that "Out of Health" boolean has toggled true or false
	AttributeSet->OnOutOfHealth.AddUObject(this, &AGGCharacterBase::OnOutOfHealthChanged);
	
	// Triggers 'OnOutOfArmorChanged' listeners when the attribute set
	// broadcasts that "Out of Armor" boolean has toggled true or false
	AttributeSet->OnOutOfArmor.AddUObject(this, &AGGCharacterBase::OnOutOfArmorChanged);
	
	// Triggers 'OnDamageTakenChanged' listeners when the attribute set
	// Broadcasts that damage has been processed
	AttributeSet->OnDamageTaken.AddUObject(this, &AGGCharacterBase::OnDamageTakenChanged);
}

UAbilitySystemComponent* AGGCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGGCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (!AbilitySystemComponent)
		return;

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	InitializeAbilities();
	InitializeEffects();
}

void AGGCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (!AbilitySystemComponent)
		return;
	
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	InitializeEffects();
}

void AGGCharacterBase::InitializeAbilities()
{
	// Only run on server
	if (!HasAuthority() || !AbilitySystemComponent)
		return;

	for (TSubclassOf<UGGGameplayAbility>& Ability : DefaultAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(Ability, 1,
				static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID), this));
	}
}

void AGGCharacterBase::InitializeEffects()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect>& Effect : DefaultEffects)
	{
		FGameplayEffectSpecHandle SpecHandle =
			AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, EffectContext);
		
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GEHandle =
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

/**
 *  Performs logic AFTER the health attribute value has been changed.
 * @param Data Contains the data regarding the attribute change
 */
void AGGCharacterBase::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnAttributeUpdated.Broadcast(Data.Attribute, Data.NewValue);
	
	// Calls the blueprint event
	OnHealthChanged(Data.OldValue, Data.NewValue);
}

/**
 *  Performs logic AFTER the armor attribute value has been changed.
 * @param Data Contains the data regarding the attribute change
 */
void AGGCharacterBase::OnArmorAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnAttributeUpdated.Broadcast(Data.Attribute, Data.NewValue);
	
	// Calls the blueprint event
	OnArmorChanged(Data.OldValue, Data.NewValue);
}

/**
 *  Performs logic after OutOfHealth boolean has toggled true or false.
 *  Internally calls the Blueprint Event, 'OnOutOfHealth'.
 * @param DamageInstigator The instigator of the boolean changing
 * @param DamageCauser The actor who caused the health to be changed
 * @param DamageSpec The damage specifics that caused the boolean to change
 * @param DamageMagnitude The amount of damage that caused the boolean to change
 */
void AGGCharacterBase::OnOutOfHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageSpec, float DamageMagnitude)
{
	OnHealthDepleted.Broadcast();
	
	// Calls the blueprint event
	OnOutOfHealth(DamageInstigator, DamageCauser, DamageSpec, DamageMagnitude);
}

/**
 *  Performs logic after OutOfArmor boolean has toggled true or false.
 *  Internally calls the Blueprint Event, 'OnOutOfHealth'.
 * @param DamageInstigator The instigator of the boolean changing
 * @param DamageCauser The actor who caused the health to be changed
 * @param DamageSpec The damage specifics that caused the boolean to change
 * @param DamageMagnitude The amount of damage that caused the boolean to change
 */
void AGGCharacterBase::OnOutOfArmorChanged(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageSpec, float DamageMagnitude)
{
	OnArmorDepleted.Broadcast();
	
	// Calls the blueprint event
	OnOutOfArmor(DamageInstigator, DamageCauser, DamageSpec, DamageMagnitude);
}

void AGGCharacterBase::OnDamageTakenChanged(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayTagContainer& DamageTags, float DamageMagnitude, bool isCritical, bool isLucky)
{
	OnDamage.Broadcast(DamageTags, DamageMagnitude);
	
	//Calls the blueprint event
	OnDamageTaken(DamageInstigator, DamageCauser, DamageTags, DamageMagnitude, isCritical, isLucky);
}

void AGGCharacterBase::OnFireAbility(const FInputActionValue& Value)
{
	SendAbilityLocalInput(Value, static_cast<int32>(EAbilityInputID::Fire));
}

void AGGCharacterBase::SendAbilityLocalInput(const FInputActionValue& Value, int32 InputID)
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (Value.Get<bool>())
	{
		AbilitySystemComponent->AbilityLocalInputPressed(InputID);
	}
	else
	{
		AbilitySystemComponent->AbilityLocalInputReleased(InputID);
	}
}