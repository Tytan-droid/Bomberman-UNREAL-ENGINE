// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "ScriptLevel.generated.h"

/**
 * 
 */
UCLASS()
class BOMBERMAN_API AScriptLevel : public ALevelScriptActor
{
	GENERATED_BODY()
	
	virtual  void BeginPlay() override;
	
	UPROPERTY(EditAnywhere);
	int DesiredPlayers;
	
	UPROPERTY(EditAnywhere);
	TSubclassOf<APawn> ToSpawn;
	
};
