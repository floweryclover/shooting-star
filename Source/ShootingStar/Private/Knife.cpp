// Copyright 2025 ShootingStar. All Rights Reserved.


#include "Knife.h"
#include "Components/SkeletalMeshComponent.h"
#include "TeamComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "EngineUtils.h"

// Sets default values
AKnife::AKnife()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BODY"));
    RootComponent = BodyMesh;
    static ConstructorHelpers::FObjectFinder<USkeletalMesh>SKELETALKNIFE(TEXT("SkeletalMesh'/Game/Toon_Soldiers_UE5/Meshes/Weapons/SKM_weapon_knife.SKM_weapon_knife'"));
    if (SKELETALKNIFE.Succeeded()) {
        BodyMesh->SetSkeletalMesh(SKELETALKNIFE.Object);
    }
    BodyMesh->SetIsReplicated(true);
    
    SetActorEnableCollision(false); // 기본적으로 끄고 서버에서만 별도로 킴
    BodyMesh->SetCollisionProfileName("Knife");
    BodyMesh->SetGenerateOverlapEvents(false);

    AttackHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
    AttackHitBox->SetupAttachment(BodyMesh);
    AttackHitBox->SetGenerateOverlapEvents(false);
    AttackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AKnife::OnOverlapBegin_Body);

    Sound = CreateDefaultSubobject<UAudioComponent>(TEXT("AUDIO"));
    Sound->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AKnife::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKnife::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKnife::OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor != GetAttachParentActor() && !bHasDamaged)
    {
            // 디버그 표시
            DrawDebugSphere(GetWorld(), GetActorLocation(), 20.0f, 12, FColor::Red, false, 2.0f);

            // 데미지 적용
            UGameplayStatics::ApplyDamage(OtherActor, knifeDamage, nullptr, GetOwner(), nullptr);
            UE_LOG(LogTemp, Warning, TEXT("Knife Hit: Player Hit"));

            // 데미지 플래그를 초기화하는 타이머 설정
            GetWorld()->GetTimerManager().SetTimer(ResetDamageFlagHandle, this, &AKnife::ResetDamageFlag, 1.f, false);
            bHasDamaged = true;
    }
}
void AKnife::ResetDamageFlag()
{
    bHasDamaged = false;
}