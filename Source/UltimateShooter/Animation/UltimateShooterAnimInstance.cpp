// Evgenii Esaulenko, Ultimate Shooter, 2024


#include "UltimateShooterAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UltimateShooter/Character/UltimateShooterCharacter.h"

void UUltimateShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	UltimateShooterCharacter = Cast<AUltimateShooterCharacter>(TryGetPawnOwner());
}

void UUltimateShooterAnimInstance::UpdateAnimation(float DeltaSeconds)
{
	if (UltimateShooterCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UltimateShooterCharacter is not set in %s"), *GetName());
		return;
	}

	FVector Velocity = UltimateShooterCharacter->GetVelocity();
	Velocity.Z = 0;
	Speed = Velocity.Size();

	UCharacterMovementComponent* CharacterMovement = UltimateShooterCharacter->GetCharacterMovement();
	bIsInAir = CharacterMovement->IsFalling();
	bIsAccelerating = CharacterMovement->GetCurrentAcceleration().Size() > 0;

	FRotator AimRotation = UltimateShooterCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Velocity);
	MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
}
