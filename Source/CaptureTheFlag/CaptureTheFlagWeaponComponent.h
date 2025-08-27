// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "CaptureTheFlagWeaponComponent.generated.h"

class UGameplayEffect;
class ACaptureTheFlagCharacter;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CAPTURETHEFLAG_API UCaptureTheFlagWeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ACaptureTheFlagProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;
	
private:
	/** The Character holding this weapon*/
	UPROPERTY()
	ACaptureTheFlagCharacter* Character;

	UPROPERTY(EditAnywhere, Category=Gameplay)
	TSoftClassPtr<UGameplayEffect> HitEffectClassPtr;
	UPROPERTY()
	TSubclassOf<UGameplayEffect> HitEffectClass;

public:
	/** Sets default values for this component's properties */
	UCaptureTheFlagWeaponComponent();
	virtual void BeginPlay() override;
	void FireVisuals() const;

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	bool AttachWeapon(ACaptureTheFlagCharacter* TargetCharacter, const bool bIsLocalPlayer);
	
	/** Make the weapon Fire a Projectile */
	void Fire() const;
};
