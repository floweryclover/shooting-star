// Copyright 2025 ShootingStar. All Rights Reserved.


#include "MapObjectActor.h"

AMapObjectActor::AMapObjectActor()
{
    static ConstructorHelpers::FObjectFinder<UMaterialInterface> TranslucentMaterialFinder(
    TEXT("/Game/Location_Western/Materials/M_Translucent"));
    ensure(TranslucentMaterialFinder.Succeeded());
    if (TranslucentMaterialFinder.Succeeded())
    {
        TranslucentMaterial = TranslucentMaterialFinder.Object;
    }
}

void AMapObjectActor::BeginPlay()
{
    Super::BeginPlay();
    
    // 초기 메터리얼 저장
    if (UStaticMeshComponent* MeshComp = GetStaticMeshComponent())
    {
        DefaultMaterial = MeshComp->GetMaterial(0);
    }
}

void AMapObjectActor::SetTranslucent(bool bShouldBeTranslucent)
{
    if (bIsTranslucent == bShouldBeTranslucent) return;
    
    if (UStaticMeshComponent* MeshComp = GetStaticMeshComponent())
    {
        if (!TranslucentMaterial)
        {
            UE_LOG(LogTemp, Error, TEXT("Translucent material is not loaded."));
        }
        
        MeshComp->SetMaterial(0, bShouldBeTranslucent ? TranslucentMaterial : DefaultMaterial);
        bIsTranslucent = bShouldBeTranslucent;
    }
}
