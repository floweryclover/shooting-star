// Copyright 2025 ShootingStar. All Rights Reserved.


#include "WifiDirectDiscoverPC.h"
#include "Blueprint/UserWidget.h"

AWifiDirectDiscoverPC::AWifiDirectDiscoverPC()
	: PeerDiscoveringElapsed{0.0f},
	  WifiDirectDiscoverUI{nullptr}
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
}

void AWifiDirectDiscoverPC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Super::EndPlay(EndPlayReason);
	//
	// UShootingStarGameInstance* const GameInstance = Cast<UShootingStarGameInstance>(GetGameInstance());
	// check(IsValid(GameInstance));
	//
	// GameInstance->StopPeerDiscovering();
}

void AWifiDirectDiscoverPC::Tick(const float DeltaSeconds)
{
	// Super::Tick(DeltaSeconds);
	//
	// PeerDiscoveringElapsed += DeltaSeconds;
	// if (PeerDiscoveringElapsed >= PeerDiscoveringInterval)
	// {
	// 	PeerDiscoveringElapsed = 0.0f;
	//
	// 	UShootingStarGameInstance* const GameInstance = Cast<UShootingStarGameInstance>(GetGameInstance());
	// 	check(IsValid(GameInstance));
	//
	// 	GameInstance->StartPeerDiscovering();
	// }
}
