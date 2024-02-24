// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Hovering UMETA(DisplayName = "Hovering"),
	EIS_Equipped UMETA(DisplayName = "Equipped")
};

//èúéZÇ™â¬î\Ç»ílÇÃÇ›ÇéÛÇØïtÇØÇÈ
template<typename T>
concept AvgType = requires(T a) {{ a / 2 } -> std::same_as<T>;};

UCLASS()
class AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();
	
	template<AvgType T>
	T Avg(T First, T Second);

	UFUNCTION()
	virtual void OnSphereOvarlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOvarlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	virtual void SpawnPickupSystem();
	virtual void SpawnPickupSound();
public:	
	virtual void Tick(float DeltaTime) override;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float Amplitube = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float TimeConstant = 5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USphereComponent> Sphere;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraComponent> ItemEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundBase> PickupSound;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraSystem> PickupEffect;
};

template<AvgType T>
inline T AItem::Avg(T First, T Second)
{
	return T((First + Second) / 2);
}
