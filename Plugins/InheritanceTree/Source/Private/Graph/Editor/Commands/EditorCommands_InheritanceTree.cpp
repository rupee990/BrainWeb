#include "EditorCommands_InheritanceTree.h"

#define LOCTEXT_NAMESPACE "EditorCommands_GenericGraph"

void FEditorCommands_InheritanceTree::RegisterCommands()
{
	UI_COMMAND(GraphSettings, "Graph Settings", "Graph Settings", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
