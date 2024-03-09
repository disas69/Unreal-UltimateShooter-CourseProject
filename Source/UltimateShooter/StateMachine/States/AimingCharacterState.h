// Evgenii Esaulenko, Ultimate Shooter, 2024

#pragma once

#include "CoreMinimal.h"
#include "CharacterState.h"

class FAimingCharacterState final : public FCharacterState
{
public:	
	virtual void Enter(AUltimateShooterCharacter* Character) override;
	virtual void HandleInput(AUltimateShooterCharacter* Character) override;
	virtual void Update(AUltimateShooterCharacter* Character) override;

private:
	bool bIsWeaponFiring = false;
	float FireWeaponStartTime = 0.0f;

	void SetupCamera(AUltimateShooterCharacter* Character);
	void SetupMovement(const AUltimateShooterCharacter* Character);

	// Weapon handlers
	void OnFireWeaponStarted(AUltimateShooterCharacter* Character);
	void FireWeaponTrace(const AUltimateShooterCharacter* Character, const FVector& WeaponLocation, FHitResult& HitResult, FVector& TraceEndLocation) const;
	void OnFireWeaponFinished();
};
