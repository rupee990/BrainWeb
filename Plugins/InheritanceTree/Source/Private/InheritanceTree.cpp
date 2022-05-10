#include "Graph/Editor/Commands/AssetTypeActions_InheritanceGraph.h"
//#include "GenericGraphEditorPCH.h"
#include "EdGraphUtilities.h"
#include "Graph/Editor/Nodes/BaseNode/EdNode_InheritanceTreeNode.h"
#include "Graph/Editor/Nodes/BaseNode/SEdNode_InheritanceTreeNode.h"

#include "Graph/Editor/Style/InheritanceTreeEditorStyle.h"
#include "IInheritanceTreeEditor.h"

DEFINE_LOG_CATEGORY(InheritanceTreeEditor)

#define LOCTEXT_NAMESPACE "Editor_InheritanceTree"

class FGraphPanelNodeFactory_InheritanceTree : public FGraphPanelNodeFactory
{
    virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override
    {
        if(UEdNode_InheritanceTreeNode* BaseNode = Cast<UEdNode_InheritanceTreeNode>(Node) )
        {
            return SNew(SEdNode_InheritanceTreeNode, BaseNode);
        }  
        return nullptr;
    }
};

TSharedPtr<FGraphPanelNodeFactory> GraphPanelNodeFactory_GenericGraph;

class FInheritanceTreeEditor : public IInheritanceTreeEditor
{
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);

private:
    TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;

    EAssetTypeCategories::Type InheritanceTreeAssetCategoryBit;
};

IMPLEMENT_MODULE( FInheritanceTreeEditor, InheritanceTree )

void FInheritanceTreeEditor::StartupModule()
{
    FInheritanceTreeEditorStyle::Initialize();

    GraphPanelNodeFactory_GenericGraph = MakeShareable(new FGraphPanelNodeFactory_InheritanceTree());
    FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory_GenericGraph);

    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

    InheritanceTreeAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Inheritance Tree")), LOCTEXT("InheritanceTreeAssetCategory", "InheritanceTree"));

    RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_InheritanceTree(InheritanceTreeAssetCategoryBit)));
}


void FInheritanceTreeEditor::ShutdownModule()
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

    FInheritanceTreeEditorStyle::Shutdown();
}

void FInheritanceTreeEditor::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
    AssetTools.RegisterAssetTypeActions(Action);
    CreatedAssetTypeActions.Add(Action);
}

#undef LOCTEXT_NAMESPACE

