#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

// ────────────────────── Forward Declarations ──────────────────────
class UCameraComponent;
class UStaticMeshComponent;
class UUserWidget;
class UTextBlock;
class USRSaveGame;

UCLASS()
class ADAYONTHERANGE_API AFPSCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AFPSCharacter();

    float GetTimeRemaining() const;

    float GetAccuracy() const
    {
        return (ShotsFired == 0) ? 0.f : (static_cast<float>(Hits) / ShotsFired) * 100.f;
    }
    void RegisterHit();

    void UpdateAccuracyUI();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);

    void FireWeapon();
    void TogglePauseMenu();
    void UpdateGameTimer();
    void TriggerGameOver();

    UPROPERTY()
    USRSaveGame* SaveGameInstance = nullptr;

    FString SaveSlotName = TEXT("PlayerSaveSlot");

    UPROPERTY(VisibleAnywhere, Category = "Stats")
    int32 HighScore = 0;

    int32 ShotsFired = 0;
    int32 Hits = 0;
    int32 CurrentScore = 0;

    FTimerHandle GameTimerHandle;
    float TimeRemaining = 0.f;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* FPSCamera = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* FPStaticArms = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    TSubclassOf<class AProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, Category = "Combat")
    USoundBase* FireSound = nullptr;

    UPROPERTY(EditAnywhere, Category = "Combat")
    UParticleSystem* MuzzleFlash = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> ReticleClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> PauseMenuClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> GameOverScreenClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UUserWidget> ScoreHUDClass;

    UPROPERTY()
    UUserWidget* ScoreHUDWidget = nullptr;

    UPROPERTY()
    UTextBlock* TimerTextBlock = nullptr;

    UPROPERTY()
    UTextBlock* AccuracyText = nullptr;

    UPROPERTY()
    UUserWidget* PauseMenuWidget = nullptr;

    UPROPERTY()
    UUserWidget* GameOverScreen = nullptr;

    bool bIsPaused = false;
};
