// Copyright 2025 ShootingStar. All Rights Reserved.


#include "LobbyNetworkComponent.h"

#include "CompetitiveSystemComponent.h"
#include "LobbyPlayerController.h"
#include "WifiDirectInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "ShootingStar/ShootingStar.h"

ULobbyNetworkComponent::ULobbyNetworkComponent()
	: bIsP2pSession{false},
	  bIsLocalComponent{false},
	  NumPreviousPlayers{false}
{
}

void ULobbyNetworkComponent::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* const PlayerController = Cast<APlayerController>(GetOwner());
	bIsLocalComponent = PlayerController != nullptr && PlayerController->IsLocalController();
	if (!bIsLocalComponent)
	{
		return;
	}

	UWifiDirectInterface* Interface = UWifiDirectInterface::GetWifiDirectInterface();
	bIsP2pSession = Interface->IsP2pGroupFormed();
	if (bIsP2pSession && !Interface->IsP2pGroupOwner())
	{
		Interface->StopBroadcastAndDiscovery();
		bIsP2pSession = false; // 일반 클라이언트인 경우 업데이트 방지를 위해 false 설정
	}
}

void ULobbyNetworkComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (bIsLocalComponent)
	{
		UWifiDirectInterface::GetWifiDirectInterface()->StopBroadcastAndDiscovery();
	}
}

void ULobbyNetworkComponent::Process(const int32 NumCurrentPlayers, const int32 NumMaxPlayers, const float DeltaTime)
{
	if (!bIsLocalComponent || !bIsP2pSession)
	{
		return;
	}

	UWifiDirectInterface* Interface = UWifiDirectInterface::GetWifiDirectInterface();
	Interface->Update(DeltaTime);

	// P2P 그룹 해체되면 종료
	if (!Interface->IsP2pGroupFormed())
	{
		UGameplayStatics::OpenLevel(GetWorld(), "/Game/Levels/MainMenu");
		bIsP2pSession = false;
		return;
	}

	// 이번에 가득 차게 된 방이라면 검색되지 않게
	if (NumCurrentPlayers >= NumMaxPlayers && NumPreviousPlayers < NumMaxPlayers)
	{
		Interface->StopBroadcastAndDiscovery();
	}
	else if (NumCurrentPlayers < NumMaxPlayers && NumPreviousPlayers >= NumMaxPlayers) // 이번에 다시 여유롭게 된 방이라면 검색되게
	{
		Interface->RegisterService();
	}

	NumPreviousPlayers = NumCurrentPlayers;
}

void ULobbyNetworkComponent::StartGame()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	const FName CompetitiveLevelName = TEXT(
		"/Game/Levels/Competitive");
	const FString Options = CompetitiveLevelName.ToString();
	GetWorld()->ServerTravel(Options);
}

void ULobbyNetworkComponent::LeaveGame()
{
	FWorldContext* const WorldContext = GetWorld()->GetGameInstance()->GetWorldContext();
	GEngine->BrowseToDefaultMap(*WorldContext);
}

void ULobbyNetworkComponent::SetTeam_Implementation(ETeam Team)
{
	ALobbyPlayerController* const PlayerController = Cast<ALobbyPlayerController>(GetOwner());
	
	const ETeam CurrentTeam = PlayerController->GetTeamComponent()->GetTeam();
	if (Team == CurrentTeam)
	{
		return;
	}

	int NumPlayersOfRequestedTeam = 0;
	for (APlayerState* PlayerState : GetWorld()->GetAuthGameMode()->GameState->PlayerArray)
	{
		ALobbyPlayerController* const OtherPlayer = Cast<ALobbyPlayerController>(PlayerState->GetPlayerController());
		if (!IsValid(OtherPlayer))
		{
			continue;
		}

		const ETeam OtherTeam = OtherPlayer->GetTeamComponent()->GetTeam();
		if (OtherTeam == Team)
		{
			NumPlayersOfRequestedTeam += 1;
		}
	}

	if (NumPlayersOfRequestedTeam >= UCompetitiveSystemComponent::MaxPlayersPerTeam)
	{
		return;
	}
	PlayerController->GetTeamComponent()->SetTeam(Team);
}

void ULobbyNetworkComponent::SetNickname_Implementation(const FString& NewNickname)
{
	APlayerController* const PlayerController = Cast<APlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}

	PlayerController->SetName(NewNickname);
}
