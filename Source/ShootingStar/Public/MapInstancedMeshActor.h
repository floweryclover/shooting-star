// Copyright 2025 ShootingStar. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapInstancedMeshActor.generated.h"

class UMapGeneratorComponent;
class UInstancedStaticMeshComponent;

/**
* Fence 등 액터 생성이 아닌 형태로 클라이언트와 복제되어야 하는 InstancedMeshComponent를 관리하는 액터.
* 게임모드에 부착되는 컴포넌트는 클라이언트와 동기화할 수 없기 때문에 별도 액터로 구현됨.
*/
UCLASS()
class SHOOTINGSTAR_API AMapInstancedMeshActor final : public AActor
{
	GENERATED_BODY()

public:
	AMapInstancedMeshActor();

	virtual void BeginPlay() override;
	
	void Initialize(UMapGeneratorComponent* InOwner);

	/**
	 * AddFenceInstance(FTransform) 형태로 구현하면 매번 OnRep이 호출되고, 매번 인스턴스를 초기화하고 삭제되기 때문에,
	 * 한 번에 배열을 업데이트하여 한 번만 호출되도록 구현.
	 * @param Transforms
	 */
	UFUNCTION()
	void SetFenceInstances(const TArray<FTransform>& InFenceTransforms);

protected:
	UPROPERTY()
	UMapGeneratorComponent* OwnerMapGeneratorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInstancedStaticMeshComponent* FenceInstancedMeshComponent;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(ReplicatedUsing=OnRep_FenceTransforms)
	TArray<FTransform> FenceTransforms;

	UPROPERTY(ReplicatedUsing=OnRep_FenceMesh)
	UStaticMesh* FenceMesh;
	
	void RefreshFences();

	UFUNCTION()
	void OnRep_FenceTransforms();

	UFUNCTION()
	void OnRep_FenceMesh();
};
