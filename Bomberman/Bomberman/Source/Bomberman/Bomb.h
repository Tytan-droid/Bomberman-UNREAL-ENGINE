// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "NiagaraSystem.h"
#include "Bomb.generated.h"

class APowerUp;
UCLASS()
class BOMBERMAN_API ABomb : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABomb();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComp;
	
	UFUNCTION(BlueprintCallable, Category = "Bomb")
	void Explode();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb Settings")
	float ExplosionRange = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb Settings")
	class UNiagaraSystem* WeakWallExplosionEffect;
	

	UPROPERTY(EditDefaultsOnly, Category="PowerUp")
	TSubclassOf<APowerUp> PowerUpA;

	UPROPERTY(EditDefaultsOnly, Category="PowerUp", meta=(ClampMin="0.0", ClampMax="1.0"))
	float ChanceA = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category="PowerUp")
	TSubclassOf<APowerUp> PowerUpB;

	UPROPERTY(EditDefaultsOnly, Category="PowerUp", meta=(ClampMin="0.0", ClampMax="1.0"))
	float ChanceB = 0.25f;
	
	void EnableCollision();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void TrySpawnPowerUp(const FVector& Location);

};
