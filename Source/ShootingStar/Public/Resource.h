// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Resource.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class SHOOTINGSTAR_API UResource : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource")
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource")
    UStaticMesh* Mesh;  // 메시 정보만 보관

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource")
    UMaterialInterface* MaterialOverride;
};
