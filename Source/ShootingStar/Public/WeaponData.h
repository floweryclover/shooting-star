#pragma once

#include "CoreMinimal.h"
#include "ResourceType.h"
#include "WeaponData.generated.h"

// 무기 데이터 구조체
USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText WeaponModifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* WeaponIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<uint8> NeedResourceCounts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<uint8> UsedResourceCounts;

	FWeaponData()
	{
		NeedResourceCounts.Init(0, (uint8)EResourceType::End);
		UsedResourceCounts.Init(0, (uint8)EResourceType::End);
	}
};
