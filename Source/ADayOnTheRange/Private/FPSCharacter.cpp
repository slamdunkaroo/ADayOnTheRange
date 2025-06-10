#include "FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Projectile.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

AFPSCharacter::AFPSCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
    FPSCamera->SetupAttachment(GetCapsuleComponent());
    FPSCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
    FPSCamera->bUsePawnControlRotation = true;

    FPStaticArms = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FPStaticArms"));
    FPStaticArms->SetupAttachment(FPSCamera);
    FPStaticArms->SetRelativeLocation(FVector(0.0f, 0.0f, -20.0f));
    FPStaticArms->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
}

void AFPSCharacter::BeginPlay()
{
    Super::BeginPlay();

    //TimeRemaining = 60.0f;
    //GetWorldTimerManager().SetTimer(GameTimerHandle, this, &AFPSCharacter::UpdateGameTimer, 1.0f, true);

    if (ReticleClass)
    {
        UUserWidget* Reticle = CreateWidget<UUserWidget>(GetWorld(), ReticleClass);
        if (Reticle)
        {
            Reticle->AddToViewport();
        }
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

        UWorld* World = GetWorld();
        if (World)
        {
            World->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation);
        }
    }
}

//void AFPSCharacter::UpdateGameTimer()
//{
    //TimeRemaining -= 1.0f;

    //if (TimeRemaining <= 0.0f)
    //{
        //GetWorldTimerManager().ClearTimer(GameTimerHandle);
        //TriggerGameOver();
   // }
//}


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
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
                PC->SetInputMode(InputMode);
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

            FInputModeGameOnly InputMode;
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = false;

            UGameplayStatics::SetGamePaused(GetWorld(), false);
            bIsPaused = false;
        }
    }
}

void AFPSCharacter::TriggerGameOver()
{
    if (GameOverScreenClass)
    {
        GameOverScreen = CreateWidget<UUserWidget>(GetWorld(), GameOverScreenClass);
        if (GameOverScreen)
        {
            GameOverScreen->AddToViewport();
            APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
            if (PC)
            {
                PC->SetShowMouseCursor(true);
                PC->SetInputMode(FInputModeUIOnly());
            }
        }
    }

    UGameplayStatics::SetGamePaused(GetWorld(), true);
}

//float AFPSCharacter::GetTimeRemaining() const
//{
    //return TimeRemaining;
//}




