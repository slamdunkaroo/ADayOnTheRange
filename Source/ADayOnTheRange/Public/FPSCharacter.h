#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

class UCameraComponent;
class UStaticMeshComponent;
class UUserWidget;

UCLASS()
class ADAYONTHERANGE_API AFPSCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    //float GetTimeRemaining() const;
    AFPSCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void FireWeapon();
    void TogglePauseMenu();
    //void UpdateGameTimer();
    void TriggerGameOver();
    FTimerHandle GameTimerHandle;
    float TimeRemaining;


    UPROPERTY(VisibleAnywhere)
    UCameraComponent* FPSCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* FPStaticArms;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TSubclassOf<class AProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, Category = "Combat")
    USoundBase* FireSound;

    UPROPERTY(EditAnywhere, Category = "Combat")
    UParticleSystem* MuzzleFlash;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> ReticleClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> PauseMenuClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> GameOverScreenClass;

    UUserWidget* PauseMenuWidget;
    UUserWidget* GameOverScreen;

    bool bIsPaused;

};
