// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class UBoxComponent;

/**
 * 
 */
UCLASS()
class SLASH_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();	

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOvarlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	bool ActorIsSameType(AActor* OtherActor);

	void ExecuteGetHit(FHitResult& BoxHit);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }

	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

private:
	void BoxTrace(FHitResult& BoxHit);

	void PlayEquipSound();
	void DisableSphereCollision();
	void DeactivateEmbers();

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<class USoundBase> EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<UBoxComponent> WeaponBox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<class USceneComponent> BoxTraceStart;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<class USceneComponent> BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;

public:
	TArray<AActor*> IgnoreActors;
};
