// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void RegenStamina(float DeltaTime);
	void ReceiveDamage(float Damage);
	void UseStamina(float StaminaCost);
	float GetHealthPercent() const;
	float GetStaminaPercent() const;
	bool IsAlive() const;
	void AddGold(int32 AmountOfGold);
	void AddSouls(int32 NumberOfSouls);

	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }

private:

	//åªç›ÇÃëÃóÕ
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;
	//ç≈ëÂëÃóÕíl
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	//åªç›ÇÃéùãvóÕ
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;
	//ç≈ëÂéùãvóÕíl
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 14.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 8.f;
};
