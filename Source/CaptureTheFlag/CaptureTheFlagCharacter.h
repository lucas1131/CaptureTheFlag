// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "CaptureTheFlagFlagActor.h"
#include "CaptureTheFlagWeaponComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Logging/LogMacros.h"
#include "CaptureTheFlagCharacter.generated.h"

class UWidgetComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(MinimalAPI, config=Game)
class ACaptureTheFlagCharacter : public ACharacter
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
	UPROPERTY()
	ACaptureTheFlagFlagActor* GrabbedFlag;
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMesh1PMat;
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMesh3PMat;
	UPROPERTY(ReplicatedUsing=OnRep_SetMaterialTint)
	FLinearColor PlayerTint;
	
	UPROPERTY(EditAnywhere, Category=Player, meta=(AllowPrivateAccess=true))
	UWidgetComponent* PlayerNameWidget;

	UPROPERTY()
	UCaptureTheFlagWeaponComponent* WeaponComponent;
	UPROPERTY()
	UAbilitySystemComponent* AbilitySystem;

	UPROPERTY(EditDefaultsOnly, Category=Ability)
	TSubclassOf<UGameplayAbility> FireWeaponAbility;

public:
	ACaptureTheFlagCharacter();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void GrabFlag(ACaptureTheFlagFlagActor* PickingFlag);
	UFUNCTION(BlueprintCallable)
	void DropFlag();
	UFUNCTION(BlueprintCallable)
	void ReleaseFlag();
	UFUNCTION(BlueprintCallable)
	bool IsHoldingFlag() const { return GrabbedFlag != nullptr; }
	ACaptureTheFlagFlagActor* GetHeldFlag() const { return GrabbedFlag; }

	bool HasWeaponEquipped() const { return IsValid(WeaponComponent); }

	UFUNCTION()
	void SetPlayerTint(const FLinearColor Color)
	{
		PlayerTint = Color;
		ApplyPlayerTint();
	}

	void SetPlayerName(const FString& InName) const;

	/* Abilities */
public:
	void GrantPlayerAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);
	void GrantPlayerAbility(const TSubclassOf<UGameplayAbility>& Ability);
	CAPTURETHEFLAG_API void FireWeapon() const { WeaponComponent->Fire(); };

private:
	void GrantPlayerAbilityNotChecked(const TSubclassOf<UGameplayAbility>& Ability) const;

	UFUNCTION(Server, Reliable)
	void ServerGrantPlayerAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);
	void ServerGrantPlayerAbilities_Implementation(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);

	UFUNCTION(Server, Reliable)
	void ServerGrantPlayerAbility(TSubclassOf<UGameplayAbility> Ability);
	void ServerGrantPlayerAbility_Implementation(TSubclassOf<UGameplayAbility> Ability);

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

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

private:
	void ApplyPlayerTint();
	
	UFUNCTION()
	void OnRep_SetMaterialTint();
};

