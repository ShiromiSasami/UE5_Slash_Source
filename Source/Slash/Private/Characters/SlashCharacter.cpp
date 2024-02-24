// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"

#include "Items/Weapons/Weapon.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Components/AttributeComponent.h"
#include "Items/Treasure.h"
#include "Items/Soul.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GroomComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "InputTriggers.h"
#include "InputAction.h"
#include "InputActionValue.h"

ASlashCharacter::ASlashCharacter()
	: CameraBoom(CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom")))
	, ViewCamera(CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera")))
	, Hair(CreateDefaultSubobject<UGroomComponent>(TEXT("Hair")))
	, Eyebrows(CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows")))
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	ViewCamera->SetupAttachment(CameraBoom);

	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("EngageableTarget"));

	InitializeSlashOverlay();
}

void ASlashCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

void ASlashCharacter::InitializeSlashOverlay()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD()))
		{
			if (SlashOverlay = SlashHUD->GetSlashOverlay())
			{
				SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SlashOverlay->SetStaminaBarPercent(1.f);
				SlashOverlay->SetGold(0);
				SlashOverlay->SetSouls(0);
			}
		}
	}
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Attributes && SlashOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(IA_MoveForward, ETriggerEvent::Triggered, this, &ASlashCharacter::MoveForward);
		EIC->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &ASlashCharacter::MoveRight);
		EIC->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &ASlashCharacter::Turn);
		EIC->BindAction(IA_LookUp, ETriggerEvent::Triggered, this, &ASlashCharacter::LookUp);
		EIC->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ASlashCharacter::Jump);
		EIC->BindAction(IA_Equip, ETriggerEvent::Triggered, this, &ASlashCharacter::EKeyPressed);
		EIC->BindAction(IA_Attack, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);
		EIC->BindAction(IA_Dodge, ETriggerEvent::Triggered, this, &ASlashCharacter::Dodge);
	}
	if (APlayerController* controller = Cast<APlayerController>(Controller))
	{
		EnableInput(controller);
		if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(controller->GetLocalPlayer()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (IMC_Player != nullptr)
				{
					InputSystem->AddMappingContext(IMC_Player, 0);
				}
			}
		}
	}

}

float ASlashCharacter::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
	
}

void ASlashCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}
}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSouls(Attributes->GetSouls());
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SlashOverlay->SetGold(Attributes->GetGold());
	}
}

void ASlashCharacter::MoveForward(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) { return; }

	const float Velocity = Value.Get<float>();
	if (Controller && Velocity != 0.f)
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Velocity);
	}
}

void ASlashCharacter::MoveRight(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) { return; }

	const float Velocity = Value.Get<float>();
	if (Controller && Velocity != 0.f)
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Velocity);
	}
}

void ASlashCharacter::Turn(const FInputActionValue& Value)
{
	AddControllerYawInput(Value.Get<float>());
}

void ASlashCharacter::LookUp(const FInputActionValue& Value)
{
	AddControllerPitchInput(Value.Get<float>());
}

void ASlashCharacter::EKeyPressed()
{
	if (AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem))
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (CanDisarm())
		{
			Disarm();
		}
		else if(CanArm())
		{
			Arm();
		}
	}
}

void ASlashCharacter::Dodge()
{
	if (!IsUnoccupied() || !HasStamina()) { return; }

	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if (Attributes && SlashOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASlashCharacter::Attack()
{
	Super::Attack();

	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

void ASlashCharacter::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ASlashCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::CanAttack()
{
	return 
		ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);

	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

bool ASlashCharacter::CanDisarm() const
{
	return 
		ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

bool ASlashCharacter::CanArm() const
{
	return
		ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

void ASlashCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

bool ASlashCharacter::HasStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

void ASlashCharacter::SetHUDHealth()
{
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

bool ASlashCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}
