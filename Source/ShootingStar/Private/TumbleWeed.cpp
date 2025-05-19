// Copyright 2025 ShootingStar. All Rights Reserved.


#include "TumbleWeed.h"
#include "Components/SphereComponent.h"

ATumbleWeed::ATumbleWeed()
{
	GetStaticMeshComponent()->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	SphereComponent->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	SphereComponent->SetupAttachment(RootComponent);
}
