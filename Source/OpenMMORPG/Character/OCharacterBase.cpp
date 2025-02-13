


#include "OCharacterBase.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "../UI/OHud.h"
#include "../Player/OPlayerState.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);


// Sets default values
AOCharacterBase::AOCharacterBase()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	bOrientRotationToMovement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller bOrientRotationToMovement

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(RootComponent);
	Sphere->SetSphereRadius(1500.0f);
}

// Called when the game starts or when spawned
void AOCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}

		PlayerController->bEnableMouseOverEvents = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bShowMouseCursor = true;

}

}

TArray<AActor*> AOCharacterBase::GetTargetsInRange()
{
	TArray<AActor*> Targets;
	Sphere->GetOverlappingActors(Targets, ACharacter::StaticClass());
	return Targets;
}

void AOCharacterBase::InitAbilitySystemComponent()
{
	AOPlayerState* PState = GetPlayerState<AOPlayerState>();
	if (!PState) return;
	AbilitySystemComponent = CastChecked<UGAS_AbilitySystemComponent>(
		PState->GetAbilitySystemComponent());
	AbilitySystemComponent->InitAbilityActorInfo(PState, this);
	Attributes = PState->GetAttributeSet();
}

void AOCharacterBase::InitHUD()
{
	if (const APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		if (AOHud* PlayerHUD = Cast<AOHud>(PlayerController->GetHUD())) {
			PlayerHUD->Init();
		}
	}
}

// Called every frame
void AOCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AOCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOCharacterBase::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOCharacterBase::Look);
		EnhancedInputComponent->BindAction(RightAction, ETriggerEvent::Started, this, &AOCharacterBase::OnRightClickPressed);
		EnhancedInputComponent->BindAction(RightAction, ETriggerEvent::Completed, this, &AOCharacterBase::OnRightClickReleased);
		EnhancedInputComponent->BindAction(CameraZoomAction, ETriggerEvent::Triggered, this, &AOCharacterBase::CameraZoom);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

}

void AOCharacterBase::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator CamRotate = CameraBoom->GetTargetRotation();
		const FRotator CamYaw(0, CamRotate.Yaw, 0);
		SetActorRotation(CamYaw);
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

	}
}
void AOCharacterBase::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr && bRightMouseClicked)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AOCharacterBase::CameraZoom(const FInputActionValue& Value)
{
	const float ArmLength = Value.Get<float>();
	if (Controller != nullptr) {
		const float NewTargetArmLength = CameraBoom->TargetArmLength + ArmLength * 10.0f;
		CameraBoom->TargetArmLength = FMath::Clamp(NewTargetArmLength, 100.0f, 600.0f);
	}
}

void AOCharacterBase::OnRightClickPressed()
{
	bRightMouseClicked = true;
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		double mouseX, mouseY;
		PlayerController->GetMousePosition(mouseX, mouseY);
		PlayerController->bEnableMouseOverEvents = false;
		PlayerController->bEnableClickEvents = false;
		PlayerController->bShowMouseCursor = false;
		MousePos = FVector2D(mouseX, mouseY);
	}
}

void AOCharacterBase::OnRightClickReleased()
{
	bRightMouseClicked = false;
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		PlayerController->bEnableMouseOverEvents = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bShowMouseCursor = true;
		PlayerController->SetMouseLocation(MousePos.X, MousePos.Y);
	}
}

// Called to bind functionality to input
UAbilitySystemComponent* AOCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AOCharacterBase::InitializeAttributes()
{
	if (AbilitySystemComponent && DefaultAttributeEffect)
	{
		// Create context object for this gameplay effecct
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		// Create an outgoing effect spec using the effect to apply and the context
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, EffectContext);

		if (SpecHandle.IsValid())
		{
			// Apply the effect using the derived spec
			// + Could be ApplyGameplayEffectToTarget() instead if we were shooting a target
			FActiveGameplayEffectHandle GEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void AOCharacterBase::GiveAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (TSubclassOf<UGAS_GameplayAbility>& StartupAbility : DefaultAbilities)
		{
			if (!StartupAbility)
			{
				continue;
			}

			AbilitySystemComponent->GiveAbility(
				FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
		}
	}
}

void AOCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Owner and Avatar are bother this character
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitAbilitySystemComponent();
	GiveAbilities();
	InitializeAttributes();
	InitHUD();
}

void AOCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitAbilitySystemComponent();
	InitializeAttributes();
	InitHUD();

	if (AbilitySystemComponent && InputComponent)
	{
		// Where the 3rd parameter is a string equal to enum typename defined in unrealgame5.h
		const FGameplayAbilityInputBinds Binds("Confirm", "Cancel", "EGASAbilityInputID", static_cast<int32>(EGASAbilityInputID::Confirm), static_cast<int32>(EGASAbilityInputID::Cancel));
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
}
