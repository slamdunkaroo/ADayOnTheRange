#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SRSaveGame.generated.h"


UCLASS()
class ADAYONTHERANGE_API USRSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    USRSaveGame();

    UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
    int32 HighScore = 0;
};
