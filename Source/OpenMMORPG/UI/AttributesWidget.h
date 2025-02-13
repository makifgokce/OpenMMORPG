

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AttributesWidget.generated.h"

UCLASS()
class OPENMMORPG_API UAttributesWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindToAttributes();

protected:
	UPROPERTY(BlueprintReadOnly)
	float HP_Percent;
	UPROPERTY(BlueprintReadOnly)
	float MP_Percent;

	UPROPERTY(BlueprintReadOnly)
	FText HPText;
	UPROPERTY(BlueprintReadOnly)
	FText MPText;
};
