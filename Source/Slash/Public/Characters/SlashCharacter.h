// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"

class UGroomComponent;
class UInputAction;

struct FInputActionValue;



UCLASS()
class SLASH_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void Jump() override;
	virtual void SetOverlappingItem(class AItem* Item) override;
	virtual void AddSouls(class ASoul* Soul) override;
	virtual void AddGold(class ATreasure* Treasure) override;
	
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const{ return ActionState; }
protected:
	virtual void BeginPlay() override;

	virtual void Die_Implementation() override;

private:
	void InitializeSlashOverlay();
	void SetHUDHealth();

#pragma region Input Bind Function

	UFUNCTION()
	void MoveForward(const FInputActionValue& Value);
	UFUNCTION()
	void MoveRight(const FInputActionValue& Value);
	UFUNCTION()
	void Turn(const FInputActionValue& Value);
	UFUNCTION()
	void LookUp(const FInputActionValue& Value);
	UFUNCTION()
	void EKeyPressed();
	UFUNCTION()
	void Dodge();

#pragma endregion

#pragma region Override Function
	//Attack
	virtual void Attack() override;
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;

	//Dodge
	virtual void DodgeEnd() override;
#pragma endregion

#pragma region Montage Function

	void PlayEquipMontage(FName SectionName);

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	UFUNCTION(BlueprintCallable)
	void HitReactEnd();

	void EquipWeapon(AWeapon* Weapon);
	bool CanDisarm() const;
	bool CanArm() const;
	void Disarm();
	void Arm();
	bool HasStamina();
	bool IsUnoccupied();

#pragma endregion

private:

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCameraComponent> ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	TObjectPtr<UGroomComponent> Hair;

	UPROPERTY(VisibleAnywhere, Category = Hair)
	TObjectPtr<UGroomComponent> Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<class AItem> OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<class UAnimMontage> EquipMontage;

	UPROPERTY()
	TObjectPtr<class USlashOverlay> SlashOverlay;

#pragma region Input
private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> IMC_Player;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_MoveForward;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_MoveRight;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Turn;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_LookUp;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Jump;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Equip;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Attack;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Dodge;

#pragma endregion

};
