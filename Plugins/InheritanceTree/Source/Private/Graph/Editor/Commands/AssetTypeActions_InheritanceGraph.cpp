#include "AssetTypeActions_InheritanceGraph.h"
//#include "GenericGraphEditorPCH.h"
#include "Graph/InheritanceTreeGraph.h"
#include "Graph/Editor/AssetEditor/AssetEditor_InheritanceTree.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_GenericGraph"

FAssetTypeActions_InheritanceTree::FAssetTypeActions_InheritanceTree(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{
}

FText FAssetTypeActions_InheritanceTree::GetName() const
{
	return LOCTEXT("FInheritanceTreeAssetTypeActionsName", "Inheritance Tree");
}

FColor FAssetTypeActions_InheritanceTree::GetTypeColor() const
{
	return FColor(129, 25, 115);
}

UClass* FAssetTypeActions_InheritanceTree::GetSupportedClass() const
{
	return UInheritanceTreeGraph::StaticClass();
}

void FAssetTypeActions_InheritanceTree::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UInheritanceTreeGraph* Graph = Cast<UInheritanceTreeGraph>(*ObjIt))
		{
			TSharedRef<FAssetEditor_InheritanceTree> NewGraphEditor(new FAssetEditor_InheritanceTree());
			NewGraphEditor->InitInheritanceTreeAssetEditor(Mode, EditWithinLevelEditor, Graph);
		}
	}
}

uint32 FAssetTypeActions_InheritanceTree::GetCategories()
{
	return EAssetTypeCategories::Animation | MyAssetCategory;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE