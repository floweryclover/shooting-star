// Copyright 2025 ShootingStar. All Rights Reserved.

#include "CompetitiveGameMode.h"
#include "CompetitivePlayerController.h"
#include "CompetitiveGameState.h"
#include "CompetitiveSystemComponent.h"
#include "ClientComponent.h"
#include "InventoryComponent.h"
#include "WeaponData.h"
#include "Engine/World.h"
#include "MapGeneratorComponent.h"
#include "ShootingStar/ShootingStar.h"

ACompetitiveGameMode::ACompetitiveGameMode()
	: NumPlayers{1} // 호스트 항상 포함
{
	PrimaryActorTick.bCanEverTick = true;

	bUseSeamlessTravel = true;
	PlayerControllerClass = ACompetitivePlayerController::StaticClass();
	GameStateClass = ACompetitiveGameState::StaticClass();
	
	CompetitiveSystemComponent = CreateDefaultSubobject<UCompetitiveSystemComponent>(TEXT("CompetitiveSystemComponent"));
	MapGeneratorComponent = CreateDefaultSubobject<UMapGeneratorComponent>(TEXT("MapGeneratorComponent"));
}

void ACompetitiveGameMode::BeginPlay()
{
    Super::BeginPlay();

    // MapGeneratorComponent 초기화
    MapGeneratorComponent->Initialize();
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

void ACompetitiveGameMode::PostLogin(APlayerController* const NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UTeamComponent* const TeamComponent = Cast<UTeamComponent>(
		NewPlayer->GetComponentByClass(UTeamComponent::StaticClass()));
	check(TeamComponent != nullptr);

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

void ACompetitiveGameMode::InteractResource(AController* const Controller)
{
	if (!IsValid(Controller))
	{
		UE_LOG(LogTemp, Error, TEXT("Controller is Invalid!"));
		return;
	}
	APawn* const Pawn = Controller->GetPawn();
	if (!IsValid(Pawn))
	{
		UE_LOG(LogTemp, Error, TEXT("Pawn is Invalid!"));
		return;
	}

	UClientComponent* const ClientComponent = Cast<UClientComponent>(
		Controller->GetComponentByClass(UClientComponent::StaticClass()));
	UInventoryComponent* const InventoryComponent = Cast<UInventoryComponent>(
		Controller->GetComponentByClass(UInventoryComponent::StaticClass()));
	if (!IsValid(ClientComponent) || !IsValid(InventoryComponent))
	{
		return;
	}

	FVector Start = Pawn->GetActorLocation();
	Start.Z = 0.f;
	FRotator Rotation = Pawn->GetActorRotation();

	FVector End = Start + Rotation.Vector() * 100.f;

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f, 0, 1.0f);
	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, CollisionChannels::ResourceActor))
	{
		DrawDebugPoint(GetWorld(), Hit.Location, 10, FColor::Red, false, 2.0f);
		AResourceActor* Resource = Cast<AResourceActor>(Hit.GetActor());
		
		InventoryComponent->AddResource(Resource->ResourceData);
	}
}

void ACompetitiveGameMode::CraftWeapon(AController* const Controller, const FWeaponData& Weapon,
                                       const TArray<int32>& Resources)
{
	if (!IsValid(Controller))
	{
		return;
	}

	UInventoryComponent* const InventoryComponent = Cast<UInventoryComponent>(
		Controller->GetComponentByClass(UInventoryComponent::StaticClass()));
	UClientComponent* const ClientComponent = Cast<UClientComponent>(
		Controller->GetComponentByClass(UClientComponent::StaticClass()));
	if (!IsValid(InventoryComponent)
		|| !IsValid(ClientComponent))
	{
		return;
	}

	// 실제로 자원을 가졌는지 검증
	const TArray<FResourceInventoryData>& ResourcesHave = InventoryComponent->GetAllResources();
	for (int i=0; i<static_cast<int>(EResourceType::End); ++i)
	{
		if (Resources[i] <= 0)
		{
			continue;
		}
		
		if (!ResourcesHave[i].Resource)
		{
			return;
		}

		if (ResourcesHave[i].Count < Resources[i])
		{
			return;
		}
	}
	
	const FWeaponData CraftedWeapon = InventoryComponent->Craft_Weapon(Weapon, Resources);
	ClientComponent->GainWeapon(CraftedWeapon);
}
