// Fill out your copyright notice in the Description page of Project Settings.


#include "GGDestructible.h"
#include "AbilitySystemComponent.h"
#include "GGAttributeSet.h"


// Sets default values
AGGDestructible::AGGDestructible()
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMeshComp->SetSimulatePhysics(true);
	StaticMeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	SetRootComponent(StaticMeshComp);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystem");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UGGAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AGGDestructible::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void AGGDestructible::BeginPlay()
{
	Super::BeginPlay();
	if (!AbilitySystemComponent)
		return;

	// Every time the health attribute is changed, OnHealthAttributeChanged will be called
	// OnHealthAttributeChanged is executed in blueprint
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AttributeSet->GetHealthAttribute()).AddUObject(
			this, &AGGDestructible::OnHealthAttributeChanged);
}

void AGGDestructible::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged(Data.OldValue, Data.NewValue);
}
