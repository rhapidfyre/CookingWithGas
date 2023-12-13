
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"

#include "GGAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class COOKINGWITHGAS_API UGGAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
public:
	UGGAbilitySystemGlobals();

	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
