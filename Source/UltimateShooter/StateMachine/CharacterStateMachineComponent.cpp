// Evgenii Esaulenko, Ultimate Shooter, 2024

#include "CharacterStateMachineComponent.h"
#include "States/AimingCharacterState.h"
#include "States/IdleCharacterState.h"
#include "UltimateShooter/Character/UltimateShooterCharacter.h"

UCharacterStateMachineComponent::UCharacterStateMachineComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterStateMachineComponent::BeginPlay()
{
    Super::BeginPlay();

    Character = Cast<AUltimateShooterCharacter>(GetOwner());

    StateMap = TMap<ECharacterState, FCharacterState*>{
        {ECharacterState::Idle, new FIdleCharacterState()},
        {ECharacterState::Aiming, new FAimingCharacterState()},
    };

    SetState(ECharacterState::Idle);
}

void UCharacterStateMachineComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    // Clean up the state map
    for (const TTuple<ECharacterState, FCharacterState*>& State : StateMap)
    {
        delete State.Value;
    }

    StateMap.Empty();
}

void UCharacterStateMachineComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    StateMap[CurrentState]->Update(Character);
}

void UCharacterStateMachineComponent::SetState(const ECharacterState NewState)
{
    StateMap[CurrentState]->Exit(Character);
    CurrentState = NewState;
    StateMap[CurrentState]->Enter(Character);
}

void UCharacterStateMachineComponent::HandleActionInput()
{
    StateMap[CurrentState]->HandleInput(Character);
}
