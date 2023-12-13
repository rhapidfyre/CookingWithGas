// Fill out your copyright notice in the Description page of Project Settings.


#include "GGEnemyCharacter.h"

#include "AbilitySystemComponent.h"
#include "GGAttributeSet.h"

// Sets default values
AGGEnemyCharacter::AGGEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

/**
 *  Allows access to the NPCs chill attribute for monitoring
 * @param Data The chill effect data
 */
void AGGEnemyCharacter::OnChilledAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnChilledChanged(Data.OldValue, Data.NewValue);
}

// Called when the game starts or when spawned
void AGGEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Sets up "OnArmorAttributeChanged" to be called whenever the CHILLED
	// attribute changes within the AbilitySystemComponent
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AttributeSet->GetChilledAttribute()).AddUObject(
			this, &AGGEnemyCharacter::OnChilledAttributeChanged);

}

