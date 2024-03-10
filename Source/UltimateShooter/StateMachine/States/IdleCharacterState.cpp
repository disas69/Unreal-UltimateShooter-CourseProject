// Evgenii Esaulenko, Ultimate Shooter, 2024

#include "IdleCharacterState.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "UltimateShooter/Camera/CameraState.h"
#include "UltimateShooter/Camera/CameraStateComponent.h"
#include "UltimateShooter/Character/UltimateShooterCharacter.h"

void FIdleCharacterState::Enter(AUltimateShooterCharacter* Character)
{
    FCharacterState::Enter(Character);

    SetupCamera(Character);
    SetupMovement(Character);
}

void FIdleCharacterState::SetupCamera(AUltimateShooterCharacter* Character)
{
    // Character does not rotate with the controller
    Character->bUseControllerRotationYaw = false;
    Character->bUseControllerRotationPitch = false;
    Character->bUseControllerRotationRoll = false;

    Character->GetCameraState()->SetCameraState(ECameraState::Default, false);
}

void FIdleCharacterState::SetupMovement(const AUltimateShooterCharacter* Character)
{
    UCharacterMovementComponent* CharacterMovementComponent = Character->GetCharacterMovement();
    CharacterMovementComponent->bOrientRotationToMovement = true;
    // CharacterMovementComponent->MaxWalkSpeed = 600.0f;
    CharacterMovementComponent->RotationRate = FRotator(0.0f, 600.0f, 0.0f);
    CharacterMovementComponent->JumpZVelocity = 600.0f;
    CharacterMovementComponent->AirControl = 0.2f;
}
