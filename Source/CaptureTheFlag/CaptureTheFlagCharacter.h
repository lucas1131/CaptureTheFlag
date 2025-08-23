// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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

UCLASS(config=Game)
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
	class UInputAction* LookAction;

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

	UFUNCTION()
	void SetPlayerTint(const FLinearColor Color)
	{
		PlayerTint = Color;
		ApplyPlayerTint();
	}

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

