

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OHud.generated.h"

class UAttributesWidget;
/**
 * 
 */
UCLASS()
class OPENMMORPG_API AOHud : public AHUD
{
	GENERATED_BODY()

public:
	void Init();

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributesWidget> AttributeWidgetClass;
	
private:
	TObjectPtr<UAttributesWidget> AttributeWidget;
};
