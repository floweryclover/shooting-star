// Copyright 2025 ShootingStar. All Rights Reserved.


#include "LobbyPlayerController.h"

#include "CompetitiveSystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "LobbyNetworkComponent.h"
#include "LobbyGameMode.h"

ALobbyPlayerController::ALobbyPlayerController()
	: LobbyUI{nullptr},
	  LobbyGameMode{nullptr}
{
	static ConstructorHelpers::FClassFinder<UUserWidget> LobbyUIBPFinder{TEXT("/Game/Blueprints/UI/BP_LobbyUI")};
	ensure(LobbyUIBPFinder.Succeeded());
	if (LobbyUIBPFinder.Succeeded())
	{
		LobbyUIClass = LobbyUIBPFinder.Class;
	}

	LobbyNetworkComponent = CreateDefaultSubobject<ULobbyNetworkComponent>(TEXT("LobbyNetworkComponent"));
	TeamComponent = CreateDefaultSubobject<UTeamComponent>(TEXT("TeamComponent"));
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		LobbyGameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
	}

	// 한 컴퓨터에는 복제된 다른 플레이어 컨트롤러들도 많으므로,
	// 오직 내 컴퓨터에서 지금 조종중인 컨트롤러에서 UI를 생성하도록 반드시 검증해야 합니다.
	if (IsLocalPlayerController())
	{
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
		bShowMouseCursor = true;
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(LobbyUI->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
	}
}

void ALobbyPlayerController::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsLocalPlayerController() && HasAuthority())
	{
		LobbyNetworkComponent->Process(LobbyGameMode->GetNumPlayers(),
		                               UCompetitiveSystemComponent::MaxPlayersPerTeam, DeltaSeconds);
	}
}
