// Copyright 2025 ShootingStar. All Rights Reserved.


#include "ResourceActor.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ShootingStar/ShootingStar.h"

// Sets default values
AResourceActor::AResourceActor()
{
    PrimaryActorTick.bStartWithTickEnabled = true;
    PrimaryActorTick.bCanEverTick = true;
    
    bReplicates = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

    MeshComponent->SetCollisionResponseToChannel(CollisionChannels::ResourceActor, ECR_Block);
}

void AResourceActor::Tick(const float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    
    if (RemainingHitShakeTime <= 0.0f)
    {
        SetActorLocation(OriginLocation);
        return;
    }
    
    const float Alpha = (HitShakeTime - RemainingHitShakeTime) / HitShakeTime;  // 0 -> 1
    const float Scalar = HitShakeRadius * FMath::Sin(Alpha * 4 * PI) * (1.0f - Alpha);
    
    const FVector DeltaLocation = HitLocation - OriginLocation;
    const FVector DeltaLocationRotated = FVector{DeltaLocation.Y, -DeltaLocation.X, DeltaLocation.Z};
    
    SetActorLocation(OriginLocation + DeltaLocationRotated.GetSafeNormal2D() * Scalar);

    RemainingHitShakeTime -= DeltaSeconds;
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

void AResourceActor::PlayHitParticle()
{
    if (!HasAuthority()) return;  // 서버에서만 실행

    // 리소스 타입에 맞는 파티클 시스템 선택
    UNiagaraSystem* ParticleToPlay = ResourceHitEffects.Contains(ResourceData->ResourceType) 
        ? ResourceHitEffects[ResourceData->ResourceType]
        : HitParticleSystem;

    if (ParticleToPlay)
    {
        // 기존 파티클 시스템이 있다면 제거
        if (ActiveParticleSystem)
            ActiveParticleSystem->DestroyComponent();

        // 새 파티클 시스템 스폰
        ActiveParticleSystem = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            ParticleToPlay,
            OriginLocation,
            FRotator::ZeroRotator
        );
    }
}

void AResourceActor::Hit(const FVector& InHitLocation)
{
    RemainingHitShakeTime = HitShakeTime;
    HitLocation = InHitLocation;

    PlayHitParticle();
    UGameplayStatics::PlaySoundAtLocation(
        GetWorld(),
        HitSound,
        OriginLocation
    );
}

void AResourceActor::UpdateMesh_AfterHarvest()
{
    ResourceState = static_cast<EResourceState>(static_cast<int>(ResourceState) + 1);

    if (ResourceState >= EResourceState::End)
    {
        Destroy();
        return;
    }
    
    OnRep_ResourceState();
}

void AResourceActor::UpdateVisual()
{
    if (ResourceData)
    {
        switch (ResourceState)
        {
        case EResourceState::Large:
            MeshComponent->SetStaticMesh(ResourceData->LargeMesh ? ResourceData->LargeMesh : nullptr);
            break;

        case EResourceState::Medium:
            MeshComponent->SetStaticMesh(ResourceData->MediumMesh ? ResourceData->MediumMesh : nullptr);
            break;

        case EResourceState::Small:
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
            case EResourceState::Large:
                SetActorScale3D(LargeWoodScale);
                break;

            case EResourceState::Medium:
                SetActorScale3D(LargeWoodScale * 0.8f);
                break;

            case EResourceState::Small:
                SetActorScale3D(LargeWoodScale * 0.6f);
                break;
            }
        }
    }
}

void AResourceActor::BeginPlay()
{
    Super::BeginPlay();

    OriginLocation = GetActorLocation();
}

void AResourceActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AResourceActor, ResourceData);
    DOREPLIFETIME(AResourceActor, ResourceState);
    DOREPLIFETIME(AResourceActor, RemainingHitShakeTime);
    DOREPLIFETIME(AResourceActor, OriginLocation);
    DOREPLIFETIME(AResourceActor, HitLocation);
}

void AResourceActor::OnRep_ResourceData()
{
    UpdateVisual();
}

void AResourceActor::OnRep_ResourceState()
{
    UpdateVisual();
}
