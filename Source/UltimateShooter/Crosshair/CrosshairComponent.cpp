// Evgenii Esaulenko, Ultimate Shooter, 2024


#include "CrosshairComponent.h"


// Sets default values for this component's properties
UCrosshairComponent::UCrosshairComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UCrosshairComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UCrosshairComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FVector2D UCrosshairComponent::GetLocation() const
{
	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
	return FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2) + CrosshairPositionOffset;
}
