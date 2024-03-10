// Evgenii Esaulenko, Ultimate Shooter, 2024

#include "WeaponHandlerComponent.h"
#include "UltimateShooter/Character/UltimateShooterCharacter.h"
#include "Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "UltimateShooter/Crosshair/CrosshairComponent.h"

// Sets default values for this component's properties
UWeaponHandlerComponent::UWeaponHandlerComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UWeaponHandlerComponent::BeginPlay()
{
    Super::BeginPlay();

    Character = Cast<AUltimateShooterCharacter>(GetOwner());
    Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
}

void UWeaponHandlerComponent::OnFireWeaponStarted()
{
    const FWeaponData& WeaponData = Weapon->GetWeaponData();

    // Play the fire sound
    if (WeaponData.FireSound != nullptr)
    {
        UGameplayStatics::PlaySoundAtLocation(this, WeaponData.FireSound, Character->GetActorLocation());
    }

    // Play the fire animation montage
    Character->PlayAnimMontage(WeaponData.FireAnimation, 1.0f, FName("StartFire"));

    // Spread the crosshair
    Character->GetCrosshair()->SetCrosshairSpread(WeaponData.Spread);

    // Set a fire rate timer to call OnFireWeaponFinished
    FireWeaponStartTime = Character->GetWorld()->GetTimeSeconds();
}

void UWeaponHandlerComponent::FireWeaponTrace(const FVector& WeaponLocation, FHitResult& HitResult, FVector& TraceEndLocation) const
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
    const FVector EndLocation = StartLocation + WorldDirection * Weapon->GetWeaponData().Range;
    TraceEndLocation = EndLocation;

    FCollisionQueryParams CollisionQueryParams;
    CollisionQueryParams.AddIgnoredActor(Character);

    if (Character->GetWorld()->LineTraceSingleByChannel(ScreenTraceResult, StartLocation, EndLocation, ECC_Visibility, CollisionQueryParams))
    {
        HitResult = ScreenTraceResult;
        TraceEndLocation = ScreenTraceResult.Location;
    }

    // Trace from the weapon to the target location to ensure there's no blocking object in between
    FHitResult WeaponTraceResult;
    const FVector WeaponStartLocation = WeaponLocation;
    const FVector WeaponEndLocation = TraceEndLocation;

    if (Character->GetWorld()->LineTraceSingleByChannel(WeaponTraceResult, WeaponStartLocation, WeaponEndLocation, ECC_Visibility, CollisionQueryParams))
    {
        HitResult = WeaponTraceResult;
        TraceEndLocation = WeaponTraceResult.Location;
    }
}

void UWeaponHandlerComponent::OnFireWeaponFinished()
{
    bIsWeaponFiring = false;
}

// Called every frame
void UWeaponHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bIsWeaponFiring)
    {
        const float Time = Character->GetWorld()->GetTimeSeconds();
        if (Time - FireWeaponStartTime >= Weapon->GetWeaponData().FireRate)
        {
            OnFireWeaponFinished();
        }
    }
}

void UWeaponHandlerComponent::FireWeapon()
{
    if (Weapon == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Weapon is not set"));
        return;
    }

    if (bIsWeaponFiring)
    {
        return;
    }

    bIsWeaponFiring = true;

    OnFireWeaponStarted();

    const USkeletalMeshSocket* BarrelSocket = Character->GetMesh()->GetSocketByName("BarrelSocket");
    if (BarrelSocket != nullptr)
    {
        const FTransform SocketTransform = BarrelSocket->GetSocketTransform(Character->GetMesh());
        const FVector SocketLocation = SocketTransform.GetLocation();

        FHitResult FireWeaponHitResult;
        FVector FireWeaponTraceEndLocation;
        FireWeaponTrace(SocketLocation, FireWeaponHitResult, FireWeaponTraceEndLocation);

        const FWeaponData& WeaponData = Weapon->GetWeaponData();

        AActor* HitActor = FireWeaponHitResult.GetActor();
        if (HitActor != nullptr)
        {
            UE_LOG(LogTemp, Warning, TEXT("Hit actor: %s, Damage: %f"), *HitActor->GetName(), WeaponData.Damage);
        }

        if (WeaponData.ImpactParticle != nullptr && FireWeaponHitResult.bBlockingHit)
        {
            UGameplayStatics::SpawnEmitterAtLocation(
                GetWorld(), WeaponData.ImpactParticle, FireWeaponHitResult.Location, FireWeaponHitResult.ImpactNormal.Rotation());
        }

        if (WeaponData.TrailParticle != nullptr)
        {
            UParticleSystemComponent* Trail = UGameplayStatics::SpawnEmitterAtLocation(
                GetWorld(), WeaponData.TrailParticle, SocketLocation, (FireWeaponTraceEndLocation - SocketLocation).Rotation());
            Trail->SetVectorParameter("Target", FireWeaponTraceEndLocation);
        }

        if (WeaponData.FireFX != nullptr)
        {
            UGameplayStatics::SpawnEmitterAttached(WeaponData.FireFX, Character->GetMesh(), BarrelSocket->GetFName(), SocketLocation,
                SocketTransform.GetRotation().Rotator(), EAttachLocation::KeepWorldPosition);
        }
    }
}
