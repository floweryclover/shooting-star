// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitiveGameMode.h"
#include "CompetitivePlayerController.h"
#include "CompetitiveGameState.h"
#include "CompetitiveSystemComponent.h"

ACompetitiveGameMode::ACompetitiveGameMode()
	: NumPlayers{1} // 호스트 항상 포함
{
	PrimaryActorTick.bCanEverTick = true;

	PlayerControllerClass = ACompetitivePlayerController::StaticClass();
	GameStateClass = ACompetitiveGameState::StaticClass();
	CompetitiveSystemComponent = CreateDefaultSubobject<
		UCompetitiveSystemComponent>(TEXT("CompetitiveSystemComponent"));
}

void ACompetitiveGameMode::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	CompetitiveSystemComponent->Update(DeltaSeconds);
	const ECompetitiveGamePhase CurrentPhase = CompetitiveSystemComponent->GetCurrentPhase();
	if (CurrentPhase == ECompetitiveGamePhase::WaitingForStart)
	{
		// 게임 입장 후 10초가 지나면 시작하는 코드
		// 추후 준비 완료 또는 모두 로그인 완료 시 시작으로 구체화 해야 할 것
		if (CompetitiveSystemComponent->GetCurrentPhaseTime() > GameStartSeconds)
		{
			CompetitiveSystemComponent->StartGame();
		}
	}
	else if (CurrentPhase == ECompetitiveGamePhase::GameDestroyed)
	{
		GetWorld()->ServerTravel(ExitLevel.ToString());
	}
}

void ACompetitiveGameMode::OnPostLogin(AController* const NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	UTeamComponent* const TeamComponent = Cast<UTeamComponent>(
		NewPlayer->GetComponentByClass(UTeamComponent::StaticClass()));
	check(TeamComponent != nullptr);

	// 로비를 거치지 않고 시작한 경우 등 팀이 없는 경우에는 새로 할당
	if (TeamComponent->GetTeam() == ETeam::None)
	{
		const ETeam TeamToAssign = CompetitiveSystemComponent->GetTeamForNextPlayer(GetGameState<AGameStateBase>()->PlayerArray);
		
		// 혹시 팀 할당에 실패했으면 즉시 게임 종료
		if (TeamToAssign == ETeam::None)
		{
			CompetitiveSystemComponent->EndGame();
			return;
		}
		TeamComponent->SetTeam(TeamToAssign);
	}
}

void ACompetitiveGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
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

void ACompetitiveGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	NumPlayers -= 1;
}

void ACompetitiveGameMode::SwapPlayerControllers(APlayerController* const OldPC, APlayerController* const NewPC)
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
