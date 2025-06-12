#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ShootingRangeGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Target.h"
#include "FPSCharacter.h"

AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    RootComponent = CollisionComponent;
    CollisionComponent->InitSphereRadius(5.0f);
    CollisionComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));

    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMesh->SetupAttachment(CollisionComponent);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = CollisionComponent;
    ProjectileMovement->InitialSpeed = 2000.f;
    ProjectileMovement->MaxSpeed = 2000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();
    CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

void AProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this)
    {
        AGameModeBase* GM = UGameplayStatics::GetGameMode(GetWorld());
        if (GM)
        {
            UE_LOG(LogTemp, Warning, TEXT("GameMode class is: %s"), *GM->GetClass()->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("GameMode is NULL!"));
        }

        AShootingRangeGameMode* SRGM = Cast<AShootingRangeGameMode>(GM);
        if (SRGM)
        {
            SRGM->AddScore(1);
        }

        AFPSCharacter* PlayerChar = Cast<AFPSCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
        if (PlayerChar)
        {
            PlayerChar->RegisterHit();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to get FPSCharacter to register hit"));
        }

        Destroy();
    }
}
