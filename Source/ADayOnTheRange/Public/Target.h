#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Target.generated.h"

class UStaticMeshComponent;
class USoundBase;
class UParticleSystem;

UCLASS()
class ADAYONTHERANGE_API ATarget : public AActor
{
    GENERATED_BODY()

public:
    ATarget(const FObjectInitializer& ObjectInitializer);


protected:
    virtual void BeginPlay() override;
    void FallOver();
    void ResetTarget();
    void Tick(float DeltaTime);
    FTimerHandle ResetTargetHandle;
    FRotator OriginalRotation;
    bool bIsFallen = false;
    FVector OriginalLocation;
    FVector FallDirection;

    virtual void NotifyHit(
        class UPrimitiveComponent* MyComp,
        AActor* Other,
        class UPrimitiveComponent* OtherComp,
        bool bSelfMoved,
        FVector HitLocation,
        FVector HitNormal,
        FVector NormalImpulse,
        const FHitResult& Hit
    ) override;


    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* TargetMesh;

    UPROPERTY(EditAnywhere, Category = "Effects")
    USoundBase* HitSound;

    UPROPERTY(EditAnywhere, Category = "Effects")
    UParticleSystem* HitEffect;

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* BullseyeMesh;

    // Movement parameters
    UPROPERTY(EditAnywhere, Category = "Movement")
    float MovementAmplitude = 100.f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float MovementSpeed = 2.f;

    FVector StartLocation;
    float RunningTime = 0.f;


};
