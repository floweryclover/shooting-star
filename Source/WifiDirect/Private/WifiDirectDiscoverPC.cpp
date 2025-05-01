// Copyright 2025 ShootingStar. All Rights Reserved.


#include "WifiDirectDiscoverPC.h"
#include "WifiDirectInterface.h"
#include "Blueprint/UserWidget.h"

AWifiDirectDiscoverPC::AWifiDirectDiscoverPC()
	: WifiDirectDiscoverUI{nullptr},
	  bOpenLevelRequested{false}
{
	static ConstructorHelpers::FClassFinder<UUserWidget> WifiDirectDiscoverUIBPFinder{
		TEXT("/Game/Levels/WifiDirectDiscover/BP_WifiDirectDiscoverUI")
	};
	ensure(WifiDirectDiscoverUIBPFinder.Succeeded());
	if (WifiDirectDiscoverUIBPFinder.Succeeded())
	{
		WifiDirectDiscoverUIClass = WifiDirectDiscoverUIBPFinder.Class;
	}
}

void AWifiDirectDiscoverPC::BeginPlay()
{
	Super::BeginPlay();

	UWifiDirectInterface::GetWifiDirectInterface()->RegisterService();

	check(IsValid(WifiDirectDiscoverUIClass));
	WifiDirectDiscoverUI = CreateWidget<UUserWidget>(
		GetWorld(), WifiDirectDiscoverUIClass);
	check(IsValid(WifiDirectDiscoverUI));
	WifiDirectDiscoverUI->AddToViewport();

	bShowMouseCursor = true;
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(WifiDirectDiscoverUI->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void AWifiDirectDiscoverPC::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UWifiDirectInterface* const Interface = UWifiDirectInterface::GetWifiDirectInterface();
	Interface->Update(DeltaSeconds);

	if (Interface->IsP2pGroupFormed())
	{
		if (bOpenLevelRequested)
		{
			return;
		}
		bOpenLevelRequested = true;

		if (Interface->IsP2pGroupOwner())
		{
			const FName LobbyLevelName = TEXT("/Game/Levels/Lobby");
			const FString ServerOptions = LobbyLevelName.ToString() + "?listen";

			GetWorld()->ServerTravel(ServerOptions);
		}
		else
		{
			ClientTravel(Interface->GetGroupOwnerIpAddress(), TRAVEL_Absolute);
		}
	}
}
