#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BombermanPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class BOMBERMAN_API ABombermanPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	virtual void OnPossess(APawn* InPawn) override;
	
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;
};