// Evgenii Esaulenko, Ultimate Shooter, 2024

#include "AimingCharacterState.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "UltimateShooter/Camera/CameraState.h"
#include "UltimateShooter/Camera/CameraStateComponent.h"
#include "UltimateShooter/Character/UltimateShooterCharacter.h"
#include "UltimateShooter/Weapon/WeaponHandlerComponent.h"

void FAimingCharacterState::Enter(AUltimateShooterCharacter* Character)
{
    FCharacterState::Enter(Character);

    SetupCamera(Character);
    SetupMovement(Character);
}

void FAimingCharacterState::HandleInput(AUltimateShooterCharacter* Character)
{
    FCharacterState::HandleInput(Character);

    Character->GetWeaponHandler()->FireWeapon();
}

void FAimingCharacterState::SetupCamera(AUltimateShooterCharacter* Character)
{
    // Character does rotate with the controller
    Character->bUseControllerRotationYaw = true;
    Character->bUseControllerRotationPitch = false;
    Character->bUseControllerRotationRoll = false;

    Character->GetCameraState()->SetCameraState(ECameraState::ZoomIn, false);
}

void FAimingCharacterState::SetupMovement(const AUltimateShooterCharacter* Character)
{
    UCharacterMovementComponent* CharacterMovementComponent = Character->GetCharacterMovement();
    CharacterMovementComponent->bOrientRotationToMovement = false;
    // CharacterMovementComponent->MaxWalkSpeed = 300.0f;
    CharacterMovementComponent->JumpZVelocity = 600.0f;
    CharacterMovementComponent->AirControl = 0.f;
}