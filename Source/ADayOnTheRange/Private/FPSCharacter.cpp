#include "FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "Components/TextBlock.h"
#include "SRSaveGame.h"

AFPSCharacter::AFPSCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
    FPSCamera->SetupAttachment(StaticCast<USceneComponent*>(GetRootComponent()));
    FPSCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
    FPSCamera->bUsePawnControlRotation = true;

    FPStaticArms = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FPStaticArms"));
    FPStaticArms->SetupAttachment(FPSCamera);
    FPStaticArms->SetRelativeLocation(FVector(0.0f, 0.0f, -20.0f));
    FPStaticArms->SetRelativeRotation(FRotator::ZeroRotator);

    SaveSlotName = TEXT("PlayerSaveSlot"); // Set default save slot name
}

void AFPSCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Load or create save game
    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
    {
        SaveGameInstance = Cast<USRSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
    }
    else
    {
        SaveGameInstance = Cast<USRSaveGame>(UGameplayStatics::CreateSaveGameObject(USRSaveGame::StaticClass()));
        UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, 0);
    }

    if (SaveGameInstance)
    {
        HighScore = SaveGameInstance->HighScore;
    }

    // Timer setup
    TimeRemaining = 30.0f;
    GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AFPSCharacter::UpdateGameTimer, 1.0f, true);

    // Reticle UI
    if (ReticleClass)
    {
        UUserWidget* Reticle = CreateWidget<UUserWidget>(GetWorld(), ReticleClass);
        if (Reticle) Reticle->AddToViewport();
    }

    // Score UI
    if (ScoreHUDClass)
    {
        ScoreHUDWidget = CreateWidget<UUserWidget>(GetWorld(), ScoreHUDClass);
        if (ScoreHUDWidget)
        {
            ScoreHUDWidget->AddToViewport();

            UTextBlock* FoundTimerText = Cast<UTextBlock>(ScoreHUDWidget->GetWidgetFromName(TEXT("TimerText")));
            UTextBlock* FoundAccuracyText = Cast<UTextBlock>(ScoreHUDWidget->GetWidgetFromName(TEXT("AccuracyText")));

            if (FoundTimerText)
            {
                TimerTextBlock = FoundTimerText;
                FString InitialTime = FString::Printf(TEXT("Time: %d"), FMath::RoundToInt(TimeRemaining));
                TimerTextBlock->SetText(FText::FromString(InitialTime));
            }

            if (FoundAccuracyText)
            {
                AccuracyText = FoundAccuracyText;
                AccuracyText->SetText(FText::FromString(TEXT("Accuracy: 0%")));
            }
        }
    }

    // Restore input mode
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        PC->SetInputMode(FInputModeGameOnly());
        PC->bShowMouseCursor = false;
    }
}

void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AFPSCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AFPSCharacter::LookUp);
    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::FireWeapon);
    PlayerInputComponent->BindAction("Pause", IE_Pressed, this, &AFPSCharacter::TogglePauseMenu);
}

void AFPSCharacter::MoveForward(float Value)
{
    AddMovementInput(GetActorForwardVector(), Value);
}

void AFPSCharacter::MoveRight(float Value)
{
    AddMovementInput(GetActorRightVector(), Value);
}

void AFPSCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void AFPSCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AFPSCharacter::FireWeapon()
{
    if (MuzzleFlash)
    {
        UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, FPStaticArms, NAME_None, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);
    }

    if (FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
    }

    if (ProjectileClass)
    {
        FVector MuzzleLocation = FPSCamera->GetComponentLocation() + FPSCamera->GetForwardVector() * 100.0f;
        FRotator MuzzleRotation = FPSCamera->GetComponentRotation();
        GetWorld()->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation);
    }

    ShotsFired++;
    UpdateAccuracyUI();
}

void AFPSCharacter::UpdateGameTimer()
{
    TimeRemaining -= 1.0f;

    if (TimerTextBlock)
    {
        FString TimeString = FString::Printf(TEXT("Time: %d"), FMath::Max(0, FMath::RoundToInt(TimeRemaining)));
        TimerTextBlock->SetText(FText::FromString(TimeString));
    }

    if (TimeRemaining <= 0.0f)
    {
        GetWorldTimerManager().ClearTimer(GameTimerHandle);
        TriggerGameOver();
    }
}

void AFPSCharacter::TriggerGameOver()
{
    if (CurrentScore > HighScore)
    {
        HighScore = CurrentScore;
    }

    if (SaveGameInstance)
    {
        SaveGameInstance->HighScore = HighScore;
        UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveSlotName, 0);
    }

    if (GameOverScreenClass)
    {
        GameOverScreen = CreateWidget<UUserWidget>(GetWorld(), GameOverScreenClass);
        if (GameOverScreen)
        {
            GameOverScreen->AddToViewport();

            UTextBlock* FinalAccuracyText = Cast<UTextBlock>(GameOverScreen->GetWidgetFromName(TEXT("FinalAccuracyText")));
            if (FinalAccuracyText)
            {
                float Accuracy = GetAccuracy();
                FString AccuracyStr = FString::Printf(TEXT("Accuracy: %.1f%%"), Accuracy);
                FinalAccuracyText->SetText(FText::FromString(AccuracyStr));
            }

            UTextBlock* FinalScoreText = Cast<UTextBlock>(GameOverScreen->GetWidgetFromName(TEXT("FinalScoreText")));
            if (FinalScoreText)
            {
                FString ScoreStr = FString::Printf(TEXT("Score: %d"), CurrentScore);
                FinalScoreText->SetText(FText::FromString(ScoreStr));
            }

            UTextBlock* HighScoreText = Cast<UTextBlock>(GameOverScreen->GetWidgetFromName(TEXT("HighScoreText")));
            if (HighScoreText)
            {
                FString HighScoreStr = FString::Printf(TEXT("High Score: %d"), HighScore);
                HighScoreText->SetText(FText::FromString(HighScoreStr));
            }

            if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
            {
                PC->SetShowMouseCursor(true);
                PC->SetInputMode(FInputModeUIOnly());
            }
        }
    }

    UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void AFPSCharacter::TogglePauseMenu()
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    if (!bIsPaused)
    {
        if (PauseMenuClass)
        {
            PauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuClass);
            if (PauseMenuWidget)
            {
                PauseMenuWidget->AddToViewport();
                PC->SetInputMode(FInputModeUIOnly());
                PC->bShowMouseCursor = true;
                UGameplayStatics::SetGamePaused(GetWorld(), true);
                bIsPaused = true;
            }
        }
    }
    else
    {
        if (PauseMenuWidget)
        {
            PauseMenuWidget->RemoveFromViewport();
            PauseMenuWidget = nullptr;
            PC->SetInputMode(FInputModeGameOnly());
            PC->bShowMouseCursor = false;
            UGameplayStatics::SetGamePaused(GetWorld(), false);
            bIsPaused = false;
        }
    }
}

float AFPSCharacter::GetTimeRemaining() const
{
    return TimeRemaining;
}

void AFPSCharacter::RegisterHit()
{
    Hits++;
    CurrentScore++;
    UE_LOG(LogTemp, Warning, TEXT("Hit registered! Hits: %d, Score: %d"), Hits, CurrentScore);
    UpdateAccuracyUI();
}

void AFPSCharacter::UpdateAccuracyUI()
{
    if (AccuracyText)
    {
        FString AccuracyString = FString::Printf(TEXT("Accuracy: %.1f%%"), GetAccuracy());
        AccuracyText->SetText(FText::FromString(AccuracyString));
    }
}
