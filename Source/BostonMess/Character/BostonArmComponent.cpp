// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BostonArmComponent.h"

#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UBostonArmComponent::UBostonArmComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UBostonArmComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UBostonArmComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBostonArmComponent::Initialize(const FVector2D& horizontalLimits, const FVector2D& verticalLimits)
{
	HorizontalLimits = horizontalLimits;
	VerticalLimits = verticalLimits;
}

void UBostonArmComponent::AddMovementInput(const FVector& movementInput)
{
	if (const USceneComponent* root = GetAttachmentRoot())
	{
		const FVector rootUp = root->GetUpVector();
		const FVector rootRight = root->GetRightVector();
		const FVector rootForward = root->GetForwardVector();
		//const FVector rootLocation = root->GetComponentLocation();

		const FVector updatedLocationRaw = GetRelativeLocation() + movementInput;
		UE_LOG(LogTemp, Display, TEXT("Original Location: %s"), *GetRelativeLocation().ToCompactString());

		const float distanceAlongRight = updatedLocationRaw.Dot(rootRight);
		const float distanceAlongUp = updatedLocationRaw.Dot(rootUp);
		const float distanceAlongForward = updatedLocationRaw.Dot(rootForward);

		const FVector clampedRight = FMath::Clamp(distanceAlongRight, HorizontalLimits.X, HorizontalLimits.Y) * rootRight;
		const FVector clampedUp = FMath::Clamp(distanceAlongUp, VerticalLimits.X, VerticalLimits.Y) * rootUp;
		const FVector forward = distanceAlongForward * rootForward;

		const FVector updateLocationClamped = clampedRight + clampedUp + forward;
		SetRelativeLocation(updateLocationClamped);

		UE_LOG(LogTemp, Display, TEXT("Updated Raw Location: %s"), *updatedLocationRaw.ToCompactString());
		UE_LOG(LogTemp, Display, TEXT("Scalers: %f, %f"), distanceAlongRight, distanceAlongUp);
		UE_LOG(LogTemp, Display, TEXT("Updated clamped Location: %s"), *updateLocationClamped.ToCompactString());

	}
}

