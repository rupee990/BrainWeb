
#pragma once

#include "CoreMinimal.h"

class FAssetEditor_BrainWeb;
class FExtender;
class FToolBarBuilder;

class FAssetEditorToolbar_BrainWeb : public TSharedFromThis<FAssetEditorToolbar_BrainWeb>
{
public:
	FAssetEditorToolbar_BrainWeb(TSharedPtr<FAssetEditor_BrainWeb> InGenericGraphEditor)
		: GenericGraphEditor(InGenericGraphEditor) {}

	void AddGenericGraphToolbar(TSharedPtr<FExtender> Extender);

private:
	void FillGenericGraphToolbar(FToolBarBuilder& ToolbarBuilder);

protected:
	/** Pointer back to the blueprint editor tool that owns us */
	TWeakPtr<FAssetEditor_BrainWeb> GenericGraphEditor;
};
