#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PowerUp.h"
#include "Bomb.h"
#include "BombermanCharacter.generated.h"

class UInputAction;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerPowerUpChanged, EPowerUpType, Type, bool, bActive);

UCLASS()
class BOMBERMAN_API ABombermanCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABombermanCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	int ID = -1;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DropBombAction;

	void Move(const FInputActionValue& Value);
	void DropBomb();

	// Couleur du joueur, lue sur le material au début du clignotement
	FLinearColor BaseColor = FLinearColor::White;
	bool bWasBlinking = false;

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLivesChanged, AController*, Owner, int32, NewLives);

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	int32 CurrentLives = 3;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float LastBomb = -5.0f;

	UPROPERTY(BlueprintAssignable)
	FOnLivesChanged OnLivesChanged;

	UFUNCTION(BlueprintCallable)
	void LoseLife();

	UFUNCTION(BlueprintCallable)
	void ActivatePowerUp(EPowerUpType Type, float Duration);

	UFUNCTION(BlueprintPure)
	float GetPowerUpRemainingTime(EPowerUpType Type) const;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerPowerUpChanged OnPowerUpChanged;

	UPROPERTY(BlueprintReadOnly)
	int32 BombRange = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<class ABomb> BombClass;

	virtual void Tick(float DeltaTime) override;
	
	// Son joué lors du hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit | Audio")
	class USoundBase* Hit1;

	// Son joué lors du hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit | Audio")
	class USoundBase* Hit2;

	// Son joué lors du hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit | Audio")
	class USoundBase* Hit3;

private:
	bool bInvincible = false;
	FTimerHandle InvincibilityTimer;
	void EndInvincibility() { bInvincible = false; }
	TMap<EPowerUpType, FTimerHandle> PowerUpTimers;

	void ApplyEffect(EPowerUpType Type);
	void RemoveEffect(EPowerUpType Type);
	void EndPowerUp(EPowerUpType Type);
};