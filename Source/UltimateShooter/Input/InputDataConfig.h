// Evgenii Esaulenko, Ultimate Shooter, 2024

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputDataConfig.generated.h"

class UInputMappingContext;
class UInputAction;

/**
 * 
 */
UCLASS()
class ULTIMATESHOOTER_API UInputDataConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* MoveForward;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* MoveRight;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* TurnRight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* LookUp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* Jump;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* Fire;
};
