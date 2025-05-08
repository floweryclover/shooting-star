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

void AResourceActor::UpdateMesh_AfterHarvest()
{
    ResourceState = static_cast<RESOURCE_STATE>(static_cast<int>(ResourceState) + 1);

    if (ResourceState >= RESOURCE_STATE_END)
    {
        Destroy();
        return;
    }

    UpdateVisual();
}

void AResourceActor::UpdateVisual()
{
    if (ResourceData)
    {
        switch (ResourceState)
        {
        case RESOURCE_STATE_LARGE:
            MeshComponent->SetStaticMesh(ResourceData->LargeMesh ? ResourceData->LargeMesh : nullptr);
            break;

        case RESOURCE_STATE_MEDIUM:
            MeshComponent->SetStaticMesh(ResourceData->MediumMesh ? ResourceData->MediumMesh : nullptr);
            break;

        case RESOURCE_STATE_SMALL:
            MeshComponent->SetStaticMesh(ResourceData->SmallMesh ? ResourceData->SmallMesh : nullptr);
            break;
        }

        MeshComponent->SetMaterial(0, ResourceData->Material ? ResourceData->Material : nullptr);

        // Wood Scale Modify
        if (ResourceData->ResourceType == EResourceType::Wood)
        {
            FVector LargeWoodScale = { 1.3, 1.3, 1.2 };

            switch (ResourceState)
            {
            case RESOURCE_STATE_LARGE:
                SetActorScale3D(LargeWoodScale);
                break;

            case RESOURCE_STATE_MEDIUM:
                SetActorScale3D(LargeWoodScale * 0.8f);
                break;

            case RESOURCE_STATE_SMALL:
                SetActorScale3D(LargeWoodScale * 0.6f);
                break;
            }
        }
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