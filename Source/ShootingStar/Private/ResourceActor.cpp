// Copyright 2025 ShootingStar. All Rights Reserved.


#include "ResourceActor.h"
#include "Components/StaticMeshComponent.h"
#include "Resource.h" 

// Sets default values
AResourceActor::AResourceActor()
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

void AResourceActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

    UpdateVisual();
}

void AResourceActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    UpdateVisual();
}

void AResourceActor::UpdateVisual()
{
    if (ResourceData)
    {
        MeshComponent->SetStaticMesh(ResourceData->Mesh ? ResourceData->Mesh : nullptr);
        MeshComponent->SetMaterial(0, ResourceData->Material ? ResourceData->Material : nullptr);
    }
}
