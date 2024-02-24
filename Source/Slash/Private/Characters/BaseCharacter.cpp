

#include "Characters/BaseCharacter.h"

#include "Items/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"
#include "Characters//CharacterTypes.h"
#include "Slash/DebugMacros.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

ABaseCharacter::ABaseCharacter()
	: Attributes(CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes")))
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::AttackEnd()
{
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::DodgeEnd()
{

}

void ABaseCharacter::Die_Implementation()
{
	Tags.Add("Dead");
	PlayDeathMontage();
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive() && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else
	{
		Die();
	}

	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	//前方向との角度差を求める(内積)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	//左右方向を求める(外積)
	//前方と接触位置からクロスベクトルを求めて、Zが+なら右、-なら左
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	FName SectionName("FromBack");
	if (Theta > -45.f && Theta < 45.f)
	{
		SectionName = FName("FromFront");
	}
	else if (Theta > -135.f && Theta < -45.f)
	{
		SectionName = FName("FromLeft");
	}
	else if (Theta > 45.f && Theta < 135.f)
	{
		SectionName = FName("FromRight");
	}
	PlayHitReactMontage(SectionName);
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
	}
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.IsEmpty()) { return -1; }
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);
	EDeathPose Pose = static_cast<EDeathPose>(Selection);
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}

	return Selection;
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, FName("Defualt"));
}

FVector ABaseCharacter::GetTranslationWarpTarget() const
{
	if (!CombatTarget) { return FVector(); }
	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	const float diff = (Location - CombatTargetLocation).Size();
	if (diff < WarpTargetDistance) { return Location; }
	TargetToMe *= WarpTargetDistance;

	return CombatTargetLocation + TargetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget() const
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

bool ABaseCharacter::IsAlive()
{
	return  Attributes && Attributes->IsAlive();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}
}

