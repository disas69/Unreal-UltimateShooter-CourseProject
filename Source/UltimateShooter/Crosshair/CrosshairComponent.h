// Evgenii Esaulenko, Ultimate Shooter, 2024

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CrosshairComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ULTIMATESHOOTER_API UCrosshairComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCrosshairComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	FVector2D GetLocation() const;

	void SetCrosshairSpread(float NewCrosshairSpread, bool bInstant = false);
	FORCEINLINE float GetCrosshairSpread() const { return CrosshairSpread; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadOffset() const;

protected:
	virtual void BeginPlay() override;

private:
	void UpdateTargetSpread(float DeltaTime);
	
	float CrosshairSpread = 0.0f;
	float TargetCrosshairSpread = 0.0f;

	UPROPERTY()
	class AUltimateShooterCharacter* Character = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	FVector2D CrosshairPositionOffset = FVector2D(0.0f, 60.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	FVector2D CrosshairSpreadRange = FVector2D(20.0f, 100.0f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairChangeSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float CrosshairRecoverySpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair", meta = (AllowPrivateAccess = "true"))
	float DefaultCrosshairSpread = 0.3f;
	
};
