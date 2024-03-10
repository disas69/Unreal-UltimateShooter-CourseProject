// Evgenii Esaulenko, Ultimate Shooter, 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponHandlerComponent.generated.h"

class AUltimateShooterCharacter;
class AWeapon;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ULTIMATESHOOTER_API UWeaponHandlerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UWeaponHandlerComponent();

    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void FireWeapon();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

private:
    bool bIsWeaponFiring = false;
    float FireWeaponStartTime = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<AWeapon> WeaponClass = nullptr;

    UPROPERTY()
    AUltimateShooterCharacter* Character = nullptr;

    UPROPERTY()
    AWeapon* Weapon = nullptr;

    // Weapon handlers
    void OnFireWeaponStarted();
    void FireWeaponTrace(const FVector& WeaponLocation, FHitResult& HitResult, FVector& TraceEndLocation) const;
    void OnFireWeaponFinished();
};
