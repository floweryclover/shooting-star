// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ResourceType.h"
#include "ResourceDataAsset.generated.h"

UENUM()
enum class EResourceState : uint8
{
    Large,
    Medium,
    Small,
    End
};

UCLASS(BlueprintType)
class SHOOTINGSTAR_API UResourceDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EResourceType ResourceType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UStaticMesh* LargeMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UStaticMesh* MediumMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UStaticMesh* SmallMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UMaterialInterface* Material;
};