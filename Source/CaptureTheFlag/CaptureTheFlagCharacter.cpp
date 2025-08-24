// Copyright Epic Games, Inc. All Rights Reserved.

#include "CaptureTheFlagCharacter.h"

#include "BillboardWidgetComponent.h"
#include "CaptureTheFlagPlayerState.h"
#include "CaptureTheFlagWeaponComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PlayerNameWidget.h"
#include "Components/WidgetComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ACaptureTheFlagCharacter

ACaptureTheFlagCharacter::ACaptureTheFlagCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	FlagArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FlagArm"));
	FlagArm->SetupAttachment(GetCapsuleComponent());
	FlagArm->TargetArmLength = 200.0f;
	FlagArm->bDoCollisionTest = false;
	FlagArm->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	PlayerTint = FLinearColor::White;
	// Setup a billboard player name widget
	PlayerNameWidget = CreateDefaultSubobject<UBillboardWidgetComponent>(TEXT("PlayerNameWidget"));
	PlayerNameWidget->SetupAttachment(GetCapsuleComponent());
	PlayerNameWidget->SetTintColorAndOpacity(PlayerTint);
	PlayerNameWidget->SetWidgetClass(UPlayerNameWidget::StaticClass());
}

void ACaptureTheFlagCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (RifleClass)
	{
		FActorSpawnParameters Parameters;
		Parameters.Owner = this;
		Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		const AActor* Rifle = GetWorld()->SpawnActor<AActor>(RifleClass, FTransform::Identity, Parameters);
		WeaponComponent = Rifle->GetComponentByClass<UCaptureTheFlagWeaponComponent>();
		if (WeaponComponent)
		{
			WeaponComponent->AttachWeapon(this, IsLocallyControlled());
		}
	}

	const ACaptureTheFlagPlayerState* State = GetPlayerState<ACaptureTheFlagPlayerState>();
	if (State) SetPlayerName(State->GetPlayerName());
}

void ACaptureTheFlagCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACaptureTheFlagCharacter, PlayerTint);
}

void ACaptureTheFlagCharacter::GrabFlag(ACaptureTheFlagFlagActor* PickingFlag)
{
	const FAttachmentTransformRules SnapLocationOnly(EAttachmentRule::SnapToTarget,
	                                                 EAttachmentRule::KeepWorld,
	                                                 EAttachmentRule::KeepWorld,
	                                                 false);
	PickingFlag->AttachToComponent(FlagArm, SnapLocationOnly);
	GrabbedFlag = PickingFlag;
}

void ACaptureTheFlagCharacter::DropFlag()
{
	if (IsValid(GrabbedFlag))
	{
		GrabbedFlag->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		GrabbedFlag->SetActorLocation(GetActorLocation());
		GrabbedFlag->OnDropped();
		GrabbedFlag = nullptr;
	}
}

void ACaptureTheFlagCharacter::ReleaseFlag()
{
	if (IsValid(GrabbedFlag))
	{
		GrabbedFlag->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		GrabbedFlag->OnDropped();
		GrabbedFlag = nullptr;
	}
}

void ACaptureTheFlagCharacter::SetPlayerName(const FString& InName) const
{
	if (PlayerNameWidget)
	{
		const UPlayerNameWidget* Widget = Cast<UPlayerNameWidget>(PlayerNameWidget->GetWidget());
		if (!Widget)
		{
			PlayerNameWidget->InitWidget();
			Widget = Cast<UPlayerNameWidget>(PlayerNameWidget->GetWidget());
		}
		
		Widget->SetPlayerName(InName);
	}
}

void ACaptureTheFlagCharacter::ServerFire_Implementation()
{
	if (IsValid(WeaponComponent))
	{
		WeaponComponent->RequestFire();
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void ACaptureTheFlagCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
			PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ACaptureTheFlagCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACaptureTheFlagCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACaptureTheFlagCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}

void ACaptureTheFlagCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ACaptureTheFlagCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACaptureTheFlagCharacter::ApplyPlayerTint()
{
	if (!IsValid(DynamicMesh1PMat)) DynamicMesh1PMat = Mesh1P->CreateAndSetMaterialInstanceDynamic(0);
	if (!IsValid(DynamicMesh3PMat)) DynamicMesh3PMat = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);

	DynamicMesh1PMat->SetVectorParameterValue(FName("Tint"), PlayerTint);
	DynamicMesh3PMat->SetVectorParameterValue(FName("Tint"), PlayerTint);

	if (IsValid(PlayerNameWidget)) PlayerNameWidget->SetTintColorAndOpacity(PlayerTint);
}

void ACaptureTheFlagCharacter::OnRep_SetMaterialTint()
{
	ApplyPlayerTint();
}
