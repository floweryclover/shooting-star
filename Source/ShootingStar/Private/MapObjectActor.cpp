// Copyright 2025 ShootingStar. All Rights Reserved.


#include "MapObjectActor.h"

#include "Net/UnrealNetwork.h"
#include "ShootingStar/ShootingStar.h"

void AMapObjectActor::BeginPlay()
{
	Super::BeginPlay();

	// 클라이언트 로드시 충돌을 제대로 반영하기 위한 함수 호출
	if (!HasAuthority())
	{
		OnRep_CollisionType();
		OnRep_CollisionProfileName();
	}
}

void AMapObjectActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMapObjectActor, CollisionType);
	DOREPLIFETIME(AMapObjectActor, CollisionProfileName);
}

void AMapObjectActor::SetCollisionType(const ECollisionEnabled::Type InCollisionType)
{
	if (!HasAuthority())
	{
		UE_LOG(LogShootingStar, Error, TEXT("AMapObjectActor::SetCollisionType()이 클라이언트에서 호출되었습니다."));
		return;
	}

	CollisionType = InCollisionType;
	OnRep_CollisionType();
}

void AMapObjectActor::SetCollisionProfileName(const FName& ProfileName)
{
	if (!HasAuthority())
	{
		UE_LOG(LogShootingStar, Error, TEXT("AMapObjectActor::SetCollisionType()이 클라이언트에서 호출되었습니다."));
		return;
	}

	CollisionProfileName = ProfileName;
	OnRep_CollisionProfileName();
}

void AMapObjectActor::OnRep_CollisionType()
{
	GetStaticMeshComponent()->SetCollisionEnabled(static_cast<ECollisionEnabled::Type>(CollisionType));
}

void AMapObjectActor::OnRep_CollisionProfileName()
{
	GetStaticMeshComponent()->SetCollisionProfileName(CollisionProfileName);
}

