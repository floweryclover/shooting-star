// Copyright 2025 ShootingStar. All Rights Reserved.

#include "CompetitiveGameMode.h"
#include "CompetitivePlayerController.h"
#include "CompetitiveGameState.h"
#include "CompetitiveSystemComponent.h"
#include "ClientComponent.h"
#include "InventoryComponent.h"
#include "WeaponData.h"
#include "Engine/World.h"
#include "ObstacleGenerator.h"
#include "SubObstacleGenerator.h"
#include "FenceGenerator.h"
#include "ResourceGenerator.h"
#include "DecorationGenerator.h"
#include "Engine/StaticMeshActor.h"
#include "ShootingStar/ShootingStar.h"

DEFINE_LOG_CATEGORY(MapGenerator);

ACompetitiveGameMode::ACompetitiveGameMode()
	: NumPlayers{1} // 호스트 항상 포함
{
	PrimaryActorTick.bCanEverTick = true;

	bUseSeamlessTravel = true;
	PlayerControllerClass = ACompetitivePlayerController::StaticClass();
	GameStateClass = ACompetitiveGameState::StaticClass();
	CompetitiveSystemComponent = CreateDefaultSubobject<
		UCompetitiveSystemComponent>(TEXT("CompetitiveSystemComponent"));
	
	// Fence InstancedMeshComponent 생성
    FenceInstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FenceInstancedMesh"));

	// Generator 객체들을 생성자에서 초기화
    obstacleGenerator = CreateDefaultSubobject<UObstacleGenerator>(TEXT("ObstacleGenerator"));
    subObstacleGenerator = CreateDefaultSubobject<USubObstacleGenerator>(TEXT("SubObstacleGenerator"));
    fenceGenerator = CreateDefaultSubobject<UFenceGenerator>(TEXT("FenceGenerator"));
    resourceGenerator = CreateDefaultSubobject<UResourceGenerator>(TEXT("ResourceGenerator")); 
    decorationGenerator = CreateDefaultSubobject<UDecorationGenerator>(TEXT("DecorationGenerator"));

}

void ACompetitiveGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    // Decoration InstancedMeshComponents 초기화 및 생성
    DecorationInstancedMeshComponents.Empty();
    for (int32 i = 0; i < decoMeshes.Num(); ++i)
    {
        FString CompName = FString::Printf(TEXT("DecorationInstancedMesh_%d"), i);
        UInstancedStaticMeshComponent* NewComp = NewObject<UInstancedStaticMeshComponent>(this, *CompName);
        if (NewComp)
        {
            NewComp->RegisterComponent();
            NewComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
            NewComp->SetStaticMesh(decoMeshes[i]);
            DecorationInstancedMeshComponents.Add(NewComp);
        }
    }

    // Generator 초기화
    obstacleGenerator->Initialize(this);
    subObstacleGenerator->Initialize(this);
    fenceGenerator->Initialize(this);
    resourceGenerator->Initialize(this);
    decorationGenerator->Initialize(this);

    // 절차적 생성 시작
    InitializeMapCoordinate(mapHalfSize * 2);
    GenerateMap();
}

#pragma region 맵 생성
void ACompetitiveGameMode::InitializeMapCoordinate(int32 GridSize)
{
    mapCoordinate.SetNum(GridSize * GridSize);
}

void ACompetitiveGameMode::GenerateMap()
{
    UE_LOG(MapGenerator, Log, TEXT("Generate Map Started"));

    obstacleGenerator->GenerateObjects();
    subObstacleGenerator->GenerateObjects();
    fenceGenerator->GenerateObjects();
    resourceGenerator->GenerateObjects();
    decorationGenerator->GenerateObjects();

    UE_LOG(MapGenerator, Log, TEXT("Generate Map Completed"));
}

void ACompetitiveGameMode::SetObjectAtArray(int32 X, int32 Y, EObjectMask ObjectType)
{
	int32 Index = GetIndex(X, Y);
    mapCoordinate[Index] |= static_cast<uint8>(ObjectType); // 비트 설정
}

void ACompetitiveGameMode::SetObjectRegion(FVector Location, UStaticMesh* ObjectMesh, EObjectMask ObjectType)
{
    if (!ObjectMesh) return;

    // 바운딩 박스 가져오기
    FBoxSphereBounds Bounds = ObjectMesh->GetBounds();
    FVector Extent = Bounds.BoxExtent; // 바운딩 박스의 반경
    FVector Min = Location - Extent;  // 최소 좌표
    FVector Max = Location + Extent;  // 최대 좌표

    // 맵 좌표계로 변환
    int32 MinX = FMath::FloorToInt(Min.X);
    int32 MinY = FMath::FloorToInt(Min.Y);
    int32 MaxX = FMath::CeilToInt(Max.X);
    int32 MaxY = FMath::CeilToInt(Max.Y);

    if (MinX < -mapHalfSize) MinX = -mapHalfSize;
    if (MinY < -mapHalfSize) MinY = -mapHalfSize;
    if (MaxX >= mapHalfSize) MaxX = mapHalfSize - 1;
    if (MaxY >= mapHalfSize) MaxY = mapHalfSize - 1;

    // 해당 영역의 모든 좌표를 설정
    for (int32 X = MinX; X <= MaxX; ++X)
    {
        for (int32 Y = MinY; Y <= MaxY; ++Y)
        {
            SetObjectAtArray(X, Y, ObjectType);
        }
    }
}

bool ACompetitiveGameMode::HasObjectAtArray(int32 X, int32 Y, EObjectMask ObjectType)
{
    int32 Index = GetIndex(X, Y);
    return (mapCoordinate[Index] & static_cast<uint8>(ObjectType)) != 0; // 비트 확인
}

FVector ACompetitiveGameMode::GetRandomPosition()
{
    float X = FMath::RandRange(-mapHalfSize, mapHalfSize);
    float Y = FMath::RandRange(-mapHalfSize, mapHalfSize);
    return FVector(X, Y, 0.f);
}

// 특정 좌표를 기준으로 Offset 이내의 좌표를 FVector로 반환하는 함수
FVector ACompetitiveGameMode::GetRandomOffsetPosition(FVector origin, float offset)
{
    float X = FMath::RandRange(origin.X - offset, origin.X + offset);
    float Y = FMath::RandRange(origin.Y - offset, origin.Y + offset);
    return FVector(X, Y, 0.f);
}

/// 해당 위치가 유효한지 검사하는 함수 (충돌 방지)
bool ACompetitiveGameMode::CheckLocation(FVector Location)
{
	// 좌표를 맵 배열의 인덱스로 변환
    int32 X = FMath::RoundToInt(Location.X);
    int32 Y = FMath::RoundToInt(Location.Y);

    // 1. 맵 범위를 벗어나는지 확인
    if (X < -mapHalfSize || X >= mapHalfSize || Y < -mapHalfSize || Y >= mapHalfSize)
    {
        UE_LOG(MapGenerator, Warning, TEXT("Location (%d, %d) is out of bounds."), X, Y);
        return false;
    }

    // 2. 해당 좌표에 이미 오브젝트가 있는지 확인
    if (HasObjectAtArray(X, Y, EObjectMask::ObstacleMask) ||
        HasObjectAtArray(X, Y, EObjectMask::SubObstacleMask) ||
        HasObjectAtArray(X, Y, EObjectMask::FenceMask) ||
        HasObjectAtArray(X, Y, EObjectMask::ResourceMask) ||
        HasObjectAtArray(X, Y, EObjectMask::DecoMask))
    {
        UE_LOG(MapGenerator, Warning, TEXT("Location (%d, %d) is already occupied."), X, Y);
        return false;
    }

    // 위치가 유효함
    return true;
}

FVector ACompetitiveGameMode::FindNearestValidLocation(FVector Origin, float SearchRadius, EObjectMask ObjectType)
{
	const int32 NumDirections = 8;
    const float AngleStep = 360.0f / NumDirections;
    
    for (float CurrentRadius = 100.f; CurrentRadius <= SearchRadius; CurrentRadius += 100.f)
    {
        for (int32 i = 0; i < NumDirections; ++i)
        {
            float Angle = i * AngleStep;
            FVector Offset(
                CurrentRadius * FMath::Cos(FMath::DegreesToRadians(Angle)),
                CurrentRadius * FMath::Sin(FMath::DegreesToRadians(Angle)),
                0.0f
            );
            
            FVector TestLocation = Origin + Offset;
            if (CheckLocation(TestLocation))
                return TestLocation;
        }
    }
    
    return Origin;
}

// Static Mesh Actor를 원하는 위치에 생성하는 함수
bool ACompetitiveGameMode::PlaceObject(FVector Location, UStaticMesh* ObjectMesh)
{
    if (!ObjectMesh)
    {
        UE_LOG(MapGenerator, Error, TEXT("Invalid ObjectMesh!"));
        return false;
    }
    else if (!GetWorld())
    {
        UE_LOG(MapGenerator, Error, TEXT("Invalid World!"));
        return false;
    }

    // StaticMeshActor 생성
    FActorSpawnParameters SpawnParams;
    AStaticMeshActor* NewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);

    if (NewActor)
    {
        NewActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
        NewActor->GetStaticMeshComponent()->SetStaticMesh(ObjectMesh);
        NewActor->SetActorLocation(Location);

        UE_LOG(MapGenerator, Log, TEXT("Placed StaticMeshActor at %s"), *Location.ToString());
        return true;
    }

    return false;
}
#pragma endregion

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

		// 서버용 자원 획득 함수(TMap은 UPROPERTY(Replicated)미지원)
		InventoryComponent->AddResource(Resource->ResourceData);

		// 클라이언트용 자원 획득 함수(동기화용)
		if (!Controller->IsLocalController())
		{
			ClientComponent->GainResource(Resource->ResourceData);
		}
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
	const TArray<FAA>& ResourcesHave = InventoryComponent->GetAllResources();
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
