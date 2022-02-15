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

	/*ToolbarBuilder.BeginSection("Generic Graph");
	{
		ToolbarBuilder.AddToolBarButton(FEditorCommands_BrainWeb::Get().GraphSettings,
			NAME_None,
			LOCTEXT("GraphSettings_Label", "Graph Settings"),
			LOCTEXT("GraphSettings_ToolTip", "Show the Graph Settings"),
			FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.GameSettings"));
	}
	ToolbarBuilder.EndSection();

	ToolbarBuilder.BeginSection("Util");
	{
		ToolbarBuilder.AddToolBarButton(FEditorCommands_BrainWeb::Get().AutoArrange,
			NAME_None,
			LOCTEXT("AutoArrange_Label", "Auto Arrange"),
			LOCTEXT("AutoArrange_ToolTip", "Auto arrange nodes, not perfect, but still handy"),
			FSlateIcon(FBrainWebEditorStyle::GetStyleSetName(), "GenericGraphEditor.AutoArrange"));
	}
	ToolbarBuilder.EndSection();*/

}


#undef LOCTEXT_NAMESPACE
