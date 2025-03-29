// Copyright 2025 ShootingStar. All Rights Reserved.


#include "WifiDirectDiscoverPC.h"
#include "WifiDirectInterface.h"
#include "Blueprint/UserWidget.h"

AWifiDirectDiscoverPC::AWifiDirectDiscoverPC()
	: WifiDirectRefreshElapsed{0.0f},
	  WifiDirectDiscoverUI{nullptr},
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
	
	check(IsValid(WifiDirectDiscoverUIClass));
	WifiDirectDiscoverUI = CreateWidget<UUserWidget>(
		GetWorld(), WifiDirectDiscoverUIClass);
	check(IsValid(WifiDirectDiscoverUI));
	WifiDirectDiscoverUI->AddToViewport();

	UWifiDirectInterface* const Interface = UWifiDirectInterface::GetWifiDirectInterface();
	Interface->Refresh();
}

void AWifiDirectDiscoverPC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AWifiDirectDiscoverPC::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UWifiDirectInterface* const Interface = UWifiDirectInterface::GetWifiDirectInterface();

	WifiDirectRefreshElapsed += DeltaSeconds;
	if (WifiDirectRefreshElapsed >= WifiDirectRefreshInterval)
	{
		WifiDirectRefreshElapsed = 0.0f;
		Interface->Refresh();
	}

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
