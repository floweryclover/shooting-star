// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientComponent.h"

#include "CompetitivePlayerController.h"
#include "InventoryComponent.h"

void UClientComponent::GainResource_Implementation(UResourceDataAsset* const Resource)
{
	ACompetitivePlayerController* const PlayerController = Cast<ACompetitivePlayerController>(GetOwner());
	if (!PlayerController)
	{
		return;
	}
	PlayerController->GetInventoryComponent()->AddResource(Resource);
}

void UClientComponent::NotifySupplyDropped_Implementation(FVector Location)
{
	ACompetitivePlayerController* const PlayerController = Cast<ACompetitivePlayerController>(GetOwner());
	if (!PlayerController)
	{
		return;
	}
	PlayerController->RenderSupplyIndicator(Location);
}
