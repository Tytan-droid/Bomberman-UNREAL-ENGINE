#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeBomberman.generated.h"

class ABombermanCharacter;
class UUserWidget; // Forward declaration

UCLASS()
class BOMBERMAN_API AGameModeBomberman : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameModeBomberman();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomberman Setup")
	int32 MaxPlayers = 4;

	// Allows picking your Game Over Widget Blueprint in the Editor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	void OnPlayerDied(ABombermanCharacter* DeadPlayer);

private:
	// Holds reference to spawned instance to prevent GC
	UPROPERTY()
	UUserWidget* GameOverWidgetInstance = nullptr;
};