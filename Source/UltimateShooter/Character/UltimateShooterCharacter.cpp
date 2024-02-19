// Evgenii Esaulenko, Ultimate Shooter, 2024


#include "UltimateShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "UltimateShooter/Input/InputDataConfig.h"


// Sets default values
AUltimateShooterCharacter::AUltimateShooterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent.Get());
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	
	SetupCharacterMovement();
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
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* LocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	LocalPlayerSubsystem->ClearAllMappings();
	LocalPlayerSubsystem->AddMappingContext(InputDataConfig->InputMappingContext, 0);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	Input->BindAction(InputDataConfig->MoveForward, ETriggerEvent::Triggered, this, &AUltimateShooterCharacter::MoveForward);
	Input->BindAction(InputDataConfig->MoveRight, ETriggerEvent::Triggered, this, &AUltimateShooterCharacter::MoveRight);
	Input->BindAction(InputDataConfig->TurnRight, ETriggerEvent::Triggered, this, &AUltimateShooterCharacter::TurnAtRate);
	Input->BindAction(InputDataConfig->LookUp, ETriggerEvent::Triggered, this, &AUltimateShooterCharacter::LookUpAtRate);
	Input->BindAction(InputDataConfig->Jump, ETriggerEvent::Triggered, this, &AUltimateShooterCharacter::Jump);
}

void AUltimateShooterCharacter::MoveForward(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (FMath::Abs(AxisValue) >= 0.1f)
	{
		FVector Direction = FRotationMatrix(FollowCamera->GetComponentRotation()).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}
}

void AUltimateShooterCharacter::MoveRight(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (FMath::Abs(AxisValue) >= 0.1f)
	{
		FVector Direction = FRotationMatrix(FollowCamera->GetComponentRotation()).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, AxisValue);
	}
}

void AUltimateShooterCharacter::TurnAtRate(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (FMath::Abs(AxisValue) >= 0.1f)
	{
		float TurnAmount = AxisValue * TurnRate * GetWorld()->GetDeltaSeconds();
		AddControllerYawInput(TurnAmount);
	}
}

void AUltimateShooterCharacter::LookUpAtRate(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (FMath::Abs(AxisValue) >= 0.1f)
	{
		float LookUpAmount = AxisValue * LookUpRate * GetWorld()->GetDeltaSeconds();
		AddControllerPitchInput(LookUpAmount);
	}
}

void AUltimateShooterCharacter::SetupCharacterMovement()
{
	UCharacterMovementComponent* CharacterMovement = GetCharacterMovement();
	CharacterMovement->bOrientRotationToMovement = true;
	CharacterMovement->RotationRate = FRotator(0.0f, 600.0f, 0.0f);
	CharacterMovement->JumpZVelocity = 600.0f;
	CharacterMovement->AirControl = 0.2f;
}

