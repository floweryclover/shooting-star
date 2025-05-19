// Copyright 2025 ShootingStar. All Rights Reserved.


#include "PickAxe.h"
#include "DrawDebugHelpers.h"

APickAxe::APickAxe()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    
    BodyMesh->SetVisibility(false);
    BodyMesh->SetHiddenInGame(true);
    BodyMesh->SetComponentTickEnabled(false);
    BodyMesh->Deactivate();

    AttackHitBox->SetupAttachment(StaticBodyMesh);
    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> STATICPICKAXE(TEXT("StaticMesh'/Game/lowpoly-mine-assets/source/Pickaxe.Pickaxe'"));
    if (STATICPICKAXE.Succeeded()) {
        StaticBodyMesh->SetStaticMesh(STATICPICKAXE.Object);
    }
}

// Called every frame
void APickAxe::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (AttackHitBox)
    {
        DrawDebugBox(
            GetWorld(),
            AttackHitBox->GetComponentLocation(),
            AttackHitBox->GetScaledBoxExtent(),
            FColor::Red,
            false,  // bPersistentLines
            -1.0f,  // LifeTime (0.0f or negative: one frame)
            0,      // DepthPriority
            2.0f    // Thickness
        );
    }
}
void APickAxe::OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnOverlapBegin_Body(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    if (!HasAuthority())
    {
        return;
    }
}

void APickAxe::BeginPlay()
{
    Super::BeginPlay();
    SetKnifeDamage(20.0f);
}
