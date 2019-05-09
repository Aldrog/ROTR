// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ROTRGameMode.h"
#include "ROTRCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameHUD.h"

AROTRGameMode::AROTRGameMode()
{
    HUDClass = AGameHUD::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
