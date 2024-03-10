// Evgenii Esaulenko, Ultimate Shooter, 2024

#pragma once

#include "CoreMinimal.h"
#include "CharacterState.h"

class FIdleCharacterState final : public FCharacterState
{
public:
    virtual void Enter(AUltimateShooterCharacter* Character) override;

private:
    void SetupCamera(AUltimateShooterCharacter* Character);
    void SetupMovement(const AUltimateShooterCharacter* Character);
};
