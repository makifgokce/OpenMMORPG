

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "../Abilities/GAS_AttributeSet.h"
#include "../Abilities/GAS_AbilitySystemComponent.h"
#include "OPlayerState.generated.h"


class UGAS_AbilitySystemComponent;
class UGAS_AttributeSet;
/**
 * 
 */
UCLASS()
class OPENMMORPG_API AOPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:	
	AOPlayerState();
	//~IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface interface
	virtual UGAS_AttributeSet* GetAttributeSet() const;
protected:
	UPROPERTY()
	TObjectPtr<UGAS_AbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UGAS_AttributeSet> AttributeSet;
	
	
};
