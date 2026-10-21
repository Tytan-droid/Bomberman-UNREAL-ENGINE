#include "BombermanCharacter.h"
#include "EnhancedInputComponent.h"
#include "GameModeBomberman.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

ABombermanCharacter::ABombermanCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(FName("Player"));

	UCharacterMovementComponent* CharMove = GetCharacterMovement();

	bUseControllerRotationYaw = false;
	CharMove->bOrientRotationToMovement = true;
	CharMove->RotationRate = FRotator(0.f, 720.f, 0.f);
}

void ABombermanCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABombermanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABombermanCharacter::Move);
		}
		if (DropBombAction)
		{
			EnhancedInputComp->BindAction(DropBombAction, ETriggerEvent::Started, this, &ABombermanCharacter::DropBomb);
		}
	}
}

void ABombermanCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddMovementInput(FVector::ForwardVector, MovementVector.Y);
		AddMovementInput(FVector::RightVector, MovementVector.X);
	}
}

void ABombermanCharacter::DropBomb()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastBomb < 5.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Timer pas encore initialise !"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("Bombe posée à la position : %s"), *GetActorLocation().ToString());

	if (BombClass)
	{
		FVector CurrentLoc = GetActorLocation();
		float SnappedX = FMath::FloorToFloat(CurrentLoc.X / 100.0f) * 100.0f + 50.0f;
		float SnappedY = FMath::FloorToFloat(CurrentLoc.Y / 100.0f) * 100.0f + 50.0f;
		FVector SpawnLocation = FVector(SnappedX, SnappedY, CurrentLoc.Z);

		FRotator SpawnRotation = GetActorRotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		// Evite que la bombe ne se coince dans le joueur au spawn
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ABomb* SpawnedBomb = GetWorld()->SpawnActor<ABomb>(BombClass, SpawnLocation, SpawnRotation, SpawnParams);

		if (SpawnedBomb && BombRange)
		{
			SpawnedBomb->ExplosionRange += BombRange * 100;
		}
		LastBomb = CurrentTime;
	}
}

void ABombermanCharacter::LoseLife()
{
	if (bInvincible || CurrentLives <= 0) return;

	UE_LOG(LogTemp, Warning, TEXT("TOUCHE BOMBE"));
	CurrentLives--;
	OnLivesChanged.Broadcast(GetController(), CurrentLives);
	if (Hit1 && Hit2 && Hit3)
	{
		// Choisit un nombre aléatoire entre 0 et 2
		int32 RandomChoice = FMath::RandRange(0, 2);
		USoundBase* SelectedSound = nullptr;

		switch (RandomChoice)
		{
		case 0: SelectedSound = Hit1; break;
		case 1: SelectedSound = Hit2; break;
		case 2: SelectedSound = Hit3; break;
		}

		if (SelectedSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				GetWorld(), 
				SelectedSound, 
				GetActorLocation()
			);
		}
	}
	if (CurrentLives <= 0)
	{
		if (AGameModeBomberman* GM = GetWorld()->GetAuthGameMode<AGameModeBomberman>())
		{
			GM->OnPlayerDied(this);
		}
		Destroy();
		return;
	}

	// Invincibilité courte pour éviter de perdre 3 vies sur la même explosion
	bInvincible = true;
	GetWorldTimerManager().SetTimer(InvincibilityTimer, this,
		&ABombermanCharacter::EndInvincibility, 1.5f, false);
}

void ABombermanCharacter::ActivatePowerUp(EPowerUpType Type, float Duration)
{
	FTimerManager& TM = GetWorldTimerManager();
	FTimerHandle& Handle = PowerUpTimers.FindOrAdd(Type);

	// On n'applique l'effet que s'il n'est pas déjà actif (sinon on prolonge juste la durée)
	if (!TM.IsTimerActive(Handle))
	{
		ApplyEffect(Type);
		OnPowerUpChanged.Broadcast(Type, true);
	}

	FTimerDelegate Delegate = FTimerDelegate::CreateUObject(this, &ABombermanCharacter::EndPowerUp, Type);
	TM.SetTimer(Handle, Delegate, Duration, false);
}

void ABombermanCharacter::EndPowerUp(EPowerUpType Type)
{
	RemoveEffect(Type);
	OnPowerUpChanged.Broadcast(Type, false);
}

void ABombermanCharacter::ApplyEffect(EPowerUpType Type)
{
	switch (Type)
	{
	case EPowerUpType::Invincibility:
		UE_LOG(LogTemp, Warning, TEXT("Invincibility"));
		bInvincible = true;
		break;
	case EPowerUpType::BombRange:
		UE_LOG(LogTemp, Warning, TEXT("BombRange"));
		BombRange += 1;
		break;
	}
}

void ABombermanCharacter::RemoveEffect(EPowerUpType Type)
{
	switch (Type)
	{
	case EPowerUpType::Invincibility:
		bInvincible = false;
		break;
	case EPowerUpType::BombRange:
		BombRange -= 1;
		break;
	}
}

float ABombermanCharacter::GetPowerUpRemainingTime(EPowerUpType Type) const
{
	const FTimerHandle* Handle = PowerUpTimers.Find(Type);
	return Handle ? GetWorldTimerManager().GetTimerRemaining(*Handle) : 0.f;
}

void ABombermanCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	Super::EndPlay(EndPlayReason);
}

void ABombermanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Rien à faire hors invincibilité (sauf pour restaurer la couleur à la fin)
	if (!bInvincible && !bWasBlinking) return;

	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	const int32 NumMats = MeshComp->GetNumMaterials();

	// Début du clignotement : on mémorise la couleur du joueur (posée par ScriptLevel)
	if (bInvincible && !bWasBlinking)
	{
		for (int32 i = 0; i < NumMats; ++i)
		{
			if (UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(MeshComp->GetMaterial(i)))
			{
				BaseColor = MID->K2_GetVectorParameterValue(FName("BaseColorFactor"));
				break;
			}
		}
	}

	FLinearColor NewColor = BaseColor;
	if (bInvincible)
	{
		const float Alpha = (FMath::Sin(GetWorld()->GetTimeSeconds() * 15.f) + 1.f) * 0.5f;
		NewColor = FMath::Lerp(BaseColor, FLinearColor::White, Alpha);
	}

	// On agit sur les instances actuellement présentes sur le mesh
	for (int32 i = 0; i < NumMats; ++i)
	{
		if (UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(MeshComp->GetMaterial(i)))
		{
			MID->SetVectorParameterValue(FName("BaseColorFactor"), NewColor);
		}
	}

	bWasBlinking = bInvincible;
}