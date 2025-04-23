// Copyright 2025 ShootingStar. All Rights Reserved.


#include "MapInstancedMeshActor.h"
#include "MapGeneratorComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

AMapInstancedMeshActor::AMapInstancedMeshActor()
{
	FenceInstancedMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMeshComponent"));
	RootComponent = FenceInstancedMeshComponent;
	FenceInstancedMeshComponent->SetVisibility(true);
	FenceInstancedMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AMapInstancedMeshActor::BeginPlay()
{
	Super::BeginPlay();

	RefreshFences();
}

void AMapInstancedMeshActor::Initialize(UMapGeneratorComponent* const InOwner)
{
	OwnerMapGeneratorComponent = InOwner;
	
	// Fence InstancedMeshComponent 초기화 및 생성
	if (OwnerMapGeneratorComponent->GetFenceMesh())
	{
		if (FenceInstancedMeshComponent)
		{
			FenceMesh = OwnerMapGeneratorComponent->GetFenceMesh();
		}
	}
}

void AMapInstancedMeshActor::SetFenceInstances(const TArray<FTransform>& InFenceTransforms)
{
	FenceTransforms.Append(InFenceTransforms);
	OnRep_FenceTransforms();
}

void AMapInstancedMeshActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMapInstancedMeshActor, FenceTransforms);
	DOREPLIFETIME(AMapInstancedMeshActor, FenceMesh);
}

void AMapInstancedMeshActor::OnRep_FenceTransforms()
{
	RefreshFences();
}

void AMapInstancedMeshActor::OnRep_FenceMesh()
{
	RefreshFences();
}

void AMapInstancedMeshActor::RefreshFences()
{
	FenceInstancedMeshComponent->ClearInstances();
	FenceInstancedMeshComponent->SetStaticMesh(FenceMesh);
	for (const FTransform& Transform : FenceTransforms)
	{
		FenceInstancedMeshComponent->AddInstance(Transform);
	}
	FenceInstancedMeshComponent->MarkRenderInstancesDirty();
}
