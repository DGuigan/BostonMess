// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/BostonMessCharacter.h"

#include "Animation/AnimInstance.h"
#include "BostonMess.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"

#include "Character/BostonArmComponent.h"

ABostonMessCharacter::ABostonMessCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	// Configure arm components
	LeftArm = CreateDefaultSubobject<UBostonArmComponent>(TEXT("LeftArm"));
	LeftArm->SetupAttachment(FirstPersonMesh);

	RightArm = CreateDefaultSubobject<UBostonArmComponent>(TEXT("RightArm"));
	RightArm->SetupAttachment(FirstPersonMesh);
}

void ABostonMessCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	LeftArm->Initialize(ArmMovementLimitsHorizontal, ArmMovementLimitsVertical);
	LeftArm->AddMovementInput(FVector::ZeroVector);

	const FVector2D reversedHorizontalLimits = { -ArmMovementLimitsHorizontal.Y, -ArmMovementLimitsHorizontal.X };
	RightArm->Initialize(reversedHorizontalLimits, ArmMovementLimitsVertical);
	RightArm->AddMovementInput(FVector::ZeroVector);
}


void ABostonMessCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABostonMessCharacter::DoJumpStart);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABostonMessCharacter::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveLeftArmAction, ETriggerEvent::Triggered, this, &ABostonMessCharacter::MoveLeftArmInput);
		EnhancedInputComponent->BindAction(MoveRightArmAction, ETriggerEvent::Triggered, this, &ABostonMessCharacter::MoveRightArmInput);
	}
	else
	{
		UE_LOG(LogBostonMess, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ABostonMessCharacter::MoveLeftArmInput(const FInputActionValue& Value)
{
	MoveArmInput(Value, EArmType::Left);
}

void ABostonMessCharacter::MoveRightArmInput(const FInputActionValue& Value)
{
	MoveArmInput(Value, EArmType::Right);
}


void ABostonMessCharacter::MoveArmInput(const FInputActionValue& Value, EArmType armType)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	UBostonArmComponent* arm = armType == EArmType::Left ? LeftArm : RightArm;
	FString name = armType == EArmType::Left ? FString("Left") : FString("Right");

	const FVector upDelta = GetActorUpVector() * MovementVector.Y;
	const FVector rightDelta = GetActorRightVector() * MovementVector.X;
	const FVector finalDelta = (upDelta + rightDelta) * 5.f;

	UE_LOG(LogBostonMess, Display, TEXT("%s arm: X: %f, Y: %f"), *name, MovementVector.X, MovementVector.Y);

	arm->AddMovementInput(finalDelta);

}

void ABostonMessCharacter::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void ABostonMessCharacter::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ABostonMessCharacter::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void ABostonMessCharacter::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void ABostonMessCharacter::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}
