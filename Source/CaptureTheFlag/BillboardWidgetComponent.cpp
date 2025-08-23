// Fill out your copyright notice in the Description page of Project Settings.

#include "BillboardWidgetComponent.h"

void UBillboardWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (LocalPlayer)
	{
		LocalPlayerController = LocalPlayer->PlayerController;
	}

	PrimaryComponentTick.bCanEverTick = IsValid(LocalPlayerController);
}

void UBillboardWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(LocalPlayerController)) return;
	
	if (const APawn* PlayerPawn = LocalPlayerController->GetPawn())
	{
		const FVector PlayerLocation = PlayerPawn->GetActorLocation();
		FRotator LookAtRotation = (PlayerLocation - GetComponentLocation()).Rotation();
	
		// Optional: Only yaw billboard, not pitch (so text stays upright)
		LookAtRotation.Pitch = 0.f;
		LookAtRotation.Roll = 0.f;
	
		SetWorldRotation(LookAtRotation);
	}
}
