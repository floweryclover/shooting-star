// Copyright 2025 ShootingStar. All Rights Reserved.


#include "Knife.h"
#include "Components/SkeletalMeshComponent.h"
#include "TeamComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "EngineUtils.h"
#include "ShootingStar/ShootingStar.h"

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
    BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    SetActorEnableCollision(false); // 기본적으로 끄고 서버에서만 별도로 킴
    BodyMesh->SetGenerateOverlapEvents(false);

    StaticBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticBodyMesh")); // 곡괭이용 StaticMesh
    StaticBodyMesh->SetupAttachment(BodyMesh);
    StaticBodyMesh->SetIsReplicated(true);
    StaticBodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    AttackHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
    AttackHitBox->SetupAttachment(BodyMesh);
    AttackHitBox->SetCollisionProfileName("Knife");
    AttackHitBox->SetGenerateOverlapEvents(false);

    Sound = CreateDefaultSubobject<UAudioComponent>(TEXT("AUDIO"));
    Sound->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AKnife::BeginPlay()
{
	Super::BeginPlay();
    if (!HasAuthority())
    {
        return;
    }
    
    if (AttackHitBox)
    {
        AttackHitBox->SetCollisionProfileName("OverlapAllDynamic");
        AttackHitBox->SetGenerateOverlapEvents(false);
        AttackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AKnife::OnOverlapped_Nonvirtual);
    }
}

// Called every frame
void AKnife::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKnife::OnOverlapped_Nonvirtual(UPrimitiveComponent* OverlappedComp, 
        AActor* OtherActor, 
        UPrimitiveComponent* OtherComp, 
        int32 OtherBodyIndex, bool bFromSweep, 
        const FHitResult& SweepResult)
{
    this->OnOverlapBegin_Body(OverlappedComp, 
        OtherActor, 
        OtherComp, 
        OtherBodyIndex, bFromSweep, 
        SweepResult);
}

void AKnife::OnOverlapBegin_Body(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!HasAuthority()
        || !IsValid(GetOwner())
        || !IsValid(GetAttachParentActor())
        || !IsValid(OtherActor)
        || !IsValid(OtherActor->FindComponentByClass<UTeamComponent>()))
    {
        return;
    }
    
    UTeamComponent* const OtherTeamComponent = OtherActor->FindComponentByClass<UTeamComponent>();
    
    if (OtherActor == GetAttachParentActor() || !bDamageableFlag)
        return;
    
    if (OtherTeamComponent->GetTeam() != GetAttachParentActor()->FindComponentByClass<UTeamComponent>()->GetTeam())
    {
        // 디버그 표시
        DrawDebugSphere(GetWorld(), GetActorLocation(), 20.0f, 12, FColor::Red, false, 2.0f);

        // 데미지 적용
        UGameplayStatics::ApplyDamage(OtherActor, KnifeDamage, Cast<APlayerController>(GetOwner()), this, DamageTypeClass);
        UE_LOG(LogTemp, Warning, TEXT("Knife Hit: Player Hit"));
        
        bDamageableFlag = false;
    }
}