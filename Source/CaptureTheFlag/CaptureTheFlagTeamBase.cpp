#include "CaptureTheFlagTeamBase.h"

#include "CaptureTheFlagCharacter.h"
#include "CaptureTheFlagGameMode.h"
#include "CaptureTheFlagGameState.h"

ACaptureTheFlagTeamBase::ACaptureTheFlagTeamBase(): Team(EPlayerTeam::Red)
{
	Collision = CreateDefaultSubobject<USphereComponent>(FName("Collision"));
	Collision->SetupAttachment(Mesh);
	Collision->SetCollisionProfileName(FName("Trigger"));
	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACaptureTheFlagTeamBase::OnOverlap);
	RootComponent = Collision;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("PPM Mesh"));
	Mesh->CastShadow = false;
	Mesh->SetRenderInMainPass(false);
	Mesh->SetRenderInDepthPass(false);
	Mesh->SetRenderCustomDepth(true);
	Mesh->SetCollisionProfileName(FName("NoCollision"));
	Mesh->SetCustomDepthStencilValue(Team == EPlayerTeam::Red ? 0 : 1);
	Mesh->SetupAttachment(Collision);
}

void ACaptureTheFlagTeamBase::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(OtherActor)) return;

	if (ACaptureTheFlagCharacter* Character = Cast<ACaptureTheFlagCharacter>(OtherActor))
	{
		const EPlayerTeam ActorTeam = Character->GetPlayerState<ACaptureTheFlagPlayerState>()->GetTeam();
		if (Character->IsHoldingFlag() && ActorTeam == Team)
		{
			if (ACaptureTheFlagGameMode* GameMode = GetWorld()->GetAuthGameMode<ACaptureTheFlagGameMode>())
			{
				GameMode->IncrementScoreForTeam(ActorTeam);
			}
			
			ACaptureTheFlagFlagActor* Flag = Character->GetHeldFlag();
			if (IsValid(Flag))
			{
				Character->ReleaseFlag();
				Flag->ResetFlag();
			}
		}
	}
}

void ACaptureTheFlagTeamBase::PostEditChangeProperty(struct FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	if (Event.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ACaptureTheFlagTeamBase, Team))
	{
		Mesh->SetCustomDepthStencilValue(Team == EPlayerTeam::Red ? 0 : 1);
	}
}
