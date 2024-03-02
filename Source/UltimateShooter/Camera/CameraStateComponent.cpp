// Evgenii Esaulenko, Ultimate Shooter, 2024


#include "CameraStateComponent.h"
#include "CameraState.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UltimateShooter/Character/UltimateShooterCharacter.h"


UCameraStateComponent::UCameraStateComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UCameraStateComponent::SetCameraState(const ECameraState NewState, bool bInstant)
{
	if (bIsInitialized && CurrentState == NewState)
	{
		return;
	}

	bIsInitialized = true;
	CurrentState = NewState;
	
	const FCameraStateConfig* CameraStateConfig = GetCameraStateConfig(CurrentState);
	if (CameraStateConfig != nullptr && Character != nullptr)
	{
		if (bInstant)
		{
			Character->GetCameraBoom()->SocketOffset = CameraStateConfig->CameraOffset;
			Character->GetFollowCamera()->SetFieldOfView(CameraStateConfig->CameraFOV);
		}

		CurrentSensitivity = CameraStateConfig->CameraSensitivity;
	}
}

void UCameraStateComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<AUltimateShooterCharacter>(GetOwner());
	if (Character == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Camera state component owner is not a character)"));
	}
}

const FCameraStateConfig* UCameraStateComponent::GetCameraStateConfig(const ECameraState State) const
{
	const FCameraStateConfig* CameraStateConfig = CameraStates.FindByPredicate([State](const FCameraStateConfig& Config) { return Config.CameraState == State; });
	if (CameraStateConfig == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Camera state config not found)"));
	}

	return CameraStateConfig;
}

void UCameraStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const FCameraStateConfig* CameraStateConfig = GetCameraStateConfig(CurrentState);

	if (CameraStateConfig != nullptr && Character != nullptr)
	{
		const FVector CameraCurrentOffset = FMath::VInterpTo(Character->GetCameraBoom()->SocketOffset, CameraStateConfig->CameraOffset, DeltaTime, CameraTransitionSpeed);
		Character->GetCameraBoom()->SocketOffset = CameraCurrentOffset;

		const float CameraCurrentFOV = FMath::FInterpTo(Character->GetFollowCamera()->FieldOfView, CameraStateConfig->CameraFOV, DeltaTime, CameraTransitionSpeed);
		Character->GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
	}
}
