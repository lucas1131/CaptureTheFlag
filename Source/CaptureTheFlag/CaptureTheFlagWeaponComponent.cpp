// Copyright Epic Games, Inc. All Rights Reserved.


#include "CaptureTheFlagWeaponComponent.h"
#include "CaptureTheFlagCharacter.h"
#include "CaptureTheFlagProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Components/SphereComponent.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UCaptureTheFlagWeaponComponent::UCaptureTheFlagWeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}

void UCaptureTheFlagWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	HitEffectClass = HitEffectClassPtr.LoadSynchronous();
}

void UCaptureTheFlagWeaponComponent::FireVisuals() const
{
	if(!IsValid(Character)) return;
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UCaptureTheFlagWeaponComponent::Fire() const
{
	if(ProjectileClass == nullptr || Character == nullptr || Character->GetController() == nullptr) return;

	UWorld* const World = GetWorld();
	if (World != nullptr)
	{
		const APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ActorSpawnParams.CustomPreSpawnInitalization = [this](AActor* NewActor)
		{
			const ACaptureTheFlagProjectile* NewProjectile = Cast<ACaptureTheFlagProjectile>(NewActor);
			NewProjectile->GetCollisionComp()->IgnoreActorWhenMoving(Character, true);
			NewProjectile->GetCollisionComp()->IgnoreActorWhenMoving(GetOwner(), true);
			NewProjectile->GetCollisionComp()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		};
	
		// Spawn the projectile at the muzzle
		ACaptureTheFlagProjectile* Projectile = World->SpawnActor<ACaptureTheFlagProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		if (Projectile)
		{
			Projectile->SetInstigator(Character);
			if (IsValid(HitEffectClass))
			{
				Projectile->SetHitEffect(HitEffectClass);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Weapon trying to set null HitEffect to projectile"));
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Weapon trying to set null HitEffect to projectile"));
			}
		}
	}
}

bool UCaptureTheFlagWeaponComponent::AttachWeapon(ACaptureTheFlagCharacter* TargetCharacter, const bool bIsLocalPlayer)
{
	Character = TargetCharacter;

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UCaptureTheFlagWeaponComponent>())
	{
		return false;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

	USkeletalMeshComponent* Mesh = bIsLocalPlayer ? Character->GetMesh1P() : Character->GetMesh();
	AttachToComponent(Mesh, AttachmentRules, FName(TEXT("GripPoint")));

	return true;
}
