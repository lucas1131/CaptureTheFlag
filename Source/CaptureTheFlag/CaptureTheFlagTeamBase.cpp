#include "CaptureTheFlagTeamBase.h"

ACaptureTheFlagTeamBase::ACaptureTheFlagTeamBase(): Team(EPlayerTeam::Red) 
{
	Collision = CreateDefaultSubobject<USphereComponent>(FName("Collision"));
	Collision->SetupAttachment(Mesh);
	Collision->SetCollisionProfileName(FName("Trigger"));
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

void ACaptureTheFlagTeamBase::PostEditChangeProperty(struct FPropertyChangedEvent& Event)
{
	Super::PostEditChangeProperty(Event);

	if (Event.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ACaptureTheFlagTeamBase, Team))
	{
		Mesh->SetCustomDepthStencilValue(Team == EPlayerTeam::Red ? 0 : 1);
	}
}
