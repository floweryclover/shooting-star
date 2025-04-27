// Copyright 2025 ShootingStar. All Rights Reserved.


#include "PickAxe.h"

APickAxe::APickAxe()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    static ConstructorHelpers::FObjectFinder<USkeletalMesh>SKELETALPICKAXE(TEXT("SkeletalMesh'/Game/lowpoly-mine-assets/source/SKM_Pickaxe.SKM_Pickaxe'"));
    if (SKELETALPICKAXE.Succeeded()) {
        BodyMesh->SetSkeletalMesh(SKELETALPICKAXE.Object);
    }

}

// Called when the game starts or when spawned
void APickAxe::BeginPlay()
{
    Super::BeginPlay();

}

// Called every frame
void APickAxe::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void APickAxe::OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnOverlapBegin_Body(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
    if (!HasAuthority())
    {
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("ProjectileHit"));
}