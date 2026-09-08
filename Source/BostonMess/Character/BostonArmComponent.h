// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"

#include "BostonArmComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BOSTONMESS_API UBostonArmComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBostonArmComponent();

	void Initialize(const FVector2D& horizontalLimits, const FVector2D& verticalLimits);

	void AddMovementInput(const FVector& movementInput);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	FVector2D HorizontalLimits;

	FVector2D VerticalLimits;
};
