// Evgenii Esaulenko, Ultimate Shooter, 2024


#include "UltimateShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "DrawDebugHelpers.h"
#include "UltimateShooter/Camera/CameraStateComponent.h"
#include "UltimateShooter/Crosshair/CrosshairComponent.h"

#include "UltimateShooter/Input/InputDataConfig.h"
#include "UltimateShooter/StateMachine/CharacterStateMachineComponent.h"
#include "UltimateShooter/Weapon/WeaponHandlerComponent.h"


// Sets default values
AUltimateShooterCharacter::AUltimateShooterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StateMachine = CreateDefaultSubobject<UCharacterStateMachineComponent>(TEXT("StateMachine"));
	WeaponHandler = CreateDefaultSubobject<UWeaponHandlerComponent>(TEXT("WeaponHandler"));

	// CameraBoom follows the controller rotation
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent.Get());
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;
	
	// Camera follows the CameraBoom
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	CameraState = CreateDefaultSubobject<UCameraStateComponent>(TEXT("CameraState"));
	Crosshair = CreateDefaultSubobject<UCrosshairComponent>(TEXT("Crosshair"));
}

// Called when the game starts or when spawned
void AUltimateShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AUltimateShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AUltimateShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (InputDataConfig == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("InputDataConfig is not set in %s"), *GetName());
		return;
	}

	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	LocalPlayerSubsystem->ClearAllMappings();
	LocalPlayerSubsystem->AddMappingContext(InputDataConfig->InputMappingContext, 0);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(InputDataConfig->MoveForward, ETriggerEvent::Triggered, this, &AUltimateShooterCharacter::MoveForward);
	Input->BindAction(InputDataConfig->MoveRight, ETriggerEvent::Triggered, this, &AUltimateShooterCharacter::MoveRight);
	Input->BindAction(InputDataConfig->TurnRight, ETriggerEvent::Triggered, this, &AUltimateShooterCharacter::TurnAtRate);
	Input->BindAction(InputDataConfig->LookUp, ETriggerEvent::Triggered, this, &AUltimateShooterCharacter::LookUpAtRate);
	Input->BindAction(InputDataConfig->Jump, ETriggerEvent::Triggered, this, &AUltimateShooterCharacter::Jump);
	Input->BindAction(InputDataConfig->Fire, ETriggerEvent::Triggered, this, &AUltimateShooterCharacter::PerformAction);
	Input->BindAction(InputDataConfig->Aim, ETriggerEvent::Started, this, &AUltimateShooterCharacter::StartAimingWeapon);
	Input->BindAction(InputDataConfig->Aim, ETriggerEvent::Completed, this, &AUltimateShooterCharacter::StopAimingWeapon);
}

void AUltimateShooterCharacter::MoveForward(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();
	if (FMath::Abs(AxisValue) >= 0.1f)
	{
		// Get the forward direction based on the camera orientation and apply movement
		FVector Direction = FRotationMatrix(FollowCamera->GetComponentRotation()).GetScaledAxis(EAxis::X);
		Direction.Z = 0;
		Direction.Normalize();
		AddMovementInput(Direction, AxisValue);
	}

	MovementInput.X = AxisValue;
}

void AUltimateShooterCharacter::MoveRight(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();
	if (FMath::Abs(AxisValue) >= 0.1f)
	{
		// Get the right direction based on the camera orientation and apply movement
		FVector Direction = FRotationMatrix(FollowCamera->GetComponentRotation()).GetScaledAxis(EAxis::Y);
		Direction.Z = 0;
		Direction.Normalize();
		AddMovementInput(Direction, AxisValue);
	}

	MovementInput.Y = AxisValue;
}

void AUltimateShooterCharacter::TurnAtRate(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();
	if (FMath::Abs(AxisValue) >= 0.1f)
	{
		const float TurnAmount = AxisValue * TurnRate * CameraState->GetCurrentSensitivity() * GetWorld()->GetDeltaSeconds();
		AddControllerYawInput(TurnAmount);
	}
}

void AUltimateShooterCharacter::LookUpAtRate(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();
	if (FMath::Abs(AxisValue) >= 0.1f)
	{
		const float LookUpAmount = AxisValue * LookUpRate * CameraState->GetCurrentSensitivity() * GetWorld()->GetDeltaSeconds();
		AddControllerPitchInput(LookUpAmount);
	}
}

void AUltimateShooterCharacter::PerformAction()
{
	StateMachine->HandleActionInput();
}

void AUltimateShooterCharacter::StartAimingWeapon()
{
	StateMachine->SetState(ECharacterState::Aiming);
}

void AUltimateShooterCharacter::StopAimingWeapon()
{
	StateMachine->SetState(ECharacterState::Idle);
}
