// Evgenii Esaulenko, Ultimate Shooter, 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UltimateShooter/StateMachine/CharacterStateMachineComponent.h"
#include "UltimateShooterCharacter.generated.h"

class UWeaponHandlerComponent;
class UCharacterStateMachineComponent;
class UCrosshairComponent;
class UCameraStateComponent;
struct FInputActionValue;
class UCameraComponent;
class USpringArmComponent;
class UInputDataConfig;

UCLASS()
class ULTIMATESHOOTER_API AUltimateShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUltimateShooterCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	FORCEINLINE UCharacterStateMachineComponent* GetStateMachine() const { return StateMachine; }
	FORCEINLINE UWeaponHandlerComponent* GetWeaponHandler() const { return WeaponHandler; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UCameraStateComponent* GetCameraState() const { return CameraState; }

	UFUNCTION(BlueprintCallable)
	bool IsAiming() const { return StateMachine->GetCurrentState() == ECharacterState::Aiming; }

	UFUNCTION(BlueprintCallable)
	UCrosshairComponent* GetCrosshair() const { return Crosshair; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Input handlers
	void MoveForward(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);
	void TurnAtRate(const FInputActionValue& Value);
	void LookUpAtRate(const FInputActionValue& Value);
	void PerformAction();
	void StartAimingWeapon();
	void StopAimingWeapon();
	
	UPROPERTY(EditAnywhere)
	UInputDataConfig* InputDataConfig = nullptr;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State Machine", meta = (AllowPrivateAccess = "true"))
	UCharacterStateMachineComponent* StateMachine = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UWeaponHandlerComponent* WeaponHandler = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraStateComponent* CameraState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCrosshairComponent* Crosshair;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float TurnRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float LookUpRate = 45.0f;
};
