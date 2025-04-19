// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerComponent.h"
#include "CompetitiveGameMode.h"

void UServerComponent::RequestInteractResource_Implementation()
{
	ACompetitiveGameMode* GameMode = Cast<ACompetitiveGameMode>(GetWorld()->GetAuthGameMode());
	AController* const Controller = Cast<AController>(GetOwner());
	GameMode->InteractResource(Controller);
}
