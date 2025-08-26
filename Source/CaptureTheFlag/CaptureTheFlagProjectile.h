// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "CaptureTheFlagProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class ACaptureTheFlagProjectile : public AActor
{
	GENERATED_BODY()

private:
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	int Bounces;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Damage, meta = (AllowPrivateAccess = "true"))
	int Damage;
	
	UPROPERTY(Replicated)
	TSubclassOf<UGameplayEffect> HitEffect;

public:
	ACaptureTheFlagProjectile();
	void SetHitEffect(const TSubclassOf<UGameplayEffect>& InHitEffect) { HitEffect = InHitEffect; }
	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

private:
	bool OnHitPlayer(AActor* OtherActor);
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};

