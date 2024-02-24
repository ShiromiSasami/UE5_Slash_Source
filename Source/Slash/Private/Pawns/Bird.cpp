// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputTriggers.h"
#include "InputAction.h"
#include "InputActionValue.h"

// Sets default values
ABird::ABird()
	: Capsule(CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule")))
	, BirdMesh(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh")))
	, CameraBoom(CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom")))
	, ViewCamera(CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera")))
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule->SetCapsuleHalfHeight(20.f);
	Capsule->SetCapsuleRadius(15.f);
	SetRootComponent(Capsule);

	BirdMesh->SetupAttachment(GetRootComponent());

	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	ViewCamera->SetupAttachment(CameraBoom);

	//AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ABird::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABird::MoveForward(const FInputActionValue& Value)
{
	const float Velocity = Value.Get<float>();
	if (Controller && Velocity != 0.f)
	{
		const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, Velocity);
	}
}

void ABird::Turn(const FInputActionValue& Value)
{
	AddControllerYawInput(Value.Get<float>());
}

void ABird::LookUp(const FInputActionValue& Value)
{
	AddControllerPitchInput(Value.Get<float>());
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &ABird::MoveForward);
		Input->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &ABird::Turn);
		Input->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &ABird::LookUp);
	}

	if(APlayerController* controller = Cast<APlayerController>(Controller))
	{
		EnableInput(controller);
		if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(controller->GetLocalPlayer()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (!IMC_Player.IsNull())
				{
					InputSystem->AddMappingContext(IMC_Player, 0);
				}
			}
		}
	}
}

