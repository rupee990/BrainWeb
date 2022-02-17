#include "AssetEditorToolbar_BrainWeb.h"
#include "AssetEditor_BrainWeb.h"
//#include "EditorCommands_BrainWeb.h"
#include "Graph/Editor/Style/BrainWebEditorStyle.h"

#define LOCTEXT_NAMESPACE "AssetEditorToolbar_GenericGraph"

void FAssetEditorToolbar_BrainWeb::AddGenericGraphToolbar(TSharedPtr<FExtender> Extender)
{
	check(GenericGraphEditor.IsValid());
	TSharedPtr<FAssetEditor_BrainWeb> GenericGraphEditorPtr = GenericGraphEditor.Pin();

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, GenericGraphEditorPtr->GetToolkitCommands(), FToolBarExtensionDelegate::CreateSP( this, &FAssetEditorToolbar_BrainWeb::FillGenericGraphToolbar ));
	GenericGraphEditorPtr->AddToolbarExtender(ToolbarExtender);
}

void FAssetEditorToolbar_BrainWeb::FillGenericGraphToolbar(FToolBarBuilder& ToolbarBuilder)
{
	check(GenericGraphEditor.IsValid());
	TSharedPtr<FAssetEditor_BrainWeb> GenericGraphEditorPtr = GenericGraphEditor.Pin();

}


#undef LOCTEXT_NAMESPACE
