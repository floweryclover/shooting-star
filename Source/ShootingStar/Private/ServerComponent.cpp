// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerComponent.h"

#include "ClientComponent.h"

// Sets default values for this component's properties
UServerComponent::UServerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UServerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UServerComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UServerComponent::ServerPleaseMakeMeJump_Implementation()
{
	if (UClientComponent* ClientComponent = Cast<UClientComponent>(
		GetOwner()->GetComponentByClass(UClientComponent::StaticClass())))
	{
		ClientComponent->JumpClient();
	}
}

void UServerComponent::GoodSpawn_Implementation(UClass* ActorToSpawn)
{
	if (UWorld* World = GetWorld())
	{
		// 이렇게 서버측에서 생성하면, 언리얼 측은 자동으로 클라이언트의 원격 클라이언트에게 복제합니다.
		FActorSpawnParameters Parameters{};
		Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FVector Location { 1000.0, 1810.0, 200.0 };
		FRotator Rotation = FRotator::ZeroRotator;
		World->SpawnActor(ActorToSpawn, &Location, &Rotation, Parameters);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GetWorld() was nullptr"));
	}
}

void UServerComponent::BadSpawn(UClass* ActorToSpawn)
{
	if (UWorld* World = GetWorld())
	{
		// 완전히 동일한 동작이지만,
		// 만약 클라이언트 컴퓨터측에서 바로 생성하면 자신을 제외한 다른 컴퓨터들에게는 반영되지 않습니다.
		// 다만 서버 컴퓨터측에서 호출한다면, 원본 컴퓨터에 생성하는 동작이기 때문에, 정상적으로 다른 클라이언트에게 복제될 것입니다.
		FActorSpawnParameters Parameters{};
		Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		FVector Location { 1000.0, 1810.0, 200.0 };
		FRotator Rotation = FRotator::ZeroRotator;
		World->SpawnActor(ActorToSpawn, &Location, &Rotation, Parameters);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GetWorld() was nullptr"));
	}
}

