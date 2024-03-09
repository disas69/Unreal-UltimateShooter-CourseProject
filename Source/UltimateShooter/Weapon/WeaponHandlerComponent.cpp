// Evgenii Esaulenko, Ultimate Shooter, 2024

#include "WeaponHandlerComponent.h"

// Sets default values for this component's properties
UWeaponHandlerComponent::UWeaponHandlerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UWeaponHandlerComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UWeaponHandlerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWeaponHandlerComponent::FireWeapon()
{
	if (Weapon == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon is nullptr"));
		return;
	}

	
}
