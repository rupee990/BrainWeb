#include "Graph/AssetTypeActions_BrainWeb.h"
//#include "GenericGraphEditorPCH.h"
#include "EdGraphUtilities.h"
#include "Graph/Editor/EdNode_BrainWebNode.h"
#include "Graph/Editor/EdNode_BrainWebEdge.h"
#include "Graph/Editor/SEdNode_BrainWebNode.h"
#include "Graph/Editor/SEdNode_BrainWebEdge.h"
#include "Graph/Editor/BrainWebEditorStyle.h"
#include "IBrainWebEditor.h"

DEFINE_LOG_CATEGORY(BrainWebEditor)

#define LOCTEXT_NAMESPACE "Editor_BrainWeb"

class FGraphPanelNodeFactory_BrainWeb : public FGraphPanelNodeFactory
{
    virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override
    {
        if (UEdNode_BrainWebNode* EdNode_GraphNode = Cast<UEdNode_BrainWebNode>(Node))
        {
            return SNew(SEdNode_BrainWebNode, EdNode_GraphNode);
        }
        else if (UEdNode_BrainWebEdge* EdNode_Edge = Cast<UEdNode_BrainWebEdge>(Node))
        {
            return SNew(SEdNode_BrainWebEdge, EdNode_Edge);
        }
        return nullptr;
    }
};

TSharedPtr<FGraphPanelNodeFactory> GraphPanelNodeFactory_GenericGraph;

class FBrainWebEditor : public IBrainWebEditor
{
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);

private:
    TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;

    EAssetTypeCategories::Type BrainWebAssetCategoryBit;
};

IMPLEMENT_MODULE( FBrainWebEditor, BrainWeb )

void FBrainWebEditor::StartupModule()
{
    FBrainWebEditorStyle::Initialize();

    GraphPanelNodeFactory_GenericGraph = MakeShareable(new FGraphPanelNodeFactory_BrainWeb());
    FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory_GenericGraph);

    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

    BrainWebAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Brain Web")), LOCTEXT("BrainWebAssetCategory", "BrainWeb"));

    RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_BrainWeb(BrainWebAssetCategoryBit)));
}


void FBrainWebEditor::ShutdownModule()
{
    // Unregister all the asset types that we registered
    if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
    {
        IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
        for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index)
        {
            AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index].ToSharedRef());
        }
    }

    if (GraphPanelNodeFactory_GenericGraph.IsValid())
    {
        FEdGraphUtilities::UnregisterVisualNodeFactory(GraphPanelNodeFactory_GenericGraph);
        GraphPanelNodeFactory_GenericGraph.Reset();
    }

    FBrainWebEditorStyle::Shutdown();
}

void FBrainWebEditor::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
    AssetTools.RegisterAssetTypeActions(Action);
    CreatedAssetTypeActions.Add(Action);
}

#undef LOCTEXT_NAMESPACE

