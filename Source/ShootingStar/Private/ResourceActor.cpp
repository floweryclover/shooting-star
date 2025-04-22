// Copyright 2025 ShootingStar. All Rights Reserved.


#include "ResourceActor.h"
#include "Components/StaticMeshComponent.h"
#include "Resource.h" 
#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "ShootingStar/ShootingStar.h"

// Sets default values
AResourceActor::AResourceActor()
{
    bReplicates = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

    MeshComponent->SetCollisionResponseToChannel(CollisionChannels::ResourceActor, ECR_Block);
}

void AResourceActor::Harvest(UInventoryComponent* Inventory)
{
    // Server
    Inventory->AddResource(ResourceData);
}

void AResourceActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

    UpdateVisual();
}

#if WITH_EDITOR
void AResourceActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    UpdateVisual();
}

#endif

void AResourceActor::UpdateVisual()
{
    if (ResourceData)
    {
        MeshComponent->SetStaticMesh(ResourceData->Mesh ? ResourceData->Mesh : nullptr);
        MeshComponent->SetMaterial(0, ResourceData->Material ? ResourceData->Material : nullptr);
    }
}

void AResourceActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AResourceActor, ResourceData);
}

void AResourceActor::OnRep_ResourceData()
{
    UpdateVisual();
}