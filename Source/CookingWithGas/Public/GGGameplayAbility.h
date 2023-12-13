#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "../CookingWithGas.h"

#include "GGGameplayAbility.generated.h"

UCLASS()
class COOKINGWITHGAS_API UGGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EAbilityInputID AbilityInputID { EAbilityInputID::None };
	
};
