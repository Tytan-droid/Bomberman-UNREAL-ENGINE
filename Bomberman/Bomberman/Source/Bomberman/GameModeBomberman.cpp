#include "GameModeBomberman.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"
#include "BombermanPlayerController.h"
#include "BombermanCharacter.h"
#include "Blueprint/UserWidget.h" // Required for CreateWidget and AddToViewport

AGameModeBomberman::AGameModeBomberman()
{
	// Définition des classes par défaut
	PlayerControllerClass = ABombermanPlayerController::StaticClass();
}

void AGameModeBomberman::BeginPlay()
{
	Super::BeginPlay();
}

void AGameModeBomberman::OnPlayerDied(ABombermanCharacter* DeadPlayer)
{
	int32 AliveCount = 0;
	ABombermanCharacter* LastAlive = nullptr;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABombermanCharacter* Char = Cast<ABombermanCharacter>((*It)->GetPawn());
		if (Char && Char != DeadPlayer && Char->CurrentLives > 0)
		{
			AliveCount++;
			LastAlive = Char;
		}
	}

	if (AliveCount <= 1)
	{
		// Spawn and display the Game Over Widget if assigned
		if (GameOverWidgetClass && !GameOverWidgetInstance)
		{
			GameOverWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
			if (GameOverWidgetInstance)
			{
				GameOverWidgetInstance->AddToViewport();
				APlayerController* PC = GetWorld()->GetFirstPlayerController();
				PC->bShowMouseCursor = true;
				PC->SetInputMode(FInputModeUIOnly());

			}
		}
	}
}