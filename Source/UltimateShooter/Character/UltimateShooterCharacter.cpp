// Evgenii Esaulenko, Ultimate Shooter, 2024


#include "UltimateShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "UltimateShooter/Camera/CameraStateComponent.h"
#include "UltimateShooter/Crosshair/CrosshairComponent.h"

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

	CameraState = CreateDefaultSubobject<UCameraStateComponent>(TEXT("CameraState"));
	Crosshair = CreateDefaultSubobject<UCrosshairComponent>(TEXT("Crosshair"));
}

// Called when the game starts or when spawned
void AUltimateShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetupFollowCamera(true);
	SetupFollowCharacterMovement();
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
	Input->BindAction(InputDataConfig->Fire, ETriggerEvent::Triggered, this, &AUltimateShooterCharacter::FireWeapon);
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

void AUltimateShooterCharacter::FireWeapon()
{
	if (!IsAiming() || bIsWeaponFiring)
	{
		return;
	}

	bIsWeaponFiring = true;

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket != nullptr)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
		const FVector SocketLocation = SocketTransform.GetLocation();

		FHitResult FireWeaponHitResult;
		FVector FireWeaponTraceEndLocation;
		FireWeaponTrace(SocketLocation, FireWeaponHitResult, FireWeaponTraceEndLocation);

		AActor* HitActor = FireWeaponHitResult.GetActor();
		if (HitActor != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s"), *HitActor->GetName());
		}
				
		if (ImpactParticle != nullptr && FireWeaponHitResult.bBlockingHit)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, FireWeaponHitResult.Location, FireWeaponHitResult.ImpactNormal.Rotation());
		}

		if (TrailParticle != nullptr)
		{
			UParticleSystemComponent* Trail = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TrailParticle, SocketLocation, (FireWeaponTraceEndLocation - SocketLocation).Rotation());
			Trail->SetVectorParameter("Target", FireWeaponTraceEndLocation);
		}

		if (WeaponFireFX != nullptr)
		{
			UGameplayStatics::SpawnEmitterAttached(WeaponFireFX, GetMesh(), BarrelSocket->GetFName(), SocketLocation, SocketTransform.GetRotation().Rotator(), EAttachLocation::KeepWorldPosition);
		}
	}

	OnFireWeaponStarted();
}

void AUltimateShooterCharacter::StartAimingWeapon()
{
	bIsAiming = true;
	SetupAimingCamera(false);
	SetupAimingCharacterMovement();
}

void AUltimateShooterCharacter::StopAimingWeapon()
{
	bIsAiming = false;
	SetupFollowCamera(false);
	SetupFollowCharacterMovement();
}

void AUltimateShooterCharacter::OnFireWeaponStarted()
{
	// Play the fire sound
	if (WeaponFireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, WeaponFireSound, GetActorLocation());
	}

	// Play the fire animation montage
	PlayAnimMontage(FireAnimation, 1.0f, FName("StartFire"));

	// Set a fire rate timer to call OnFireWeaponFinished
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AUltimateShooterCharacter::OnFireWeaponFinished, WeaponFireRate, false);
}

void AUltimateShooterCharacter::FireWeaponTrace(const FVector& WeaponLocation, FHitResult& HitResult, FVector& TraceEndLocation) const
{
	// Get the crosshair location
	const FVector2D CrossLocation = Crosshair->GetLocation();

	FVector WorldDirection;
	FVector WorldLocation;

	// Project the crosshair location to the world
	UGameplayStatics::DeprojectScreenToWorld(Cast<APlayerController>(GetController()), CrossLocation, WorldLocation, WorldDirection);

	// Trace from the crosshair to the target location
	FHitResult ScreenTraceResult;
	const FVector StartLocation = WorldLocation;
	const FVector EndLocation = StartLocation + WorldDirection * WeaponFireRange;
	TraceEndLocation = EndLocation;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(ScreenTraceResult, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, CollisionQueryParams))
	{
		HitResult = ScreenTraceResult;
		TraceEndLocation = ScreenTraceResult.Location;
	}
		
	// Trace from the weapon to the target location to ensure there's no blocking object in between
	FHitResult WeaponTraceResult;
	const FVector WeaponStartLocation = WeaponLocation;
	const FVector WeaponEndLocation = TraceEndLocation;

	if (GetWorld()->LineTraceSingleByChannel(WeaponTraceResult, WeaponStartLocation, WeaponEndLocation, ECollisionChannel::ECC_Visibility, CollisionQueryParams))
	{
		HitResult = WeaponTraceResult;
		TraceEndLocation = WeaponTraceResult.Location;
	}
}

void AUltimateShooterCharacter::OnFireWeaponFinished()
{
	bIsWeaponFiring = false;
}

void AUltimateShooterCharacter::SetupFollowCharacterMovement() const
{
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->bOrientRotationToMovement = true;
	// CharacterMovementComponent->MaxWalkSpeed = 600.0f;
	CharacterMovementComponent->RotationRate = FRotator(0.0f, 600.0f, 0.0f);
	CharacterMovementComponent->JumpZVelocity = 600.0f;
	CharacterMovementComponent->AirControl = 0.2f;
}

void AUltimateShooterCharacter::SetupFollowCamera(bool bInstant)
{
	// Character does not rotate with the controller
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	CameraState->SetCameraState(ECameraState::Default, bInstant);
}

void AUltimateShooterCharacter::SetupAimingCharacterMovement() const
{
	UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement();
	CharacterMovementComponent->bOrientRotationToMovement = false;
	// CharacterMovementComponent->MaxWalkSpeed = 300.0f;
	CharacterMovementComponent->JumpZVelocity = 0.f;
	CharacterMovementComponent->AirControl = 0.f;
}

void AUltimateShooterCharacter::SetupAimingCamera(bool bInstant)
{
	// Character does rotate with the controller
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	CameraState->SetCameraState(ECameraState::ZoomIn, bInstant);
}
