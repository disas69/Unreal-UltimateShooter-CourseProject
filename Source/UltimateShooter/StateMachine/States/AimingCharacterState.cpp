// Evgenii Esaulenko, Ultimate Shooter, 2024

#include "AimingCharacterState.h"

#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UltimateShooter/Camera/CameraState.h"
#include "UltimateShooter/Camera/CameraStateComponent.h"
#include "UltimateShooter/Character/UltimateShooterCharacter.h"
#include "UltimateShooter/Crosshair/CrosshairComponent.h"

void FAimingCharacterState::Enter(AUltimateShooterCharacter* Character)
{
	FCharacterState::Enter(Character);

	SetupCamera(Character);
	SetupMovement(Character);
}

void FAimingCharacterState::HandleInput(AUltimateShooterCharacter* Character)
{
	FCharacterState::HandleInput(Character);

	if (bIsWeaponFiring)
	{
		return;
	}

	bIsWeaponFiring = true;

	const USkeletalMeshSocket* BarrelSocket = Character->GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket != nullptr)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(Character->GetMesh());
		const FVector SocketLocation = SocketTransform.GetLocation();

		FHitResult FireWeaponHitResult;
		FVector FireWeaponTraceEndLocation;
		FireWeaponTrace(Character, SocketLocation, FireWeaponHitResult, FireWeaponTraceEndLocation);

		AActor* HitActor = FireWeaponHitResult.GetActor();
		if (HitActor != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s"), *HitActor->GetName());
		}
				
		// if (ImpactParticle != nullptr && FireWeaponHitResult.bBlockingHit)
		// {
		// 	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, FireWeaponHitResult.Location, FireWeaponHitResult.ImpactNormal.Rotation());
		// }
		//
		// if (TrailParticle != nullptr)
		// {
		// 	UParticleSystemComponent* Trail = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TrailParticle, SocketLocation, (FireWeaponTraceEndLocation - SocketLocation).Rotation());
		// 	Trail->SetVectorParameter("Target", FireWeaponTraceEndLocation);
		// }
		//
		// if (WeaponFireFX != nullptr)
		// {
		// 	UGameplayStatics::SpawnEmitterAttached(WeaponFireFX, GetMesh(), BarrelSocket->GetFName(), SocketLocation, SocketTransform.GetRotation().Rotator(), EAttachLocation::KeepWorldPosition);
		// }
	}

	OnFireWeaponStarted(Character);
}

void FAimingCharacterState::Update(AUltimateShooterCharacter* Character)
{
	FCharacterState::Update(Character);

	if (bIsWeaponFiring)
	{
		const float Time = Character->GetWorld()->GetTimeSeconds();
		if (Time - FireWeaponStartTime >= Character->WeaponFireRate)
		{
			OnFireWeaponFinished();
		}
	}
}

void FAimingCharacterState::SetupCamera(AUltimateShooterCharacter* Character)
{
	// Character does rotate with the controller
	Character->bUseControllerRotationYaw = true;
	Character->bUseControllerRotationPitch = false;
	Character->bUseControllerRotationRoll = false;

	Character->GetCameraState()->SetCameraState(ECameraState::ZoomIn, false);
}

void FAimingCharacterState::SetupMovement(const AUltimateShooterCharacter* Character)
{
	UCharacterMovementComponent* CharacterMovementComponent = Character->GetCharacterMovement();
	CharacterMovementComponent->bOrientRotationToMovement = false;
	// CharacterMovementComponent->MaxWalkSpeed = 300.0f;
	CharacterMovementComponent->JumpZVelocity = 600.0f;
	CharacterMovementComponent->AirControl = 0.f;
}

void FAimingCharacterState::OnFireWeaponStarted(AUltimateShooterCharacter* Character)
{
	// // Play the fire sound
	// if (WeaponFireSound != nullptr)
	// {
	// 	UGameplayStatics::PlaySoundAtLocation(this, WeaponFireSound, GetActorLocation());
	// }

	// Play the fire animation montage
	Character->PlayAnimMontage(Character->FireAnimation, 1.0f, FName("StartFire"));

	// Spread the crosshair
	Character->GetCrosshair()->SetCrosshairSpread(0.8f);

	// Set a fire rate timer to call OnFireWeaponFinished
	FireWeaponStartTime = Character->GetWorld()->GetTimeSeconds();
}

void FAimingCharacterState::FireWeaponTrace(const AUltimateShooterCharacter* Character, const FVector& WeaponLocation, FHitResult& HitResult, FVector& TraceEndLocation) const
{
	// Get the crosshair location
	const FVector2D CrossLocation = Character->GetCrosshair()->GetLocation();

	FVector WorldDirection;
	FVector WorldLocation;

	// Project the crosshair location to the world
	UGameplayStatics::DeprojectScreenToWorld(Cast<APlayerController>(Character->GetController()), CrossLocation, WorldLocation, WorldDirection);

	// Trace from the crosshair to the target location
	FHitResult ScreenTraceResult;
	const FVector StartLocation = WorldLocation;
	const FVector EndLocation = StartLocation + WorldDirection * Character->WeaponFireRange;
	TraceEndLocation = EndLocation;

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(Character);

	if (Character->GetWorld()->LineTraceSingleByChannel(ScreenTraceResult, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, CollisionQueryParams))
	{
		HitResult = ScreenTraceResult;
		TraceEndLocation = ScreenTraceResult.Location;
	}
		
	// Trace from the weapon to the target location to ensure there's no blocking object in between
	FHitResult WeaponTraceResult;
	const FVector WeaponStartLocation = WeaponLocation;
	const FVector WeaponEndLocation = TraceEndLocation;

	if (Character->GetWorld()->LineTraceSingleByChannel(WeaponTraceResult, WeaponStartLocation, WeaponEndLocation, ECollisionChannel::ECC_Visibility, CollisionQueryParams))
	{
		HitResult = WeaponTraceResult;
		TraceEndLocation = WeaponTraceResult.Location;
	}
}

void FAimingCharacterState::OnFireWeaponFinished()
{
	bIsWeaponFiring = false;
}
