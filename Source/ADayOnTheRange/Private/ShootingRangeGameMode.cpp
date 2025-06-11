#include "ShootingRangeGameMode.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "SRSaveGame.h"

AShootingRangeGameMode::AShootingRangeGameMode()
{
    static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/UI/WBP_ScoreHUD"));
    if (WidgetClassFinder.Succeeded())
    {
        ScoreWidgetClass = WidgetClassFinder.Class;
    }
    if (ScoreText)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameMode] Initial ScoreText = %s"), *ScoreText->GetText().ToString());
    }

    CurrentScore = 0;
    HighScore = 0;
}

void AShootingRangeGameMode::BeginPlay()
{
    Super::BeginPlay();

    if (ScoreWidgetClass)
    {
        ScoreWidget = CreateWidget<UUserWidget>(GetWorld(), ScoreWidgetClass);

        if (ScoreWidget)
        {
            ScoreWidget->AddToViewport();

            ScoreText = Cast<UTextBlock>(ScoreWidget->GetWidgetFromName(TEXT("ScoreText")));
            HighScoreText = Cast<UTextBlock>(ScoreWidget->GetWidgetFromName(TEXT("HighScoreText")));

            if (ScoreText)
            {
                ScoreText->SetText(FText::AsNumber(CurrentScore));
            }

            if (HighScoreText)
            {
                HighScoreText->SetText(FText::AsNumber(HighScore));
            }
        }
    }
}

void AShootingRangeGameMode::AddScore(int32 Points)
{
    CurrentScore += Points;

    if (CurrentScore > HighScore)
    {
        HighScore = CurrentScore;
    }

    if (ScoreText)
    {
        ScoreText->SetText(FText::AsNumber(CurrentScore));
    }

    if (HighScoreText)
    {
        HighScoreText->SetText(FText::AsNumber(HighScore));
    }
}

