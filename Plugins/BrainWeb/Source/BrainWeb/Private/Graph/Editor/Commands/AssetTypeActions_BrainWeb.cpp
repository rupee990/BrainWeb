#include "AssetTypeActions_BrainWeb.h"
//#include "GenericGraphEditorPCH.h"
#include "Graph/BrainWebGraph.h"
#include "Graph/Editor/AssetEditor/AssetEditor_BrainWeb.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_GenericGraph"

FAssetTypeActions_BrainWeb::FAssetTypeActions_BrainWeb(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{
}

FText FAssetTypeActions_BrainWeb::GetName() const
{
	return LOCTEXT("FBrainWebAssetTypeActionsName", "Brain Web");
}

FColor FAssetTypeActions_BrainWeb::GetTypeColor() const
{
	return FColor(129, 196, 115);
}

UClass* FAssetTypeActions_BrainWeb::GetSupportedClass() const
{
	return UBrainWebGraph::StaticClass();
}

void FAssetTypeActions_BrainWeb::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UBrainWebGraph* Graph = Cast<UBrainWebGraph>(*ObjIt))
		{
			TSharedRef<FAssetEditor_BrainWeb> NewGraphEditor(new FAssetEditor_BrainWeb());
			NewGraphEditor->InitBrainWebAssetEditor(Mode, EditWithinLevelEditor, Graph);
		}
	}
}

uint32 FAssetTypeActions_BrainWeb::GetCategories()
{
	return EAssetTypeCategories::Animation | MyAssetCategory;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE