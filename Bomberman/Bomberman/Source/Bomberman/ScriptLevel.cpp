// Fill out your copyright notice in the Description page of Project Settings.


#include "ScriptLevel.h"

#include "BombermanCharacter.h"
#include "GameFramework/InputDeviceLibrary.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void AScriptLevel::BeginPlay()
{
	Super::BeginPlay();
	
	if (!ToSpawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("No character to spawn"));
		return;
	}
	
	TArray<FInputDeviceId> ConnectedDevices;
	UInputDeviceLibrary::GetAllConnectedInputDevices(ConnectedDevices);
	
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
	
	int const numDevice = FMath::Max(ConnectedDevices.Num() - 1, 1);
	int const numPlayers = FMath::Min(numDevice, DesiredPlayers);
	
	for (int32 i = 0; i < numPlayers; i++)
	{
		if (PlayerStarts.Num() <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("No player starts found"));
			return;
		}
		
		if (auto PlayerStart = Cast<APlayerStart>(PlayerStarts[i]))
		{
			APawn* Char = GetWorld()->SpawnActor<APawn>(ToSpawn, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
			
			if (ABombermanCharacter* Character = Cast<ABombermanCharacter>(Char))
			{
				Character->ID = i;

				FLinearColor PlayerColor;

				switch (i)
				{
				case 0:
					PlayerColor = FLinearColor::Red;
					break;
				case 1:
					PlayerColor = FLinearColor::Blue;
					break;
				case 2:
					PlayerColor = FLinearColor::Green;
					break;
				case 3:
					PlayerColor = FLinearColor::Yellow;
					break;
				default:
					PlayerColor = FLinearColor::White;
					break;
				}

				if (USkeletalMeshComponent* Mesh = Character->GetMesh())
				{
					const int32 NumMaterials = Mesh->GetNumMaterials();

					for (int32 MaterialIndex = 0; MaterialIndex < NumMaterials; ++MaterialIndex)
					{
						if (UMaterialInstanceDynamic* Material =
							Mesh->CreateAndSetMaterialInstanceDynamic(MaterialIndex))
						{
							Material->SetVectorParameterValue(
								FName("BaseColorFactor"),
								PlayerColor
							);
						}
					}
				}
			
				if (auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), i))
				{
					PlayerController->Possess(Char);
				}
				else
				{
					APlayerController* Controller = UGameplayStatics::CreatePlayer(GetWorld(), i);
					Controller->Possess(Char);
				}
			}
		
		}
	}
}
