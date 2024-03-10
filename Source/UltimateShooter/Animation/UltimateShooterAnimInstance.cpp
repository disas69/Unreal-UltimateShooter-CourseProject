// Evgenii Esaulenko, Ultimate Shooter, 2024

#include "UltimateShooterAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "UltimateShooter/Character/UltimateShooterCharacter.h"

void UUltimateShooterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Character = Cast<AUltimateShooterCharacter>(TryGetPawnOwner());
}

void UUltimateShooterAnimInstance::UpdateAnimation(float DeltaSeconds)
{
    if (Character == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("UltimateShooterCharacter is not set in %s"), *GetName());
        return;
    }

    FVector Velocity = Character->GetVelocity();
    Velocity.Z = 0;
    Speed = Velocity.Size();

    const UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();
    bIsInAir = CharacterMovement->IsFalling();
    bIsAccelerating = CharacterMovement->GetCurrentAcceleration().Size() > 0;

    bIsAiming = Character->IsAiming();

    const FRotator AimRotation = Character->GetBaseAimRotation();
    const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Velocity);
    MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

    if (Velocity.Size() > 0)
    {
        LastMovementOffsetYaw = MovementOffsetYaw;
    }

    const float MaxVelocity = Character->GetCharacterMovement()->GetMaxSpeed();
    ScaledPlayRate = FMath::Clamp(Velocity.Size2D() / MaxVelocity, 0.3f, 1.0f);
}
