// Copyright 2025 ShootingStar. All Rights Reserved.


#include "EnvironmentActorSpawner.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnvironmentActorSpawner::AEnvironmentActorSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = Root;
}

// Called when the game starts or when spawned
void AEnvironmentActorSpawner::BeginPlay()
{
	Super::BeginPlay();
	Spawn();
}

// Called every frame
void AEnvironmentActorSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// if Total SpawnNumber == 0, Infinite 
	if (!ActorToSpawn || (TotalSpawnNumber && CurSpawnNumber > TotalSpawnNumber)) return;

	TimeSum += DeltaTime;
	if (TimeSum >= CoolTime)
	{
		TimeSum = 0.f;
		Spawn();
	}
}

void AEnvironmentActorSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FRotator Rotator = GetActorRotation();

#if WITH_EDITOR
	FlushPersistentDebugLines(GetWorld());

	DrawDebugBox(
		GetWorld(),
		GetActorLocation(),
		SpawnExtent,
		Rotator.Quaternion(),
		FColor::Green,
		true,
		-1.f,
		0,
		10.f
	);

#endif
}

void AEnvironmentActorSpawner::Spawn()
{
	CurSpawnNumber++;

	FVector Origin = GetActorLocation();
	FVector RandomLocation = UKismetMathLibrary::RandomPointInBoundingBox(Origin, SpawnExtent);
	float Radius = FVector::Dist2D(Origin, RandomLocation);
	float RadZ = FMath::DegreesToRadians(GetActorRotation().Yaw + 90);
	RandomLocation.X = Origin.X + Radius * cosf((RadZ));
	RandomLocation.Y = Origin.Y + Radius * sinf(RadZ);

	GetWorld()->SpawnActor<AActor>(ActorToSpawn, RandomLocation, FRotator::ZeroRotator);
}

