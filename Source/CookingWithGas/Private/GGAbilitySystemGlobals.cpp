
#include "GGAbilitySystemGlobals.h"

#include "GGGameplayEffectContext.h"

UGGAbilitySystemGlobals::UGGAbilitySystemGlobals()
{
	
}

FGameplayEffectContext* UGGAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FGGGameplayEffectContext();
}
