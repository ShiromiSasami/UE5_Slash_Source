// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

UCLASS()
class SLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

private:
	void CheckCombatTarget();
	void CheckPatrolTarget();
	void PatrolTimerFinished();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void StartAttackTimer();
	void ClearPatrolTimer();
	void ClearAttackTimer();
	void SpawnDefaultWeapon();
	void InitializeEnemy();
	void SpawnSoul();

protected:
	virtual void Die_Implementation() override;
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void AttackEnd() override;

	bool InTargetRange(AActor* Target, double Radius);
	void MoverToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

private:
#pragma region Components

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UHealthBarComponent> HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UPawnSensingComponent> PawnSensing;
#pragma endregion

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = Combat)
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	double AttackRadius = 150.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	double AcceptanceRadius = 50.f;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 1.0f;

#pragma region Navigation
	UPROPERTY()
	TObjectPtr<class AAIController> EnemyController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation", meta = (ToolTip = "Current Patrol Target"))
	TObjectPtr<AActor> PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation", meta = (ToolTip = "Patrol Targets"))
	TArray<TObjectPtr<AActor>> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrollingSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ChasingSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan = 3.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class ASoul> SoulClass;

	FTimerHandle PatrolTimer;
#pragma endregion

protected:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
};
