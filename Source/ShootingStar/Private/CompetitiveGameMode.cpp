// Copyright 2025 ShootingStar. All Rights Reserved.

#include "CompetitiveGameMode.h"
#include "CompetitivePlayerController.h"
#include "CompetitiveGameState.h"
#include "CompetitiveSystemComponent.h"
#include "CompetitivePlayerCharacter.h"
#include "InventoryComponent.h"
#include "WeaponData.h"
#include "Engine/World.h"
#include "MapGeneratorComponent.h"
#include "GameFramework/PlayerState.h"
#include "ShootingStar/ShootingStar.h"
#include "Components/StaticMeshComponent.h"
#include "SafeZoneActor.h"
#include "SupplyActor.h"
#include "ResourceActor.h"
#include "Kismet/GameplayStatics.h"

ACompetitiveGameMode::ACompetitiveGameMode()
	: NumPlayers{1} // 호스트 항상 포함
{
	PrimaryActorTick.bCanEverTick = true;

	bUseSeamlessTravel = true;
	PlayerControllerClass = ACompetitivePlayerController::StaticClass();
	GameStateClass = ACompetitiveGameState::StaticClass();

	CompetitiveSystemComponent = CreateDefaultSubobject<
		UCompetitiveSystemComponent>(TEXT("CompetitiveSystemComponent"));
	MapGeneratorComponent = CreateDefaultSubobject<UMapGeneratorComponent>(TEXT("MapGeneratorComponent"));
}

void ACompetitiveGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// MapGeneratorComponent 초기화
	MapGeneratorComponent->Initialize();
	MapGeneratorComponent->OnActorBeginOverlapOnTumbleWeed.AddDynamic(this, &ACompetitiveGameMode::OnActorBeginOverlapOnTumbleWeedHandler);
	MapGeneratorComponent->OnActorEndOverlapOnTumbleWeed.AddDynamic(this, &ACompetitiveGameMode::OnActorEndOverlapOnTumbleWeedHandler);

	// SafeZone 액터 생성
	SafeZoneActor = GetWorld()->SpawnActor<ASafeZoneActor>(SafeZoneActorClass);
	if (!IsValid(SafeZoneActor))
	{
		UE_LOG(LogShootingStar, Error, TEXT("Failed to create SafeZone."));
	}

	CompetitiveSystemComponent->Init([this]() { return SafeZoneActor->GetRadius(); });
	CompetitiveSystemComponent->OnGameStarted.AddDynamic(this, &ACompetitiveGameMode::OnGameStarted);
	CompetitiveSystemComponent->OnSupplyDropped.AddDynamic(this, &ACompetitiveGameMode::HandleSupplyDrop);
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
		if (CompetitiveSystemComponent->GetCurrentPhaseTime() > WaitingForGameStartSeconds)
		{
			CompetitiveSystemComponent->StartGame();
		}
	}
	else if (CurrentPhase == ECompetitiveGamePhase::GameDestroyed)
	{
		GetWorld()->ServerTravel(ExitLevel.ToString());
	}
	else if (CurrentPhase == ECompetitiveGamePhase::Game)
	{
		// 자기장 업데이트
		SafeZoneActor->SetRadiusByAlpha(CompetitiveSystemComponent->GetSafeZoneAlpha());

		// 자기장 밖 플레이어들 데미지
		TimeElapsedLastSafeZoneDamaged += DeltaSeconds;
		if (TimeElapsedLastSafeZoneDamaged >= IntervalSafeZoneDamageApply)
		{
			TimeElapsedLastSafeZoneDamaged = 0.0f;
			const float RadiusSquaredSafeZone = SafeZoneActor->GetRadius() * SafeZoneActor->GetRadius();
			for (APlayerState* const PlayerState : GameState->PlayerArray)
			{
				APlayerController* const PlayerController = PlayerState->GetPlayerController();
				if (!IsValid(PlayerController) || !IsValid(PlayerController->GetCharacter()))
				{
					continue;
				}

				ACharacter* const Character = PlayerController->GetCharacter();
				const float DistanceSquaredFromCenter = Character->GetActorLocation().SizeSquared2D();
				if (DistanceSquaredFromCenter > RadiusSquaredSafeZone)
				{
					Character->SetActorHiddenInGame(true);
					UGameplayStatics::ApplyDamage(Character, DamageSafeZone, nullptr, SafeZoneActor, nullptr);
				}
			}
		}
	}

	// 리스폰해야 하는 플레이어 리스폰
	for (APlayerState* const PlayerState : GameState->PlayerArray)
	{
		APlayerController* const PlayerController = PlayerState->GetPlayerController();
		if (!IsValid(PlayerController) || IsValid(PlayerController->GetPawn()))
		{
			continue;
		}

		RestartPlayer(PlayerController);
	}
}

void ACompetitiveGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
                                    FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	if (NumPlayers >= UCompetitiveSystemComponent::MaxPlayersPerTeam * 2)
	{
		ErrorMessage = TEXT("정원이 가득 찼습니다.");
		return;
	}
	NumPlayers += 1;
}

void ACompetitiveGameMode::OnPostLogin(AController* const NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
}

void ACompetitiveGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	// 누군가 게임을 도중에 나갈 경우 게임 즉시 종료
	const ECompetitiveGamePhase Phase = CompetitiveSystemComponent->GetCurrentPhase();
	if (Phase != ECompetitiveGamePhase::GameEnd && Phase != ECompetitiveGamePhase::GameDestroyed)
	{
		CompetitiveSystemComponent->EndGame();
	}

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

APawn* ACompetitiveGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer,
                                                                        const FTransform& SpawnTransform)
{
	if (!DefaultPawnClass)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = nullptr;
	SpawnInfo.Owner = NewPlayer;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	// 충돌 무시하고 스폰

	APawn* const Pawn = GetWorld()->SpawnActor<APawn>(
		DefaultPawnClass, SpawnTransform, SpawnInfo);;

	return Pawn;
}

void ACompetitiveGameMode::RestartPlayer(AController* const NewPlayer)
{
	ACompetitivePlayerController* const CompetitivePlayerController = Cast<ACompetitivePlayerController>(NewPlayer);
	if (!IsValid(NewPlayer) || !IsValid(CompetitivePlayerController))
	{
		return;
	}

	if (APawn* const OldPawn = NewPlayer->GetPawn(); IsValid(OldPawn))
	{
		NewPlayer->UnPossess();
		OldPawn->Destroy();
	}

	const FVector SpawnPoint = GetMostIsolatedSpawnPointFor(CompetitivePlayerController);
	ACompetitivePlayerCharacter* const CompetitivePlayerCharacter = Cast<ACompetitivePlayerCharacter>(
		SpawnDefaultPawnAtTransform(NewPlayer, FTransform{SpawnPoint + FVector{0.0, 0.0, 100.0}}));
	AssignTeamIfNone(CompetitivePlayerController);
	UTeamComponent* const TeamComponent = CompetitivePlayerController->GetTeamComponent();
	CompetitivePlayerCharacter->GetTeamComponent()->SetTeam(TeamComponent->GetTeam());
	CompetitivePlayerCharacter->SetPlayerName(NewPlayer->GetPlayerState<APlayerState>()->GetPlayerName());

	NewPlayer->Possess(CompetitivePlayerCharacter);
}

void ACompetitiveGameMode::HandleKill(AActor* const Killee)
{
	// 지금 게임 중인지 검증
	if (CompetitiveSystemComponent->GetCurrentPhase() != ECompetitiveGamePhase::Game)
	{
		return;
	}

	if (!IsValid(Killee))
	{
		UE_LOG(LogShootingStar, Error, TEXT("HandleKill() failed - Killee is invalid."));
		return;
	}

	// 팀이 유효한지 검증
	UTeamComponent* const TeamComponent_Killee = Cast<UTeamComponent>(
		Killee->GetComponentByClass(UTeamComponent::StaticClass()));
	if (!IsValid(TeamComponent_Killee) || TeamComponent_Killee->GetTeam() == ETeam::None)
	{
		return;
	}

	CompetitiveSystemComponent->GiveKillScoreForTeam(TeamComponent_Killee->GetTeam() == ETeam::Blue
		                                                 ? ETeam::Red
		                                                 : ETeam::Blue);
}

FVector ACompetitiveGameMode::GetMostIsolatedSpawnPointFor(APlayerController* const Player) const
{
	if (!IsValid(Player))
	{
		return {};
	}

	TArray<FVector> OtherPlayerPositions;
	for (const APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (!IsValid(PlayerState->GetPlayerController())
			|| !IsValid(PlayerState->GetPlayerController()->GetPawn())
			|| PlayerState->GetPlayerController() == Player)
		{
			continue;
		}

		const APawn* const Pawn = PlayerState->GetPlayerController()->GetPawn();
		OtherPlayerPositions.Add(Pawn->GetActorLocation());
	}

	TArray<FVector> SpawnPointsDescending; // Z값은 거리합으로 사용, 거리합이 큰 순서대로 정렬됨
	const TArray<FVector>& PlayerSpawnPoints = MapGeneratorComponent->GetPlayerSpawnPoints();
	for (const FVector& SpawnPoint : PlayerSpawnPoints)
	{
		double DistanceSquaredSum = 0.0;
		for (const FVector& OtherPlayerPosition : OtherPlayerPositions)
		{
			DistanceSquaredSum += FVector::DistSquaredXY(SpawnPoint, OtherPlayerPosition);
		}

		SpawnPointsDescending.Add({SpawnPoint.X, SpawnPoint.Y, DistanceSquaredSum});
	}

	Algo::Sort(SpawnPointsDescending, [](const FVector& Lhs, const FVector& Rhs) { return Lhs.Z > Rhs.Z; });

	FVector ReturnValue;
	const float SafeZoneRadius = SafeZoneActor->GetRadius();

	for (const FVector& SpawnPoint : SpawnPointsDescending)
	{
		ReturnValue = SpawnPoint;
		ReturnValue.Z = 0.0f;

		if (SpawnPoint.SizeSquared2D() < SafeZoneRadius * SafeZoneRadius)
		{
			break;
		}

		UE_LOG(LogShootingStar, Log, TEXT("%f, %f는 자기장 밖에 있습니다."), SpawnPoint.X, SpawnPoint.Y);
	}

	return ReturnValue;
}

void ACompetitiveGameMode::AssignTeamIfNone(APlayerController* Player)
{
	UTeamComponent* const TeamComponent = Cast<UTeamComponent>(
		Player->GetComponentByClass(UTeamComponent::StaticClass()));
	check(IsValid(TeamComponent));

	// 로비를 거치지 않고 시작한 경우 등 팀이 없는 경우에는 새로 할당
	if (TeamComponent->GetTeam() == ETeam::None)
	{
		const ETeam TeamToAssign = CompetitiveSystemComponent->GetTeamForNextPlayer(
			GetGameState<AGameStateBase>()->PlayerArray);

		// 혹시 팀 할당에 실패했으면 즉시 게임 종료
		if (TeamToAssign == ETeam::None)
		{
			CompetitiveSystemComponent->EndGame();
			return;
		}
		TeamComponent->SetTeam(TeamToAssign);
	}
}

void ACompetitiveGameMode::OnGameStarted()
{
	// 플레이어들 재시작
	for (APlayerState* const PlayerState : GameState->PlayerArray)
	{
		APlayerController* const Player = PlayerState->GetPlayerController();
		if (!IsValid(Player))
		{
			continue;
		}

		RestartPlayer(Player);
	}

	// 보급품 제거
	for (ASupplyActor* const SupplyActor : SupplyActors)
	{
		SupplyActor->Destroy();
	}
	SupplyActors.Empty();

	TimeElapsedLastSafeZoneDamaged = 0.0f;
}

void ACompetitiveGameMode::HandleSupplyDrop(FVector Location)
{
	if (!SupplyActorClass)
	{
		UE_LOG(LogShootingStar, Error, TEXT("SupplyActorClass is not set in GameMode"));
		return;
	}

	// SupplyActor 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (ASupplyActor* SupplyActor = GetWorld()->SpawnActor<ASupplyActor>(
		SupplyActorClass, Location, FRotator::ZeroRotator, SpawnParams))
	{
		SupplyActor->SetReplicates(true);
		SupplyActors.Add(SupplyActor);
		UE_LOG(LogShootingStar, Log, TEXT("Supply %d spawned at %s"), SupplyActors.Num()-1, *Location.ToString());
	}
	else
		UE_LOG(LogShootingStar, Error, TEXT("Failed to spawn SupplyActor"));
}

void ACompetitiveGameMode::OnActorBeginOverlapOnTumbleWeedHandler(AActor* OverlappedActor, AActor* OtherActor)
{
	if (ACompetitivePlayerCharacter* const Character = Cast<ACompetitivePlayerCharacter>(OtherActor);
		IsValid(Character))
	{
		Character->SetActorHiddenInGame(true);
	}
}

void ACompetitiveGameMode::OnActorEndOverlapOnTumbleWeedHandler(AActor* OverlappedActor, AActor* OtherActor)
{
	if (ACompetitivePlayerCharacter* const Character = Cast<ACompetitivePlayerCharacter>(OtherActor);
	IsValid(Character))
	{
		Character->SetActorHiddenInGame(false);
	}
}
