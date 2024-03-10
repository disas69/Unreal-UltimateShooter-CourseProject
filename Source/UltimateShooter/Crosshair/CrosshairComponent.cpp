// Evgenii Esaulenko, Ultimate Shooter, 2024

#include "CrosshairComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "UltimateShooter/Character/UltimateShooterCharacter.h"

UCrosshairComponent::UCrosshairComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

float UCrosshairComponent::GetCrosshairSpreadOffset() const
{
    return FMath::Lerp(CrosshairSpreadRange.X, CrosshairSpreadRange.Y, CrosshairSpread);
}

void UCrosshairComponent::BeginPlay()
{
    Super::BeginPlay();

    Character = Cast<AUltimateShooterCharacter>(GetOwner());
    if (Character == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Crosshair component owner is not a character)"));
    }

    SetCrosshairSpread(DefaultCrosshairSpread, true);
}

void UCrosshairComponent::UpdateTargetSpread(float DeltaTime)
{
    if (Character == nullptr)
    {
        return;
    }

    const float Velocity = Character->GetCharacterMovement()->Velocity.Size2D();
    const float MaxVelocity = Character->GetCharacterMovement()->GetMaxSpeed();

    float Spread = FMath::Clamp(Velocity / MaxVelocity, DefaultCrosshairSpread, 1.0f);
    if (Character->GetMovementComponent()->IsFalling())
    {
        Spread = 1.0f;
    }

    SetCrosshairSpread(Spread);

    // Interpolate crosshair spread to target value
    CrosshairSpread = FMath::FInterpTo(CrosshairSpread, TargetCrosshairSpread, DeltaTime, CrosshairChangeSpeed);

    // Recover target crosshair spread to default value
    TargetCrosshairSpread = FMath::FInterpTo(TargetCrosshairSpread, DefaultCrosshairSpread, DeltaTime, CrosshairRecoverySpeed);
}

void UCrosshairComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateTargetSpread(DeltaTime);
}

FVector2D UCrosshairComponent::GetLocation() const
{
    FVector2D ViewportSize;
    GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
    return FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2) + CrosshairPositionOffset;
}

void UCrosshairComponent::SetCrosshairSpread(float NewCrosshairSpread, bool bInstant)
{
    if (bInstant)
    {
        CrosshairSpread = NewCrosshairSpread;
        TargetCrosshairSpread = NewCrosshairSpread;
    }
    else
    {
        if (NewCrosshairSpread > TargetCrosshairSpread)
        {
            TargetCrosshairSpread = NewCrosshairSpread;
        }
    }
}
