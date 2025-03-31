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
	ImpulsePower.Z = FMath::FRandRange(300.f, 600.f);
	ImpulsePower.X = FMath::FRandRange(-600.f, -300.f);

	OwnerMesh->AddImpulse(ImpulsePower, NAME_None, true);
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

		OwnerMesh->AddImpulse(ImpulsePower, NAME_None, true);
	}
}

