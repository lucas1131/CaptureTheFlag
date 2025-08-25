// Copyright Epic Games, Inc. All Rights Reserved.

#include "CaptureTheFlagProjectile.h"

#include "CaptureTheFlagCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

ACaptureTheFlagProjectile::ACaptureTheFlagProjectile()
{
	SetReplicates(true);
	AActor::SetReplicateMovement(true);

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ACaptureTheFlagProjectile::OnHit);
	// set up a notification for when this component hits something blocking
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ACaptureTheFlagProjectile::OnBeginOverlap);

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	Bounces = 3;
	Damage = 1;
}

bool ACaptureTheFlagProjectile::OnHitPlayer(AActor* OtherActor)
{
	// If we hit player
	APawn* InstigatorActor = GetInstigator();
	const IAbilitySystemInterface* HitActorAbilityActor = Cast<IAbilitySystemInterface>(OtherActor);
	const IAbilitySystemInterface* InstigatorAbilityActor = Cast<IAbilitySystemInterface>(InstigatorActor);
	if (HitActorAbilityActor && InstigatorAbilityActor)
	{
		UAbilitySystemComponent* HitASC = HitActorAbilityActor->GetAbilitySystemComponent();
		UAbilitySystemComponent* InstigatorASC = InstigatorAbilityActor->GetAbilitySystemComponent();
		if (HitASC && InstigatorASC)
		{
			FGameplayEffectContextHandle Context = InstigatorASC->MakeEffectContext();
			Context.AddInstigator(InstigatorActor, InstigatorActor->GetController());
			InstigatorASC->BP_ApplyGameplayEffectToTarget(HitEffect, HitASC, 1, Context);
			Destroy();
			return true;
		}
	}

	return false;
}

void ACaptureTheFlagProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                      FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsValid(OtherActor) && OtherActor == GetInstigator())
	{
		return;
	}

	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		Destroy();
		return;
	}

	if (OnHitPlayer(OtherActor))
	{
		return;
	}

	// Hitting anything else, just bounce
	if (Bounces-- <= 0)
	{
		Destroy();
	}
}

void ACaptureTheFlagProjectile::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                               int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && OtherActor == GetInstigator())
	{
		return;
	}

	OnHitPlayer(OtherActor);
}
