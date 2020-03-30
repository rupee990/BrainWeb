// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ExtendEditorExampleGameMode.h"
#include "ExtendEditorExampleHUD.h"
#include "ExtendEditorExampleCharacter.h"
#include "UObject/ConstructorHelpers.h"

AExtendEditorExampleGameMode::AExtendEditorExampleGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AExtendEditorExampleHUD::StaticClass();
}
