// Copyright 2025 ShootingStar. All Rights Reserved.


#include "LobbyPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "LobbyNetworkComponent.h"
#include "TeamComponent.h"

ALobbyPlayerController::ALobbyPlayerController()
	: LobbyUI{nullptr}
{
	static ConstructorHelpers::FClassFinder<UUserWidget> LobbyUIBPFinder{TEXT("/Game/Blueprints/UI/BP_LobbyUI")};
	ensure(LobbyUIBPFinder.Succeeded());
	if (LobbyUIBPFinder.Succeeded())
	{
		LobbyUIClass = LobbyUIBPFinder.Class;
	}

	TeamComponent = CreateDefaultSubobject<UTeamComponent>(TEXT("TeamComponent"));
	LobbyNetworkComponent = CreateDefaultSubobject<ULobbyNetworkComponent>(TEXT("LobbyNetworkComponent"));
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// 한 컴퓨터에는 복제된 다른 플레이어 컨트롤러들도 많으므로,
	// 오직 내 컴퓨터에서 지금 조종중인 컨트롤러에서 UI를 생성하도록 반드시 검증해야 합니다.
	if (IsLocalPlayerController())
	{
		bShowMouseCursor = true;
		SetInputMode(FInputModeUIOnly());
		
		ensure(LobbyUIClass);
		if (LobbyUIClass)
		{
			LobbyUI = CreateWidget<UUserWidget>(GetWorld(), LobbyUIClass);
			ensure(LobbyUI);
			if (LobbyUI)
			{
				LobbyUI->AddToViewport();
			}
		}
	}
}
