// Evgenii Esaulenko, Ultimate Shooter, 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "States/CharacterState.h"

#include "CharacterStateMachineComponent.generated.h"

class UCharacterState;
class AUltimateShooterCharacter;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle,
	Aiming,
	Reloading,
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ULTIMATESHOOTER_API UCharacterStateMachineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCharacterStateMachineComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE ECharacterState GetCurrentState() const { return CurrentState; }

	void SetState(const ECharacterState NewState);
	void HandleActionInput();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	AUltimateShooterCharacter* Character = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State", meta=(AllowPrivateAccess="true"))
	ECharacterState CurrentState = ECharacterState::Idle;
	
	TMap<ECharacterState, FCharacterState*> StateMap;
};
