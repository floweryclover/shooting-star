// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

class UJoinOrHostGameComponent;
/**
 * 
 */
UCLASS()
class AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	UJoinOrHostGameComponent* JoinOrHostGameComponent;
	
	AMainMenuPlayerController();
	
	UFUNCTION()
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TSubclassOf<UUserWidget> MainMenuUIClass;
	
	UPROPERTY()
	UUserWidget* MainMenuUI;
};
