// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureTheFlagFlagActor.generated.h"

enum class EPlayerTeam : uint8;
class USphereComponent;

UCLASS(Blueprintable, BlueprintType)
class CAPTURETHEFLAG_API ACaptureTheFlagFlagActor : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Collision)
	USphereComponent* Collision;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Mesh)
	UStaticMeshComponent* Pole;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Mesh)
	UStaticMeshComponent* Flag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Materials)
	TMap<EPlayerTeam, FLinearColor> FlagColors;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Materials)
	UMaterialInterface* FlagMaterial;

private:
	UPROPERTY(ReplicatedUsing=OnRep_Color)
	FLinearColor CurrentColor;
	UPROPERTY()
	class UMaterialInstanceDynamic* FlagMaterialInstance;
	FVector StartingLocation;

public:
	ACaptureTheFlagFlagActor();
	virtual void BeginPlay() override;
	void SetFlagMaterialColor() const;
	void SetFlagColor(EPlayerTeam Team);
	void ResetFlag();
	void OnDropped();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRep_Color() const;
};
