// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUp.h"

#include "FXRenderingUtils.h"
#include "GameModeBomberman.h"
#include "Components/BoxComponent.h"
#include "BombermanCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APowerUp::APowerUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	PowerUpCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider"));
	RootComponent = PowerUpCollider;
	PowerUpCollider->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	PowerUpCollider->SetGenerateOverlapEvents(true);
	
	PowerUpMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	PowerUpMesh->SetupAttachment(RootComponent);
	PowerUpMesh->SetCollisionProfileName(TEXT("NoCollision"));

}

// Called when the game starts or when spawned
void APowerUp::BeginPlay()
{
	Super::BeginPlay();
	
	InitialZ = PowerUpMesh->GetRelativeLocation().Z;
	
	PowerUpCollider->OnComponentBeginOverlap.AddDynamic(this, &APowerUp::OnBeginOverlap);
}

// Called every frame
void APowerUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PowerUpMesh)
	{
		return;
	}

	// Rotate around the vertical axis
	FRotator Rotation = PowerUpMesh->GetRelativeRotation();
	Rotation.Yaw += RotationSpeed * DeltaTime;
	PowerUpMesh->SetRelativeRotation(Rotation);

	// Hover up and down
	RunningTime += DeltaTime;

	FVector Location = PowerUpMesh->GetRelativeLocation();
	Location.Z = InitialZ +
		FMath::Sin(RunningTime * HoverFrequency) * HoverAmplitude;

	PowerUpMesh->SetRelativeLocation(Location);
}

void APowerUp::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (ABombermanCharacter* Player = Cast<ABombermanCharacter>(OtherActor))
	{
		Player->ActivatePowerUp(Type, Duration);
		
		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(), 
				PickupSound, 
				GetActorLocation()
			);
		}
		Destroy();
	}else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 1.1f, FColor::Yellow,
				FString::Printf(TEXT("Not player")));
		}
	}
}

