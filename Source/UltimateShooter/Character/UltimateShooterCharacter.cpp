// Evgenii Esaulenko, Ultimate Shooter, 2024


#include "UltimateShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "UltimateShooter/Input/InputDataConfig.h"


// Sets default values
AUltimateShooterCharacter::AUltimateShooterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// CameraBoom follows the controller rotation
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent.Get());
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Camera follows the CameraBoom
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Character does not rotate with the controller
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
	Input->BindAction(InputDataConfig->Fire, ETriggerEvent::Triggered, this, &AUltimateShooterCharacter::FireWeapon);
}

void AUltimateShooterCharacter::MoveForward(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (FMath::Abs(AxisValue) >= 0.1f)
	{
		// Get the forward direction based on the camera orientation and apply movement
		FVector Direction = FRotationMatrix(FollowCamera->GetComponentRotation()).GetScaledAxis(EAxis::X);
		Direction.Z = 0;
		Direction.Normalize();
		AddMovementInput(Direction, AxisValue);
	}
}

void AUltimateShooterCharacter::MoveRight(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();
	if (FMath::Abs(AxisValue) >= 0.1f)
	{
		// Get the right direction based on the camera orientation and apply movement
		FVector Direction = FRotationMatrix(FollowCamera->GetComponentRotation()).GetScaledAxis(EAxis::Y);
		Direction.Z = 0;
		Direction.Normalize();
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

void AUltimateShooterCharacter::FireWeapon()
{
	if (WeaponFireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, WeaponFireSound, GetActorLocation());
	}

	if (WeaponFireFX != nullptr)
	{
		const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
		if (BarrelSocket != nullptr)
		{
			const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
			UGameplayStatics::SpawnEmitterAttached(WeaponFireFX, GetMesh(), BarrelSocket->GetFName(), SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator(), EAttachLocation::KeepWorldPosition);
		}
	}
}

void AUltimateShooterCharacter::SetupCharacterMovement() const
{
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->bOrientRotationToMovement = true;
	CharacterMovementComponent->RotationRate = FRotator(0.0f, 600.0f, 0.0f);
	CharacterMovementComponent->JumpZVelocity = 600.0f;
	CharacterMovementComponent->AirControl = 0.2f;
}

