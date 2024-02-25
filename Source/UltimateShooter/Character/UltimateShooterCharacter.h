// Evgenii Esaulenko, Ultimate Shooter, 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UltimateShooterCharacter.generated.h"

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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Input handlers
	void MoveForward(const FInputActionValue& Value);
	void MoveRight(const FInputActionValue& Value);
	void TurnAtRate(const FInputActionValue& Value);
	void LookUpAtRate(const FInputActionValue& Value);
	void FireWeapon();
	void StartAimingWeapon();
	void StopAimingWeapon();
	
	UPROPERTY(EditAnywhere)
	UInputDataConfig* InputDataConfig = nullptr;

private:
	void SetupFollowCharacterMovement() const;
	void SetupFollowCamera();
	void SetupAimingCharacterMovement() const;
	void SetupAimingCamera();
	void OnFireWeaponFinished();

	bool bIsWeaponFiring = false;
	bool bIsAiming = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float TurnRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	float LookUpRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	FVector FollowCameraOffset = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	FVector AimCameraOffset = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	float WeaponFireRange = 10000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	float WeaponFireRate = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class USoundBase* WeaponFireSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* WeaponFireFX = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* FireAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* TrailParticle = nullptr;
};
