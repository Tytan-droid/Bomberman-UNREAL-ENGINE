// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUp.generated.h"

UENUM(BlueprintType)
enum class EPowerUpType : uint8
{
	Invincibility,
	BombRange
};
UCLASS()
class BOMBERMAN_API APowerUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerUp();
	
	UPROPERTY(EditAnywhere, Category = "PowerUp|Animation")
	float RotationSpeed = 90.0f;

	UPROPERTY(EditAnywhere, Category = "PowerUp|Animation")
	float HoverAmplitude = 20.0f;

	UPROPERTY(EditAnywhere, Category = "PowerUp|Animation")
	float HoverFrequency = 2.0f;

	float InitialZ = 0.0f;
	float RunningTime = 0.0f;
	
public:
	// Son joué lors de la récupération du PowerUp
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUp | Audio")
	class USoundBase* PickupSound;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* PowerUpCollider;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* PowerUpMesh;
	

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PowerUp")
	EPowerUpType Type = EPowerUpType::Invincibility;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="PowerUp")
	float Duration = 3.f;
};
