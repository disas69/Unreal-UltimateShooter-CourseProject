// Evgenii Esaulenko, Ultimate Shooter, 2024

#pragma once

#include "CoreMinimal.h"
#include "CameraState.generated.h"

UENUM(BlueprintType)
enum class ECameraState : uint8
{
    Default,
    ZoomIn,
    ZoomOut
};

USTRUCT(BlueprintType)
struct ULTIMATESHOOTER_API FCameraStateConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ECameraState CameraState = ECameraState::Default;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector CameraOffset = FVector(0.0f, 0.0f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float CameraFOV = 90.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float CameraSensitivity = 1.0f;
};
