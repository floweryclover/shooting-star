// Copyright 2025 ShootingStar. All Rights Reserved.


#include "Gun.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "EngineUtils.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BODY"));
    RootComponent = BodyMesh;
    BodyMesh->SetIsReplicated(true);

    Sound = CreateDefaultSubobject<UAudioComponent>(TEXT("AUDIO"));
    Sound->SetupAttachment(RootComponent);

    WeaponeLever = WEAPONLEVER::SINGLEFIRE;
    WeaponeLeverCheck.Init(false, 3);

    static ConstructorHelpers::FObjectFinder<USoundWave> SoundWaveAsset(TEXT("SoundWave'/Game/Audios/SFX/WeaponSounds/Gunshot.Gunshot'"));
    if (SoundWaveAsset.Succeeded())
    {
        ShootSound = SoundWaveAsset.Object;
    }
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

UClass* AGun::GetStaticClass()
{
    return nullptr;
}

AGun* AGun::SpawnToHand(APawn* owner, FVector loc, FRotator rot)
{
    return nullptr;
}

bool AGun::ProjectileFire(FVector loc, FRotator rot, FRotator bulletRot)
{
    PlaySound();
	return true;
}
void AGun::PlayFireMontage()
{
}

void AGun::StopFireMontage()
{
}

void AGun::PlayReloadMontage()
{
}

void AGun::StopReloadMontage()
{
}

void AGun::PlaySound_Implementation()
{
	if (!GetIsReload()) {
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ShootSound,
			GetActorLocation()
		);
	}
}

//
// void AGun::PullTrigger()
// {
//     APlayerController* const OwnerController = Cast<APlayerController>(GetOwner());
//     if (!IsValid(OwnerController))
//     {
//         UE_LOG(LogTemp, Error, TEXT("OwnerController is nullptr!"));
//         return;
//     }
//     
//     APawn* const OwnerPawn = OwnerController->GetPawn();
//     if (!IsValid(OwnerPawn))
//     {
//         UE_LOG(LogTemp, Error, TEXT("OwnerPawn is nullptr!"));
//         return;
//     }
//
//     FVector Location = OwnerPawn->GetActorLocation() + OwnerPawn->GetActorForwardVector() * 100.0f;
//     FRotator Rotation = OwnerPawn->GetActorRotation();
//
//     FVector End = Location + Rotation.Vector() * MaxRange;
//
//     FHitResult Hit;
//     bool bSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1);
//
//     if (bSuccess)
//     {
//         DrawDebugLine(GetWorld(), Location, End, FColor::Green, false, 2, 0, 2);
//         DrawDebugPoint(GetWorld(), Hit.Location, 10, FColor::Red, false, 2.0f);
//
//         FVector ShotDirection = -Rotation.Vector();
//         AActor* HitActor = Hit.GetActor();
//
//         if (HitActor)
//         {
//             if (!OwnerController)
//             {
//                 UE_LOG(LogTemp, Error, TEXT("OwnerController is nullptr in TakeDamage!"));
//                 return;
//             }
//
//             FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
//             HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
//         }
//     }
// }
//
