// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "CaptureTheFlagFlagActor.h"
#include "CaptureTheFlagWeaponComponent.h"
#include "HealthAttributeSet.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Logging/LogMacros.h"
#include "CaptureTheFlagCharacter.generated.h"

class UHealthAttributeSet;
class UWidgetComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(MinimalAPI, config=Game)
class ACaptureTheFlagCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;
	
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> RifleClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* FlagArm;

private:
	UPROPERTY(Replicated)
	ACaptureTheFlagFlagActor* GrabbedFlag;
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMesh1PMat;
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMesh3PMat;
	UPROPERTY(ReplicatedUsing=OnRep_SetMaterialTint)
	FLinearColor PlayerTint;
	
	UPROPERTY(ReplicatedUsing=OnRep_SetIsRagdoll)
	bool bIsRagdoll;
	
	FVector DeathCameraLocation;
	FVector CameraDefaultLocation;
	
	UPROPERTY(EditAnywhere, Category=Player, meta=(AllowPrivateAccess=true))
	UWidgetComponent* PlayerNameWidget;

	UPROPERTY(BlueprintReadonly, meta=(AllowPrivateAccess=true))
	UCaptureTheFlagWeaponComponent* WeaponComponent;

	/* Abilities and attributes */
	UPROPERTY(EditAnywhere, BlueprintReadonly, Category=Ability, meta=(AllowPrivateAccess=true))
	UAbilitySystemComponent* AbilitySystem;
	UPROPERTY(EditAnywhere, Category="Ability|Attributes")
	UHealthAttributeSet* Attributes;

	UPROPERTY(EditDefaultsOnly, Category=Ability)
	TSoftClassPtr<UGameplayAbility> FireWeaponAbilityPtr;
	FGameplayAbilitySpec FireWeaponAbility;

	UPROPERTY(EditDefaultsOnly, Category=Ability)
	TSoftClassPtr<UGameplayAbility> DeathAbilityPtr;
	
	UPROPERTY(EditDefaultsOnly, Category=Ability)
	TSoftClassPtr<UGameplayAbility> RespawnAbilityPtr;
	
	UPROPERTY(EditAnywhere, Category="Ability|Attributes")
	TSubclassOf<UGameplayEffect> InitAttributesEffectClass;
	/* End Abilities and attributes */

public:
	ACaptureTheFlagCharacter();

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable)
	UCameraComponent* GetPlayerCamera() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintCallable)
	void GrabFlag(ACaptureTheFlagFlagActor* PickingFlag);
	UFUNCTION(BlueprintCallable)
	CAPTURETHEFLAG_API void DropFlag();
	UFUNCTION(BlueprintCallable)
	CAPTURETHEFLAG_API void ReleaseFlag();
	UFUNCTION(BlueprintCallable)
	CAPTURETHEFLAG_API bool IsHoldingFlag() const { return GrabbedFlag != nullptr; }
	ACaptureTheFlagFlagActor* GetHeldFlag() const { return GrabbedFlag; }

	CAPTURETHEFLAG_API bool HasWeaponEquipped() const { return IsValid(WeaponComponent); }
	
	CAPTURETHEFLAG_API void SetRagdoll(const bool InbIsRagdoll)
	{
		bIsRagdoll = InbIsRagdoll;
		if (HasAuthority())
		{
			OnRep_SetIsRagdoll(); // Force rep on host 
		}
	}

	float GetMaxHealth() const { return Attributes->GetMaxHealth(); }
	
private:
	UFUNCTION()
	void OnRep_SetIsRagdoll() const;

public:
	UFUNCTION()
	void SetPlayerTint(const FLinearColor Color)
	{
		PlayerTint = Color;
		ApplyPlayerTint();
	}

	void SetPlayerName(const FString& InName) const;

	/* IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystem; }
	/* End IAbilitySystemInterface */

	/* Abilities and attributes */
public:
	CAPTURETHEFLAG_API void InitializeCharacterAttributes() const;
	void GrantPlayerAbilities(const TArray<FGameplayAbilitySpec>& Abilities);
	void GrantPlayerAbility(const FGameplayAbilitySpec& Ability);
	CAPTURETHEFLAG_API UCaptureTheFlagWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }
	CAPTURETHEFLAG_API FOnHealthChanged& GetOnHealthChangedEvent() const { return Attributes->OnHealthChanged;}

private:
	UFUNCTION(Server, Reliable)
	void ServerInitializeCharacterAttributes() const;
	void ServerInitializeCharacterAttributes_Implementation() const;
	void GrantPlayerAbilityNotChecked(const FGameplayAbilitySpec& Ability) const;

	UFUNCTION(Server, Reliable)
	void ServerGrantPlayerAbilities(const TArray<FGameplayAbilitySpec>& Abilities);
	void ServerGrantPlayerAbilities_Implementation(const TArray<FGameplayAbilitySpec>& Abilities);

	UFUNCTION(Server, Reliable)
	void ServerGrantPlayerAbility(const FGameplayAbilitySpec& Ability);
	void ServerGrantPlayerAbility_Implementation(const FGameplayAbilitySpec& Ability);
	/* End Abilities and attributes */

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	void FireWeapon(const FInputActionValue& _);

protected:
	// APawn interface
	virtual void NotifyControllerChanged() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	void SetupTeamTag(EPlayerTeam Team) const;
private:
	void ApplyPlayerTint();
	
	UFUNCTION()
	void OnRep_SetMaterialTint();
};

