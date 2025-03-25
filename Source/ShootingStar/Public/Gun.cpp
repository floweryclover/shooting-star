// Copyright 2025 ShootingStar. All Rights Reserved.


#include "Gun.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGun::PullTrigger()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn)
    {
        UE_LOG(LogTemp, Error, TEXT("OwnerPawn is nullptr!"));
        return;
    }

    AController* OwnerController = OwnerPawn->GetController();
    if (!OwnerController)
    {
        UE_LOG(LogTemp, Error, TEXT("OwnerController is nullptr!"));
        return;
    }

    FVector Location = OwnerPawn->GetActorLocation() + OwnerPawn->GetActorForwardVector() * 100.0f;
    FRotator Rotation = OwnerPawn->GetActorRotation();

    FVector End = Location + Rotation.Vector() * MaxRange;

    FHitResult Hit;
    bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1);

    if (bSuccess)
    {
        DrawDebugLine(GetWorld(), Location, End, FColor::Green, false, 2, 0, 2);
        DrawDebugPoint(GetWorld(), Hit.Location, 10, FColor::Red, false, 2.0f);

        FVector ShotDirection = -Rotation.Vector();
        AActor* HitActor = Hit.GetActor();

        if (HitActor)
        {
            if (!OwnerController)
            {
                UE_LOG(LogTemp, Error, TEXT("OwnerController is nullptr in TakeDamage!"));
                return;
            }

            FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
            HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
        }
    }
}

