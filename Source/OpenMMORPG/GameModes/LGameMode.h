

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LGameMode.generated.h"

/**
 * 
 */
UCLASS()
class OPENMMORPG_API ALGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:	
	
	UFUNCTION(BlueprintCallable)
	void OnLogin(FString Account, FString Password);
};
