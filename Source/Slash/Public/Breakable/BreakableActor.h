// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

UCLASS()
class SLASH_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UGeometryCollectionComponent> GeometoryCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<class UCapsuleComponent> Capsule;

private:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class ATreasure>> TreasureClasses;

	UPROPERTY(EditAnywhere)
	bool bBroken = false;
};
