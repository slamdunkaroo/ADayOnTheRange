#include "Target.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "ShootingRangeGameMode.h"


ATarget::ATarget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

    PrimaryActorTick.bCanEverTick = false;

    // Main target mesh setup
    TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
    RootComponent = TargetMesh;

    TargetMesh->SetNotifyRigidBodyCollision(true);
    TargetMesh->SetGenerateOverlapEvents(true);
    TargetMesh->SetCollisionProfileName("BlockAllDynamic");
    TargetMesh->SetSimulatePhysics(true);

    TargetMesh->OnComponentHit.AddDynamic(this, &ATarget::OnHit);

    // Bullseye mesh setup
    BullseyeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BullseyeMesh"));
    BullseyeMesh->SetupAttachment(TargetMesh);

    BullseyeMesh->SetNotifyRigidBodyCollision(true);
    BullseyeMesh->SetGenerateOverlapEvents(true);
    BullseyeMesh->SetCollisionProfileName("BlockAllDynamic");

    BullseyeMesh->ComponentTags.Add(FName("Bullseye"));
}

void ATarget::BeginPlay()
{
    Super::BeginPlay();
    OriginalRotation = GetActorRotation();
    OriginalLocation = GetActorLocation();
    bIsFallen = false;
}

void ATarget::NotifyHit(
    UPrimitiveComponent* MyComp,
    AActor* Other,
    UPrimitiveComponent* OtherComp,
    bool bSelfMoved,
    FVector HitLocation,
    FVector HitNormal,
    FVector NormalImpulse,
    const FHitResult& Hit)
{
    Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
}

void ATarget::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (!bIsFallen)
    {
        AGameModeBase* GM = UGameplayStatics::GetGameMode(GetWorld());
        AShootingRangeGameMode* SRGM = Cast<AShootingRangeGameMode>(GM);
        if (SRGM)
        {
            SRGM->AddScore(10);
        }

        FallOver();
    }
}


void ATarget::FallOver()
{
    if (bIsFallen) return;

    TargetMesh->SetSimulatePhysics(true);
    TargetMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
    TargetMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

    FVector Torque = FVector(500000.f, 0.f, 0.f);
    TargetMesh->AddTorqueInDegrees(Torque, NAME_None, true);

    bIsFallen = true;

    GetWorldTimerManager().SetTimer(ResetTargetHandle, this, &ATarget::ResetTarget, 1.0f, false);

}

void ATarget::ResetTarget()
{
    TargetMesh->SetSimulatePhysics(false);
    TargetMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
    TargetMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

    SetActorLocation(OriginalLocation);
    SetActorRotation(OriginalRotation);

    bIsFallen = false;
}

