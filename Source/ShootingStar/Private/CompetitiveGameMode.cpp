// Copyright 2025 ShootingStar. All Rights Reserved.


#include "CompetitiveGameMode.h"
#include "CompetitivePlayerController.h"
#include "CompetitiveGameState.h"
#include "CompetitiveSystemComponent.h"

ACompetitiveGameMode::ACompetitiveGameMode()
{
	PlayerControllerClass = ACompetitivePlayerController::StaticClass();
	GameStateClass = ACompetitiveGameState::StaticClass();
	CompetitiveSystemComponent = CreateDefaultSubobject<UCompetitiveSystemComponent>(TEXT("CompetitiveSystemComponent"));
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

	ACompetitivePlayerController* const CompetitivePlayerController = Cast<ACompetitivePlayerController>(NewPlayer);
	ensure(CompetitivePlayerController);
	if (!CompetitivePlayerController)
	{
		return;
	}

	UTeamComponent* const TeamComponent = CompetitivePlayerController->GetTeamComponent();
	check(CompetitiveSystemComponent);
	check(TeamComponent);
	FText FailReason;
	if (!CompetitiveSystemComponent->RegisterPlayer(CompetitivePlayerController, TeamComponent->GetTeam(), FailReason))
	{
		GEngine->AddOnScreenDebugMessage(0, 5.0f, FColor::Red, FailReason.ToString());
		CompetitiveSystemComponent->EndGame();
		return;
	}
	// 로비 등을 거치지 않고 게임을 시작한 경우, TeamComponent->Team은 ETeam::None이 되어 RegisterPlayer가 새로 임의의 팀에 할당함
	// 따라서 이러한 경우를 위해 다시 값을 설정
	TeamComponent->SetTeam(CompetitiveSystemComponent->GetTeamOf(CompetitivePlayerController));
}

void ACompetitiveGameMode::SwapPlayerControllers(APlayerController* const OldPC, APlayerController* const NewPC)
{
	Super::SwapPlayerControllers(OldPC, NewPC);

	UTeamComponent* const OldTeamComponent = Cast<UTeamComponent>(OldPC->GetComponentByClass(UTeamComponent::StaticClass()));
	UTeamComponent* const NewTeamComponent = Cast<UTeamComponent>(NewPC->GetComponentByClass(UTeamComponent::StaticClass()));
	if (OldTeamComponent == nullptr || NewTeamComponent == nullptr)
	{
		return;
	}
	NewTeamComponent->SetTeam(OldTeamComponent->GetTeam());
}

