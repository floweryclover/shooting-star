// Copyright 2025 ShootingStar. All Rights Reserved.


#include "WindPusher.h"

// Sets default values for this component's properties
UWindPusher::UWindPusher()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWindPusher::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (Owner)
		OwnerMesh = Owner->FindComponentByClass<UStaticMeshComponent>();

	BaseTime = FMath::FRandRange(1.f, 2.f);
	Push(ImplusePower_Initial);
}


// Called every frame
void UWindPusher::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerMesh)
		return;

	AccTime += DeltaTime;
	if (AccTime >= BaseTime)
	{
		AccTime = 0.f;
		Random_Push();
	}
}

void UWindPusher::Push(const FVector& Impulse)
{
	OwnerMesh->AddImpulse(Impulse, NAME_None, true);
}

void UWindPusher::Random_Push()
{
	FVector ImpulsePower = FVector(
		FMath::FRandRange(ImpulsePower_Min.X, ImpulsePower_Max.X),
		FMath::FRandRange(ImpulsePower_Min.Y, ImpulsePower_Max.Y),
		FMath::FRandRange(ImpulsePower_Min.Z, ImpulsePower_Max.Z)
	);

	OwnerMesh->AddImpulse(ImpulsePower, NAME_None, true);
}

