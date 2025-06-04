#include "Target.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ATarget::ATarget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    UE_LOG(LogTemp, Warning, TEXT("ATarget Constructor called"));

    PrimaryActorTick.bCanEverTick = false;

    TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
    RootComponent = TargetMesh;

    TargetMesh->SetNotifyRigidBodyCollision(true);
    TargetMesh->SetGenerateOverlapEvents(true);
    TargetMesh->SetCollisionProfileName("BlockAllDynamic");
    TargetMesh->SetSimulatePhysics(true);

    TargetMesh->OnComponentHit.AddDynamic(this, &ATarget::OnHit);
}



void ATarget::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("BeginPlay: physics sim is %s"), TargetMesh->IsSimulatingPhysics() ? TEXT("ON") : TEXT("OFF"));
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
        UE_LOG(LogTemp, Warning, TEXT("Hit registered, falling target."));
        FallOver();
    }
}

void ATarget::FallOver()
{
    if (bIsFallen) return;

    TargetMesh->SetSimulatePhysics(true);
    TargetMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
    TargetMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

    // Apply torque to rotate on X-axis (like a hinged fall forward)
    FVector Torque = FVector(500000.f, 0.f, 0.f); // Tune this value as needed
    TargetMesh->AddTorqueInDegrees(Torque, NAME_None, true);

    bIsFallen = true;

    GetWorldTimerManager().SetTimer(ResetTargetHandle, this, &ATarget::ResetTarget, 1.0f, false);

    UE_LOG(LogTemp, Warning, TEXT("Target is falling with torque!"));
}


void ATarget::ResetTarget()
{
    TargetMesh->SetSimulatePhysics(false);
    TargetMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
    TargetMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

    SetActorLocation(OriginalLocation);
    SetActorRotation(OriginalRotation);

    bIsFallen = false;

    UE_LOG(LogTemp, Warning, TEXT("Target reset."));
}
