// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;
class USoundBase;
class UParticleSystem;

UCLASS()
class SLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

protected:
	virtual void BeginPlay() override;

#pragma region Combat Virtual Function
	UFUNCTION()
	virtual void Attack();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	virtual bool CanAttack();

	UFUNCTION(BlueprintCallable)
	virtual void DodgeEnd();

	UFUNCTION(BlueprintNativeEvent)
	void Die();
	virtual void HandleDamage(float DamageAmount);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
#pragma endregion

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget() const;

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget() const;

	void StopAttackMontage();
	void PlayHitReactMontage(const FName& SectionName);
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	void DisableCapsule();
	void DisableMeshCollision();

	bool IsAlive();

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	FORCEINLINE EDeathPose GetDeathPose() const { return DeathPose; }

private:
	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UParticleSystem> HitParticles;

protected:
	UPROPERTY(VisibleInstanceOnly, Category = Weapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attributes;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(EditAnywhere, Category = Combat)
	double WarpTargetDistance = 75.f;

	//Animation / Sound / VisualEffect//
	UPROPERTY(EditDefaultsOnly, Category = Combat)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	TObjectPtr<UAnimMontage> HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	TObjectPtr<UAnimMontage> DodgeMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> DeathMontageSections;

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose;
};
