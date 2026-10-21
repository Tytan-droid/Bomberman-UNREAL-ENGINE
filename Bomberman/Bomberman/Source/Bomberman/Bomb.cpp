// Fill out your copyright notice in the Description page of Project Settings.


#include "Bomb.h"

#include "BombermanCharacter.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PowerUp.h"

// Sets default values
ABomb::ABomb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Création du composant sphère et définition comme racine (RootComponent)
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	RootComponent = CollisionComp;

}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();

	if (CollisionComp) 
	{
		// 1. Au spawn, la bombe laisse passer le joueur (en mode Overlap)
		CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
       
		// 2. On lance un timer pour la rendre solide
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABomb::EnableCollision, 2.67f, false);
	}
}

void ABomb::EnableCollision()
{
	if (CollisionComp)
	{
		CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		UE_LOG(LogTemp, Warning, TEXT("Bombe devenue solide !"));
	}
}


// Called every frame
void ABomb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABomb::Explode()
{
	
	float capsule_radius = 15.0f;
	FVector BombLocation = GetActorLocation();

	FVector Directions[] = {
		FVector::ForwardVector,
		FVector::RightVector,
		-FVector::ForwardVector,
		-FVector::RightVector
	};
	
	for (FVector Dir : Directions)
	{
		FVector EndLocation = BombLocation + (Dir*ExplosionRange);
		
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		
		FCollisionShape RayShape = FCollisionShape::MakeCapsule(capsule_radius, capsule_radius);
		
		bool bHit = GetWorld()->SweepSingleByChannel(
			 Hit,
			 BombLocation,
			 EndLocation,
			 FQuat::Identity, // Rotation de la capsule (pas de rotation particulière ici)
			  ECC_Pawn,
			 RayShape,
			 Params
		  );
		
		FVector ExplosionEnd = bHit ? Hit.Location : EndLocation;

		if (WeakWallExplosionEffect)
		{
			float Distance = FVector::Dist(BombLocation, ExplosionEnd);
			int32 NumSteps = FMath::Max(1, FMath::RoundToInt(Distance / 50.0f));

			for (int32 i = 1; i <= NumSteps; i++)
			{
				float Alpha = (float)i / (float)NumSteps;
				FVector StepLocation = FMath::Lerp(BombLocation, ExplosionEnd, Alpha);
             
				// On récupère le composant Niagara créé
				UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				   GetWorld(), 
				   WeakWallExplosionEffect, 
				   StepLocation, 
				   FRotator::ZeroRotator, 
				   FVector(1.0f)
				);

				// Si vous voulez le détruire explicitement au bout de 5 secondes :
				if (NiagaraComp)
				{
					FTimerHandle DespawnTimerHandle;
					GetWorld()->GetTimerManager().SetTimer(
					   DespawnTimerHandle, 
					   [NiagaraComp]()
					   {
						   if (IsValid(NiagaraComp))
						   {
							   NiagaraComp->DestroyComponent();
						   }
					   }, 
					   1.0f, 
					   false
					);
				}
			}
		}
               
		if (bHit && Hit.GetActor())
		{
			AActor* HitActor = Hit.GetActor();
			
			if (HitActor->ActorHasTag("Weak"))
			{
				FVector SpawnLocation = Hit.ImpactPoint + FVector(0.0f, 0.0f, 20.0f);
				
				TrySpawnPowerUp(HitActor->GetActorLocation());
				HitActor->Destroy();
			} 
			else if (ABombermanCharacter* Player = Cast<ABombermanCharacter>(HitActor)){
				Player->LoseLife();
			}
			else 
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit : %s, at %s"), *HitActor->GetName(), *HitActor->GetActorLocation().ToString());
			}
		}
	}
	Destroy();
}

void ABomb::TrySpawnPowerUp(const FVector& Location)
{
	const float Roll = FMath::FRand();   // [0, 1[

	TSubclassOf<APowerUp> ToSpawn = nullptr;

	if (Roll < ChanceA)
	{
		ToSpawn = PowerUpA;                  // tranche [0, ChanceA[
	}
	else if (Roll < ChanceA + ChanceB)
	{
		ToSpawn = PowerUpB;                  // tranche [ChanceA, ChanceA+ChanceB[
	}
	// sinon : aucun power-up

	if (ToSpawn)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<APowerUp>(ToSpawn, Location, FRotator::ZeroRotator, Params);
	}
}


