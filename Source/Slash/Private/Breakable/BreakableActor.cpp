

#include "Breakable/BreakableActor.h"

#include "Items/Treasure.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/CapsuleComponent.h"


ABreakableActor::ABreakableActor()
	: GeometoryCollection(CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometoryCollection")))
	, Capsule(CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule")))
{
	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(GeometoryCollection);
	GeometoryCollection->SetGenerateOverlapEvents(true);
	GeometoryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometoryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (bBroken) { return; }
	bBroken = true;
	UWorld* world = GetWorld();
	if (world && !TreasureClasses.IsEmpty())
	{
		FVector Location = GetActorLocation();
		Location.Z += 75.0f;

		int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);
		world->SpawnActor<ATreasure>(TreasureClasses[Selection], Location, GetActorRotation());
	}
}

