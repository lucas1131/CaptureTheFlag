// Fill out your copyright notice in the Description page of Project Settings.
#include "CaptureTheFlagFlagActor.h"

#include "CaptureTheFlagCharacter.h"
#include "CaptureTheFlagGameMode.h"
#include "CaptureTheFlagPlayerState.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

ACaptureTheFlagFlagActor::ACaptureTheFlagFlagActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	Collision->InitSphereRadius(60.0f);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->BodyInstance.SetCollisionProfileName("OverlapOnlyPawn");
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACaptureTheFlagFlagActor::OnOverlap);
	RootComponent = Collision;

	Pole = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pole"));
	Pole->SetupAttachment(Collision);
	Pole->SetCollisionProfileName(FName("NoCollision"));
	Pole->CastShadow = false;

	Flag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Flag"));
	Flag->SetupAttachment(Pole);
	Flag->SetCollisionProfileName(FName("NoCollision"));
	Flag->CastShadow = false;

	SetFlagColor(EPlayerTeam::Spectator);
}

void ACaptureTheFlagFlagActor::BeginPlay()
{
	Super::BeginPlay();

	FlagMaterialInstance = Flag->CreateAndSetMaterialInstanceDynamicFromMaterial(0, FlagMaterial);

	StartingLocation = GetActorLocation();
	SetFlagColor(EPlayerTeam::Spectator);
}

void ACaptureTheFlagFlagActor::SetFlagMaterialColor() const
{
	if (FlagMaterialInstance)
	{
		FlagMaterialInstance->SetVectorParameterValue(FName("Color"), CurrentColor);
	}
}

void ACaptureTheFlagFlagActor::SetFlagColor(const EPlayerTeam Team)
{
	const UWorld* World = GetWorld();
	if (!World) return;
	if (ACaptureTheFlagGameMode* GameMode = World->GetAuthGameMode<ACaptureTheFlagGameMode>())
	{
		CurrentColor = GameMode->GetTeamColor(Team);
		SetFlagMaterialColor();
	}
}

void ACaptureTheFlagFlagActor::ResetFlag()
{
	SetActorLocation(StartingLocation);
	SetFlagColor(EPlayerTeam::Spectator);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ACaptureTheFlagFlagActor::OnDropped()
{
	SetFlagColor(EPlayerTeam::Spectator);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ACaptureTheFlagFlagActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACaptureTheFlagFlagActor, CurrentColor);
}

void ACaptureTheFlagFlagActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                         int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(OtherActor))
	{
		const ACaptureTheFlagPlayerState* PlayerState = Character->GetPlayerState<ACaptureTheFlagPlayerState>();
		if (IsValid(PlayerState))
		{
			const EPlayerTeam Team = PlayerState->GetTeam();
			Character->GrabFlag(this);
			Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SetFlagColor(Team);
		}
	}
}

void ACaptureTheFlagFlagActor::OnRep_Color() const
{
	SetFlagMaterialColor();
}
