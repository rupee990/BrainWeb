#include "EditorCommands_GenericGraph.h"

#define LOCTEXT_NAMESPACE "EditorCommands_GenericGraph"

void FEditorCommands_BrainWeb::RegisterCommands()
{
	UI_COMMAND(GraphSettings, "Graph Settings", "Graph Settings", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
