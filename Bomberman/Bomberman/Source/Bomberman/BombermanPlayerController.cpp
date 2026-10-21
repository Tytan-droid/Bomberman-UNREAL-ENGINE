#include "BombermanPlayerController.h"
#include "EnhancedInputSubsystems.h"

void ABombermanPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
}

void ABombermanPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}
