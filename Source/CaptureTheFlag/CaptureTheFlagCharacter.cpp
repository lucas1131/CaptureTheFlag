// Copyright Epic Games, Inc. All Rights Reserved.

#include "CaptureTheFlagCharacter.h"

#include "AbilitySystemComponent.h"
#include "BillboardWidgetComponent.h"
#include "CaptureTheFlagPlayerController.h"
#include "HealthAttributeSet.h"
#include "CaptureTheFlagPlayerState.h"
#include "CaptureTheFlagWeaponComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HUDWidget.h"
#include "InputActionValue.h"
#include "PlayerNameWidget.h"
#include "Components/WidgetComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
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

	if (USkeletalMeshComponent* Mesh3P = GetMesh())
	{
		Mesh3P->SetIsReplicated(true);
	}

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

	// Abilities and attributes
	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);
	HealthAttribute = CreateDefaultSubobject<UHealthAttributeSet>(TEXT("CharacterAttributes"));
	MovementAttribute = CreateDefaultSubobject<UMovementAttribute>(TEXT("MovementAttributes"));
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

	const ACaptureTheFlagPlayerState* CTFPlayerState = GetPlayerState<ACaptureTheFlagPlayerState>();
	if (CTFPlayerState)
	{
		SetPlayerName(CTFPlayerState->GetPlayerName());
	}
	
	if (IsValid(AbilitySystem))
	{
		InitializeCharacterAttributes();

		// I don't like these Sync Loads here, at this point its very possible player is already in world and seeing something, this could cause visible stuttering
		FireWeaponAbility = FGameplayAbilitySpec(FireWeaponAbilityPtr.LoadSynchronous());
		GrantPlayerAbilities({
			FireWeaponAbility,
			FGameplayAbilitySpec(DeathAbilityPtr.LoadSynchronous()),
			FGameplayAbilitySpec(RespawnAbilityPtr.LoadSynchronous()),
		});

		AbilitySystem->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag("Character.State.HoldingFlag"), EGameplayTagEventType::NewOrRemoved)
			.AddUObject(this, &ACaptureTheFlagCharacter::OnHoldingFlagStateChanged);
	}

	const FVector DeathCameraOffset = FirstPersonCameraComponent->GetForwardVector() * -50.0f + FirstPersonCameraComponent->GetUpVector() * 15.0f;
	DeathCameraLocation = FirstPersonCameraComponent->GetRelativeLocation() + DeathCameraOffset;
	CameraDefaultLocation = FirstPersonCameraComponent->GetRelativeLocation();
}

void ACaptureTheFlagCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AbilitySystem->InitAbilityActorInfo(NewController, this);
}

void ACaptureTheFlagCharacter::SetupTeamTag(const EPlayerTeam Team) const
{
	if (!HasAuthority()) return;

	const FGameplayTag BlueTeamTag = FGameplayTag::RequestGameplayTag("Character.Team.Blue");
	const FGameplayTag RedTeamTag = FGameplayTag::RequestGameplayTag("Character.Team.Red");
	const FGameplayTag SpectatorTeamTag = FGameplayTag::RequestGameplayTag("Character.Team.Spectator");

	const FGameplayEffectSpecHandle Handle = AbilitySystem->MakeOutgoingSpec(TeamTagEffect, 1.0f, AbilitySystem->MakeEffectContext());
	FGameplayEffectSpec* Effect = Handle.Data.Get();

	switch (Team)
	{
	case EPlayerTeam::Blue:
		Effect->DynamicGrantedTags.AddTag(BlueTeamTag);
		break;
	case EPlayerTeam::Red:
		Effect->DynamicGrantedTags.AddTag(RedTeamTag);
		break;
	case EPlayerTeam::Spectator:
	default:
		Effect->DynamicGrantedTags.AddTag(SpectatorTeamTag);
		break;
	}

	AbilitySystem->ApplyGameplayEffectSpecToSelf(*Effect);
}

void ACaptureTheFlagCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACaptureTheFlagCharacter, PlayerTint);
	DOREPLIFETIME(ACaptureTheFlagCharacter, bIsRagdoll);
	DOREPLIFETIME(ACaptureTheFlagCharacter, GrabbedFlag);
}

void ACaptureTheFlagCharacter::GrabFlag(ACaptureTheFlagFlagActor* PickingFlag)
{
	const FAttachmentTransformRules SnapLocationOnly(EAttachmentRule::SnapToTarget,
	                                                 EAttachmentRule::KeepWorld,
	                                                 EAttachmentRule::KeepWorld,
	                                                 false);
	PickingFlag->AttachToComponent(FlagArm, SnapLocationOnly);
	GrabbedFlag = PickingFlag;
	IsHoldingFlagEffectHandle = AbilitySystem->BP_ApplyGameplayEffectToSelf(
		HoldingFlagEffect, 1, AbilitySystem->MakeEffectContext());
}

void ACaptureTheFlagCharacter::DropFlag()
{
	if (IsValid(GrabbedFlag) && IsHoldingFlagEffectHandle.IsValid())
	{
		GrabbedFlag->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		GrabbedFlag->SetActorLocation(GetActorLocation());
		GrabbedFlag->OnDropped();
		GrabbedFlag = nullptr;
		AbilitySystem->RemoveActiveGameplayEffect(IsHoldingFlagEffectHandle);
	}
}

void ACaptureTheFlagCharacter::ReleaseFlag()
{
	if (IsValid(GrabbedFlag))
	{
		GrabbedFlag->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		GrabbedFlag->OnDropped();
		GrabbedFlag = nullptr;
		AbilitySystem->RemoveActiveGameplayEffect(IsHoldingFlagEffectHandle);
	}
}

void ACaptureTheFlagCharacter::OnHoldingFlagStateChanged(FGameplayTag GameplayTag, const int Count) const
{
	if (const ACaptureTheFlagPlayerController* CTFController = GetController<ACaptureTheFlagPlayerController>())
	{
		if (const UHUDWidget* HUDWidget = CTFController->GetHUDWidget())
		{
			if (Count > 0)
			{
				// New tag, started holding flag
				HUDWidget->ShowFlagIcon(true, PlayerTint);
			}
			else
			{
				// No tag, dropped or released flag
				HUDWidget->ShowFlagIcon(false);
			}
		}
	}
}

void ACaptureTheFlagCharacter::OnRep_SetIsRagdoll() const
{
	USkeletalMeshComponent* Mesh3P = GetMesh();

	// TODO a lot of testing collision here
	if (Mesh1P)
	{
		Mesh1P->SetOwnerNoSee(bIsRagdoll);
		Mesh1P->SetCollisionProfileName(bIsRagdoll ? "NoCollision" : "Pawn");
	}

	if (Mesh3P)
	{
		Mesh3P->SetOwnerNoSee(!bIsRagdoll);
		Mesh3P->SetAllBodiesSimulatePhysics(bIsRagdoll);
		Mesh3P->SetSimulatePhysics(bIsRagdoll);
		Mesh3P->SetCollisionProfileName(bIsRagdoll ? "Ragdoll" : "Pawn");
		Mesh3P->bPauseAnims = bIsRagdoll;

		FirstPersonCameraComponent->SetRelativeLocation(bIsRagdoll ? DeathCameraLocation : CameraDefaultLocation);
	}

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(bIsRagdoll ? ECollisionEnabled::Type::NoCollision : ECollisionEnabled::Type::QueryAndPhysics);
	}

	if (WeaponComponent)
	{
		WeaponComponent->SetOwnerNoSee(bIsRagdoll);
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

//////////////////////////////////////////////////////////////////////////// Abilities

void ACaptureTheFlagCharacter::InitializeCharacterAttributes() const
{
	if (HasAuthority())
	{
		ServerInitializeCharacterAttributes_Implementation();
	}
	else
	{
		ServerInitializeCharacterAttributes();
	}
}

void ACaptureTheFlagCharacter::ServerInitializeCharacterAttributes_Implementation() const
{
	AbilitySystem->BP_ApplyGameplayEffectToSelf(InitAttributesEffectClass, 1, AbilitySystem->MakeEffectContext());
}

void ACaptureTheFlagCharacter::GrantPlayerAbilities(const TArray<FGameplayAbilitySpec>& Abilities)
{
	if (IsValid(AbilitySystem))
	{
		if (HasAuthority())
		{
			for (const FGameplayAbilitySpec Ability : Abilities)
			{
				GrantPlayerAbilityNotChecked(Ability);
			}
		}
		else
		{
			ServerGrantPlayerAbilities(Abilities);
		}
	}
}

void ACaptureTheFlagCharacter::GrantPlayerAbility(const FGameplayAbilitySpec& Ability)
{
	if (IsValid(AbilitySystem))
	{
		if (HasAuthority())
		{
			GrantPlayerAbilityNotChecked(Ability);
		}
		else
		{
			ServerGrantPlayerAbility(Ability);
		}
	}
}

void ACaptureTheFlagCharacter::GrantPlayerAbilityNotChecked(const FGameplayAbilitySpec& Ability) const
{
	AbilitySystem->GiveAbility(Ability);
	
}

void ACaptureTheFlagCharacter::ServerGrantPlayerAbilities_Implementation(const TArray<FGameplayAbilitySpec>& Abilities)
{
	GrantPlayerAbilities(Abilities);
}

void ACaptureTheFlagCharacter::ServerGrantPlayerAbility_Implementation(const FGameplayAbilitySpec& Ability)
{
	GrantPlayerAbility(Ability);
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
			Subsystem->AddMappingContext(FireMappingContext, 1);
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

		// Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ACaptureTheFlagCharacter::FireWeapon);
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
		AddMovementInput(GetActorForwardVector() * MovementAttribute->GetSpeedMultiplier(), MovementVector.Y);
		AddMovementInput(GetActorRightVector() * MovementAttribute->GetSpeedMultiplier(), MovementVector.X);
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

void ACaptureTheFlagCharacter::FireWeapon(const FInputActionValue& _)
{
	AbilitySystem->TryActivateAbility(FireWeaponAbility.Handle, true);
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
