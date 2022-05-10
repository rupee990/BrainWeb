#include "AssetEditorToolbar_InheritanceTree.h"
#include "AssetEditor_InheritanceTree.h"
//#include "EditorCommands_InheritanceTree.h"
#include "Graph/Editor/Style/InheritanceTreeEditorStyle.h"

#define LOCTEXT_NAMESPACE "AssetEditorToolbar_GenericGraph"

void FAssetEditorToolbar_InheritanceTree::AddGenericGraphToolbar(TSharedPtr<FExtender> Extender)
{
	check(GenericGraphEditor.IsValid());
	TSharedPtr<FAssetEditor_InheritanceTree> GenericGraphEditorPtr = GenericGraphEditor.Pin();

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, GenericGraphEditorPtr->GetToolkitCommands(), FToolBarExtensionDelegate::CreateSP( this, &FAssetEditorToolbar_InheritanceTree::FillGenericGraphToolbar ));
	GenericGraphEditorPtr->AddToolbarExtender(ToolbarExtender);
}

void FAssetEditorToolbar_InheritanceTree::FillGenericGraphToolbar(FToolBarBuilder& ToolbarBuilder)
{
	check(GenericGraphEditor.IsValid());
	TSharedPtr<FAssetEditor_InheritanceTree> GenericGraphEditorPtr = GenericGraphEditor.Pin();

}


#undef LOCTEXT_NAMESPACE
