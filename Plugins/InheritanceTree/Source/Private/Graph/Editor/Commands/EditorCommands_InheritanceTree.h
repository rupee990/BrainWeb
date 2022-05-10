#pragma once

#include "CoreMinimal.h"

class FEditorCommands_InheritanceTree : public TCommands<FEditorCommands_InheritanceTree>
{
public:
	/** Constructor */
	FEditorCommands_InheritanceTree()
		: TCommands<FEditorCommands_InheritanceTree>("GenericGraphEditor", NSLOCTEXT("Contexts", "GenericGraphEditor", "Generic Graph Editor"), NAME_None, FEditorStyle::GetStyleSetName())
	{
	}
	
	TSharedPtr<FUICommandInfo> GraphSettings;
	TSharedPtr<FUICommandInfo> AutoArrange;

	virtual void RegisterCommands() override;
};
