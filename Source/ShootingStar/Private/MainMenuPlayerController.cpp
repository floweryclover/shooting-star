// Copyright 2025 ShootingStar. All Rights Reserved.


#include "MainMenuPlayerController.h"
#include "JoinOrHostGameComponent.h"
#include "Blueprint/UserWidget.h"

AMainMenuPlayerController::AMainMenuPlayerController()
	: MainMenuUI{nullptr},
	  JoinOrHostGameComponent{nullptr}
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuUIBPClassFinder{
		TEXT("/Game/Blueprints/UI/BP_MainMenuUI")
	};
	ensure(MainMenuUIBPClassFinder.Succeeded());
	if (MainMenuUIBPClassFinder.Succeeded())
	{
		MainMenuUIClass = MainMenuUIBPClassFinder.Class;
	}

	JoinOrHostGameComponent = CreateDefaultSubobject<UJoinOrHostGameComponent>(TEXT("JoinOrHostGameComponent"));
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
}
