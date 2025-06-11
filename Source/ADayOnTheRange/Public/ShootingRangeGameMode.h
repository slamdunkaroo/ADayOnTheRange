#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "SRSaveGame.h"
#include "ShootingRangeGameMode.generated.h"


UCLASS()
class ADAYONTHERANGE_API AShootingRangeGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AShootingRangeGameMode();

    virtual void BeginPlay() override;

    void AddScore(int32 Points);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
    TSubclassOf<class UUserWidget> ScoreWidgetClass;

    UPROPERTY()
    class UUserWidget* ScoreWidget;

    UPROPERTY()
    class UTextBlock* ScoreText;

    UPROPERTY()
    class UTextBlock* HighScoreText;

private:
    USRSaveGame* SaveGameInstance;
    int32 CurrentScore;
    int32 HighScore;
    FTimerHandle GameTimerHandle;
    int32 TimeRemaining = 30;

    void UpdateTimer();
    void EndGame();

};
