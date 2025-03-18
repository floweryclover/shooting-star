// Copyright 2025 ShootingStar. All Rights Reserved.


#include "MainMenuPlayerController.h"
#include "JoinOrHostGameComponent.h"
#include "Blueprint/UserWidget.h"

AMainMenuPlayerController::AMainMenuPlayerController()
	: MainMenuUI{nullptr}
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuUIBPFinder{TEXT("/Game/Blueprints/UI/BP_MainMenuUI")};
	ensure(MainMenuUIBPFinder.Succeeded());
	if (MainMenuUIBPFinder.Succeeded())
	{
		MainMenuUIClass = MainMenuUIBPFinder.Class;
	}

	CreateDefaultSubobject<UJoinOrHostGameComponent>(TEXT("JoinOrHostGameComponent"));
}

void AMainMenuPlayerController::BeginPlay()
{
	ensure(MainMenuUIClass);
	if (MainMenuUIClass)
	{
		MainMenuUI = CreateWidget<UUserWidget>(GetWorld(), MainMenuUIClass);
		ensure(MainMenuUI);
		if (MainMenuUI)
		{
			MainMenuUI->AddToViewport();
		}
	}

	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
}
