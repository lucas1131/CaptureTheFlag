#pragma once
#include "CaptureTheFlagPlayerState.h"
#include "Components/SphereComponent.h"

#include "CaptureTheFlagTeamBase.generated.h"

UCLASS(Blueprintable, BlueprintType)
class ACaptureTheFlagTeamBase : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Team, meta = (AllowPrivateAccess = "true"))
	EPlayerTeam Team;

	UPROPERTY(VisibleAnywhere, Category=Collision)
	USphereComponent* Collision;
	UPROPERTY(VisibleAnywhere, Category=Mesh)
	UStaticMeshComponent* Mesh;

public:
	ACaptureTheFlagTeamBase();
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& Event) override;
};
