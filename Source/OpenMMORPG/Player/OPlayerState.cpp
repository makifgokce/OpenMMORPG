


#include "OPlayerState.h"

AOPlayerState::AOPlayerState() {
	NetUpdateFrequency = 100.f;

	AbilitySystemComponent = CreateDefaultSubobject<UGAS_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UGAS_AttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AOPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UGAS_AttributeSet* AOPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}
