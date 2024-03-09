// Evgenii Esaulenko, Ultimate Shooter, 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

class AUltimateShooterCharacter;

class FCharacterState
{
public:
	virtual ~FCharacterState() = default;
	virtual void Enter(AUltimateShooterCharacter* Character) { }
	virtual void Update(AUltimateShooterCharacter* Character) { }
	virtual void HandleInput(AUltimateShooterCharacter* Character) { }
	virtual void Exit(AUltimateShooterCharacter* Character) { }
};
