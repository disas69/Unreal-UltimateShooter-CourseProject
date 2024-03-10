// Evgenii Esaulenko, Ultimate Shooter, 2024

#pragma once

#include "CoreMinimal.h"
#include "CameraState.h"
#include "Components/ActorComponent.h"
#include "CameraStateComponent.generated.h"

class AUltimateShooterCharacter;
struct FCameraStateConfig;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ULTIMATESHOOTER_API UCameraStateComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UCameraStateComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    FORCEINLINE ECameraState GetCurrentState() const { return CurrentState; }
    FORCEINLINE float GetCurrentSensitivity() const { return CurrentSensitivity; }

    void SetCameraState(ECameraState NewState, bool bInstant = false);

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

private:
    bool bIsInitialized = false;

    const FCameraStateConfig* GetCameraStateConfig(ECameraState State) const;

    UPROPERTY()
    AUltimateShooterCharacter* Character = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    ECameraState CurrentState = ECameraState::Default;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float CurrentSensitivity = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    float CameraTransitionSpeed = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
    TArray<FCameraStateConfig> CameraStates;
};
