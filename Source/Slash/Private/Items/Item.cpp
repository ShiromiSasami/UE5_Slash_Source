// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"

#include "Slash/DebugMacros.h"
#include "Interfaces/PickupInterface.h"

#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AItem::AItem()
	: ItemMesh(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent")))
	, Sphere(CreateDefaultSubobject<USphereComponent>(TEXT("Sphere")))
	, ItemEffect(CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers")))
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RootComponent = ItemMesh;
	Sphere->SetupAttachment(GetRootComponent());
	ItemEffect->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOvarlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOvarlap);
}

float AItem::TransformedSin()
{
	return Amplitube * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos()
{
	return Amplitube * FMath::Cos(RunningTime * TimeConstant);
}

void AItem::OnSphereOvarlap(
	UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	if (IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor))
	{
		PickupInterface->SetOverlappingItem(this);
	}
	
}

void AItem::OnSphereEndOvarlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	if (IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor))
	{
		PickupInterface->SetOverlappingItem(nullptr);
	}
}

void AItem::SpawnPickupSystem()
{
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation());
	}
}

void AItem::SpawnPickupSound()
{
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
}

