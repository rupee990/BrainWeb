
#pragma once

#include "CoreMinimal.h"

class FAssetEditor_InheritanceTree;
class FExtender;
class FToolBarBuilder;

class FAssetEditorToolbar_InheritanceTree : public TSharedFromThis<FAssetEditorToolbar_InheritanceTree>
{
public:
	FAssetEditorToolbar_InheritanceTree(TSharedPtr<FAssetEditor_InheritanceTree> InGenericGraphEditor)
		: GenericGraphEditor(InGenericGraphEditor) {}

	void AddGenericGraphToolbar(TSharedPtr<FExtender> Extender);

private:
	void FillGenericGraphToolbar(FToolBarBuilder& ToolbarBuilder);

protected:
	/** Pointer back to the blueprint editor tool that owns us */
	TWeakPtr<FAssetEditor_InheritanceTree> GenericGraphEditor;
};
