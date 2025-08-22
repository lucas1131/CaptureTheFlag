// Fill out your copyright notice in the Description page of Project Settings.
#include "CaptureTheFlagFlagActor.h"

#include "CaptureTheFlagCharacter.h"
#include "CaptureTheFlagPlayerState.h"
#include "Components/SphereComponent.h"

ACaptureTheFlagFlagActor::ACaptureTheFlagFlagActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollision"));
	Collision->InitSphereRadius(60.0f);
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

	FlagColors.Add(EPlayerTeam::Spectator, FColor(255, 255, 255));
	FlagColors.Add(EPlayerTeam::Red, FColor(255, 20.4, 12.75));
	FlagColors.Add(EPlayerTeam::Blue, FColor(20.4, 50, 255));

	SetFlagColor(EPlayerTeam::Spectator);
}

void ACaptureTheFlagFlagActor::BeginPlay()
{
	Super::BeginPlay();

	FlagMaterialInstance = Flag->CreateAndSetMaterialInstanceDynamicFromMaterial(0, FlagMaterial);

	StartingLocation = GetActorLocation();
	SetFlagColor(EPlayerTeam::Spectator);
}

void ACaptureTheFlagFlagActor::SetFlagColor(const EPlayerTeam Team)
{
	if (FlagMaterialInstance)
	{
		FlagMaterialInstance->SetVectorParameterValue(FName("Color"), FlagColors[Team]);
	}
}

void ACaptureTheFlagFlagActor::ResetFlag()
{
	SetActorLocation(StartingLocation);
	SetFlagColor(EPlayerTeam::Spectator);
}

void ACaptureTheFlagFlagActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                         int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(OtherActor))
	{
		const EPlayerTeam Team = Character->GetPlayerState<ACaptureTheFlagPlayerState>()->GetTeam();
		Character->GrabFlag(this);
		SetFlagColor(Team);
	}
}
