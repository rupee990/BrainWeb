#pragma once

#include "CoreMinimal.h"

class FEditorCommands_BrainWeb : public TCommands<FEditorCommands_BrainWeb>
{
public:
	/** Constructor */
	FEditorCommands_BrainWeb()
		: TCommands<FEditorCommands_BrainWeb>("GenericGraphEditor", NSLOCTEXT("Contexts", "GenericGraphEditor", "Generic Graph Editor"), NAME_None, FEditorStyle::GetStyleSetName())
	{
	}
	
	TSharedPtr<FUICommandInfo> GraphSettings;
	TSharedPtr<FUICommandInfo> AutoArrange;

	virtual void RegisterCommands() override;
};
