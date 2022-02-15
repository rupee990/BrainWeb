#include "Graph/Editor/Commands/AssetTypeActions_BrainWeb.h"
//#include "GenericGraphEditorPCH.h"
#include "EdGraphUtilities.h"
#include "Graph/Editor/Nodes/BaseNode/EdNode_BrainWebNode.h"
#include "Graph/Editor/Nodes/BaseNode/SEdNode_BrainWebNode.h"

#include "Graph/Editor/Nodes/EndNode/SEdNode_BrainWebNode_End.h"
#include "Graph/Editor/Style/BrainWebEditorStyle.h"
#include "IBrainWebEditor.h"
#include "Graph/Editor/Nodes/EndNode/EdNode_BrainWebNode_End.h"

DEFINE_LOG_CATEGORY(BrainWebEditor)

#define LOCTEXT_NAMESPACE "Editor_BrainWeb"

class FGraphPanelNodeFactory_BrainWeb : public FGraphPanelNodeFactory
{
    virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override
    {
        
        if(UEdNode_BrainWebNode_End* EdNode_GraphNodeEnd = Cast<UEdNode_BrainWebNode_End>(Node))
        {
            return SNew(SEdNode_BrainWebNode_End, EdNode_GraphNodeEnd);
        }
        
        if(UEdNode_BrainWebNode* BaseNode = Cast<UEdNode_BrainWebNode>(Node) )
        {
            return SNew(SEdNode_BrainWebNode, BaseNode);
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

