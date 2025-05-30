// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "MapObjectActor.generated.h"

/**
 * MapGeneratorComponent가 PlaceObject할 때 생성하는 액터.
 */
UCLASS()
class AMapObjectActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	explicit AMapObjectActor();
	
	void SetTranslucent(bool bShouldBeTranslucent);

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UMaterialInterface* DefaultMaterial;

    UPROPERTY()
    UMaterialInterface* TranslucentMaterial;

    bool bIsTranslucent = false;
};
