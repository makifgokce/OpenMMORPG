

#pragma once

#include "../Abilities/GAS_AbilitySystemComponent.h"
#include "../Abilities/GAS_GameplayAbility.h"
#include "../Abilities/GAS_AttributeSet.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Components/SphereComponent.h"
#include "OCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USphereComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);


UCLASS(Blueprintable, config = Game)
class OPENMMORPG_API AOCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RightAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CameraZoomAction;

	UPROPERTY(EditAnywhere, Category = "Character | Target")
	class USphereComponent* Sphere;

public:
	// Sets default values for this character's properties
	AOCharacterBase();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*
		GameAbilitySystem
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilitiy")
	class UGAS_AbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilitiy")
	class UGAS_AttributeSet* Attributes;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilitiy")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilitiy")
	TArray<TSubclassOf<class UGAS_GameplayAbility>> DefaultAbilities;

	// Getter for AbilitySystemComponent attached to this character.
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Overload to initialize attributes for GAS, and component to store default attributes
	virtual void InitializeAttributes();

	// Overload to initialize abilities for GAS, this handles applying the DefaultAbilities effect defined above.
	virtual void GiveAbilities();

	void PossessedBy(AController* NewController) override;

	void OnRep_PlayerState() override;

	/*
		End GameAbilitySystem
	*/
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	void CameraZoom(const FInputActionValue& Value);
	void OnRightClickPressed();
	void OnRightClickReleased();
	bool bRightMouseClicked = false;
	FVector2D MousePos = FVector2D(0, 0);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<AActor*> GetTargetsInRange();

private:
	void InitAbilitySystemComponent();
	void InitHUD();
};
