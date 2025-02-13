


#include "AttributesWidget.h"
#include "../Abilities/GAS_AttributeSet.h"
#include "../Player/OPlayerState.h"

class FText;

void UAttributesWidget::BindToAttributes()
{
	const AOPlayerState* PlayerState = Cast<AOPlayerState>(GetOwningPlayerState());

	if (!PlayerState) return;

	UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
	const UGAS_AttributeSet* OGAS_AS = PlayerState->GetAttributeSet();


	HP_Percent = NUMERIC_VALUE(OGAS_AS, Health) / NUMERIC_VALUE(OGAS_AS, MaxHealth);
	MP_Percent = NUMERIC_VALUE(OGAS_AS, Mana) / NUMERIC_VALUE(OGAS_AS, MaxMana);
	HPText = FText::Format(NSLOCTEXT("CharacterLifeStats", "HP", "{0} / {1}"), NUMERIC_VALUE(OGAS_AS, Health), NUMERIC_VALUE(OGAS_AS, MaxHealth));
	MPText = FText::Format(NSLOCTEXT("CharacterLifeStats", "MP", "{0} / {1}"), NUMERIC_VALUE(OGAS_AS, Mana), NUMERIC_VALUE(OGAS_AS, MaxMana));

	ASC->GetGameplayAttributeValueChangeDelegate(OGAS_AS->GetHealthAttribute()).AddLambda(
		[this, OGAS_AS](const FOnAttributeChangeData& Data) {
			HP_Percent = Data.NewValue / NUMERIC_VALUE(OGAS_AS, MaxHealth);
			HPText = FText::Format(NSLOCTEXT("CharacterLifeStats", "HP", "{0} / {1}"), NUMERIC_VALUE(OGAS_AS, Health), NUMERIC_VALUE(OGAS_AS, MaxHealth));
		});
	ASC->GetGameplayAttributeValueChangeDelegate(OGAS_AS->GetManaAttribute()).AddLambda(
		[this, OGAS_AS](const FOnAttributeChangeData& Data) {
			MP_Percent = Data.NewValue / NUMERIC_VALUE(OGAS_AS, MaxMana);
			MPText = FText::Format(NSLOCTEXT("CharacterLifeStats", "MP", "{0} / {1}"), NUMERIC_VALUE(OGAS_AS, Mana), NUMERIC_VALUE(OGAS_AS, MaxMana));
		});
}
