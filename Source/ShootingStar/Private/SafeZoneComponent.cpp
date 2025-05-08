#include "SafeZoneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CompetitiveGameMode.h"
#include "CompetitiveSystemComponent.h"

USafeZoneComponent::USafeZoneComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.0f;
}

void USafeZoneComponent::BeginPlay()
{
    Super::BeginPlay();
    InitializeComponents();
}

void USafeZoneComponent::InitializeComponents()
{
    // CompetitiveSystem 찾기
    ACompetitiveGameMode* GameMode = Cast<ACompetitiveGameMode>(UGameplayStatics::GetGameMode(this));
    if (GameMode)
        CompetitiveSystem = GameMode->GetCompetitiveSystemComponent();

    // Static Mesh Component 찾기
    AActor* Owner = GetOwner();
    if (Owner)
        OwnerMeshComponent = Cast<UStaticMeshComponent>(Owner->GetComponentByClass(UStaticMeshComponent::StaticClass()));

    if (CompetitiveSystem && OwnerMeshComponent)
    {
        CurrentRadius = InitialSafeRadius;
        SafeZoneScale = FVector::OneVector * InitialSafeRadius;
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("Safe Zone Compenent: Failed to find CompetitiveSystem or OwnerMeshComponent"));
}

void USafeZoneComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    UpdateSafeZoneScale();
}

void USafeZoneComponent::UpdateSafeZoneScale()
{
    if (!CompetitiveSystem || !OwnerMeshComponent)
        return;

    CurrentRadius = CalculateCurrentRadius();
    SafeZoneScale = FVector::OneVector * CurrentRadius;
    OwnerMeshComponent->SetWorldScale3D(SafeZoneScale);
}

float USafeZoneComponent::CalculateCurrentRadius() const
{
    if (!CompetitiveSystem)
        return InitialSafeRadius;

    const float Alpha = CompetitiveSystem->GetSafeZoneAlpha();
    return FMath::Lerp(InitialSafeRadius, FinalSafeRadius, Alpha);
}