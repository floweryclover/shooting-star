// Copyright 2025 ShootingStar. All Rights Reserved.


#include "TumbleWeed.h"

#include "CompetitivePlayerCharacter.h"
#include "Components/SphereComponent.h"

ATumbleWeed::ATumbleWeed()
{
	GetStaticMeshComponent()->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	SphereComponent->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	SphereComponent->SetSphereRadius(80.0f);
	SphereComponent->SetupAttachment(RootComponent);
}

void ATumbleWeed::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &ATumbleWeed::OnActorBeginOverlapHandler);
	OnActorEndOverlap.AddDynamic(this, &ATumbleWeed::OnActorEndOverlapHandler);
}

void ATumbleWeed::OnActorBeginOverlapHandler(AActor* /*const OverlappedActor*/, AActor* const OtherActor)
{
	ACompetitivePlayerCharacter* const CompetitivePlayerCharacter = Cast<ACompetitivePlayerCharacter>(OtherActor);
	if (!IsValid(CompetitivePlayerCharacter))
	{
		return;
	}
	
	if (!OverlappingCharacters.Contains(CompetitivePlayerCharacter))
	{
		OverlappingCharacters.Add(CompetitivePlayerCharacter);
	}
	OverlappingCharacters.RemoveAll([](const auto Character){ return !IsValid(Character); });
	
	const bool bSameTeamIsHere = Algo::FindByPredicate(OverlappingCharacters, [&](ACompetitivePlayerCharacter* const Character)
	{
		return Character->IsSameTeamWithLocalPlayer();
	}) != nullptr;
	
	SetTranslucent(bSameTeamIsHere);
	
#pragma region Server
	if (!HasAuthority())
	{
		return;
	}
	OnTumbleWeedOverlapChanged.Broadcast(this, OverlappingCharacters);
#pragma endregion Server;
}

void ATumbleWeed::OnActorEndOverlapHandler(AActor* /*const OverlappedActor*/, AActor* const OtherActor)
{
	ACompetitivePlayerCharacter* const CompetitivePlayerCharacter = Cast<ACompetitivePlayerCharacter>(OtherActor);
	if (!IsValid(CompetitivePlayerCharacter))
	{
		return;
	}
	
	if (OverlappingCharacters.Contains(CompetitivePlayerCharacter))
	{
		OverlappingCharacters.Remove(CompetitivePlayerCharacter);
	}
	OverlappingCharacters.RemoveAll([](const auto Character){ return !IsValid(Character); });
	
	const bool bSameTeamIsHere = Algo::FindByPredicate(OverlappingCharacters, [&](ACompetitivePlayerCharacter* const Character)
	{
		return Character->IsSameTeamWithLocalPlayer();
	}) != nullptr;
	
	SetTranslucent(bSameTeamIsHere);
	
#pragma region Server
	if (!HasAuthority())
	{
		return;
	}

	OnTumbleWeedOverlapChanged.Broadcast(this, OverlappingCharacters);
#pragma endregion Server;
}
