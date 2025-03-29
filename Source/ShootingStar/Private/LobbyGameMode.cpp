// Copyright 2025 ShootingStar. All Rights Reserved.


#include "LobbyGameMode.h"
#include "CompetitiveSystemComponent.h"
#include "LobbyPlayerController.h"
#include "LobbyGameState.h"
#include "WifiDirectInterface.h"

ALobbyGameMode::ALobbyGameMode()
	: NumPlayers{1}, // 항상 호스트 포함
	  WifiDirectRefreshElapsed{0.0f}
{
	PrimaryActorTick.bCanEverTick = true;
	bUseSeamlessTravel = true;
	PlayerControllerClass = ALobbyPlayerController::StaticClass();
	GameStateClass = ALobbyGameState::StaticClass();

	CompetitiveSystemComponent = CreateDefaultSubobject<
		UCompetitiveSystemComponent>(TEXT("CompetitiveSystemComponent"));
}

int32 ALobbyGameMode::GetNumPlayers()
{
	Super::GetNumPlayers();

	return NumPlayers;
}

void ALobbyGameMode::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UWifiDirectInterface* const Interface = UWifiDirectInterface::GetWifiDirectInterface();
	if (Interface->IsP2pGroupFormed() && Interface->IsP2pGroupOwner())
	{
		WifiDirectRefreshElapsed += DeltaSeconds;
		if (WifiDirectRefreshElapsed >= WifiDirectRefreshInterval)
		{
			WifiDirectRefreshElapsed = 0.0f;
			Interface->Refresh();
		}
	}
}

void ALobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
                              FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	if (NumPlayers >= CompetitiveSystemComponent->GetMaxPlayersPerTeam() * 2)
	{
		ErrorMessage = TEXT("정원이 가득 찼습니다.");
		return;
	}

	NumPlayers += 1;
}

void ALobbyGameMode::PostLogin(APlayerController* const NewPlayer)
{
	Super::PostLogin(NewPlayer);

	const ETeam TeamToAssign = CompetitiveSystemComponent->GetTeamForNextPlayer(
		GetGameState<AGameStateBase>()->PlayerArray);
	if (TeamToAssign == ETeam::None)
	{
		// 플레이어 팀 할당 실패, 추방
		Logout(NewPlayer);
		return;
	}

	UTeamComponent* const TeamComponent = Cast<UTeamComponent>(
		NewPlayer->GetComponentByClass(UTeamComponent::StaticClass()));
	check(TeamComponent != nullptr);

	TeamComponent->SetTeam(TeamToAssign);
}

void ALobbyGameMode::Logout(AController* const Exiting)
{
	Super::Logout(Exiting);

	NumPlayers -= 1;
}

void ALobbyGameMode::SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC)
{
	Super::SwapPlayerControllers(OldPC, NewPC);

	UTeamComponent* const OldTeamComponent = Cast<UTeamComponent>(
		OldPC->GetComponentByClass(UTeamComponent::StaticClass()));
	UTeamComponent* const NewTeamComponent = Cast<UTeamComponent>(
		NewPC->GetComponentByClass(UTeamComponent::StaticClass()));
	if (OldTeamComponent == nullptr || NewTeamComponent == nullptr)
	{
		return;
	}
	NewTeamComponent->SetTeam(OldTeamComponent->GetTeam());
}
