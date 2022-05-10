#include "AssetEditor_InheritanceTree.h"
//#include "GenericGraphEditorPCH.h"
#include "AssetEditorToolbar_InheritanceTree.h"
#include "AssetGraphSchema_InheritanceTree.h"
//#include "EditorCommands_InheritanceTree.h"

#include "AssetToolsModule.h"
#include "HAL/PlatformApplicationMisc.h"

#include "GraphEditorActions.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraphUtilities.h"
#include "Graph/Editor/Graph/EdGraph_InheritanceTree.h"
#include "Graph/Editor/Nodes/BaseNode/EdNode_InheritanceTreeNode.h"
#include "GraphEditor.h"
#include "ScopedTransaction.h"
#include "Framework/Commands/GenericCommands.h"
#define LOCTEXT_NAMESPACE "AssetEditor_InheritanceTree"

const FName GenericGraphEditorAppName = FName(TEXT("InheritanceTreeEditorApp"));

struct FInheritanceTreeAssetEditorTabs
{
	// Tab identifiers
	static const FName InheritanceTreePropertyID;
	static const FName ViewportID;
	static const FName InheritanceTreeEditorSettingsID;
};

//////////////////////////////////////////////////////////////////////////

const FName FInheritanceTreeAssetEditorTabs::InheritanceTreePropertyID(TEXT("InheritanceTreeProperty"));
const FName FInheritanceTreeAssetEditorTabs::ViewportID(TEXT("Viewport"));
const FName FInheritanceTreeAssetEditorTabs::InheritanceTreeEditorSettingsID(TEXT("InheritanceTreeEditorSettings"));

//////////////////////////////////////////////////////////////////////////

FAssetEditor_InheritanceTree::FAssetEditor_InheritanceTree()
{
	EditingGraph = nullptr;

	GenricGraphEditorSettings = NewObject<UInheritanceTreeEditorSettings>(UInheritanceTreeEditorSettings::StaticClass());

	OnPackageSavedDelegateHandle = UPackage::PackageSavedEvent.AddRaw(this, &FAssetEditor_InheritanceTree::OnPackageSaved);
}

FAssetEditor_InheritanceTree::~FAssetEditor_InheritanceTree()
{
	UPackage::PackageSavedEvent.Remove(OnPackageSavedDelegateHandle);
}

void FAssetEditor_InheritanceTree::InitInheritanceTreeAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UInheritanceTreeGraph* Graph)
{
	EditingGraph = Graph;
	
	CreateEdGraph();

	FGenericCommands::Register();
	FGraphEditorCommands::Register();
	//EditorCommands_InheritanceTree::Register();

	if (!ToolbarBuilder.IsValid())
	{
		ToolbarBuilder = MakeShareable(new FAssetEditorToolbar_InheritanceTree(SharedThis(this)));
	}

	BindCommands();

	CreateInternalWidgets();

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

	ToolbarBuilder->AddGenericGraphToolbar(ToolbarExtender);
	
	// Layout
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_InheritanceTreeEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)->SetHideTabWell(true)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.65f)
					->AddTab(FInheritanceTreeAssetEditorTabs::ViewportID, ETabState::OpenedTab)->SetHideTabWell(true)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.7f)
						->AddTab(FInheritanceTreeAssetEditorTabs::InheritanceTreePropertyID, ETabState::OpenedTab)->SetHideTabWell(true)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.3f)
						->AddTab(FInheritanceTreeAssetEditorTabs::InheritanceTreeEditorSettingsID, ETabState::OpenedTab)
					)
				)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, GenericGraphEditorAppName, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, EditingGraph, false);

	RegenerateMenusAndToolbars();
}

void FAssetEditor_InheritanceTree::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_InheritanceTreeEditor", "Brain Web Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FInheritanceTreeAssetEditorTabs::ViewportID, FOnSpawnTab::CreateSP(this, &FAssetEditor_InheritanceTree::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("GraphCanvasTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(FInheritanceTreeAssetEditorTabs::InheritanceTreePropertyID, FOnSpawnTab::CreateSP(this, &FAssetEditor_InheritanceTree::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("DetailsTab", "Property"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FInheritanceTreeAssetEditorTabs::InheritanceTreeEditorSettingsID, FOnSpawnTab::CreateSP(this, &FAssetEditor_InheritanceTree::SpawnTab_EditorSettings))
		.SetDisplayName(LOCTEXT("EditorSettingsTab", "Brain Web Editor Setttings"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FAssetEditor_InheritanceTree::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(FInheritanceTreeAssetEditorTabs::ViewportID);
	InTabManager->UnregisterTabSpawner(FInheritanceTreeAssetEditorTabs::InheritanceTreePropertyID);
	InTabManager->UnregisterTabSpawner(FInheritanceTreeAssetEditorTabs::InheritanceTreeEditorSettingsID);
}

FName FAssetEditor_InheritanceTree::GetToolkitFName() const
{
	return FName("FInheritanceTreeEditor");
}

FText FAssetEditor_InheritanceTree::GetBaseToolkitName() const
{
	return LOCTEXT("GenericGraphEditorAppLabel", "Brain Web Editor");
}

FText FAssetEditor_InheritanceTree::GetToolkitName() const
{
	const bool bDirtyState = EditingGraph->GetOutermost()->IsDirty();

	FFormatNamedArguments Args;
	Args.Add(TEXT("InheritanceTreeName"), FText::FromString(EditingGraph->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(LOCTEXT("InheritanceTreeEditorToolkitName", "{InheritanceTreeName}{DirtyState}"), Args);
}

FText FAssetEditor_InheritanceTree::GetToolkitToolTipText() const
{
	return FAssetEditorToolkit::GetToolTipTextForObject(EditingGraph);
}

FLinearColor FAssetEditor_InheritanceTree::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FString FAssetEditor_InheritanceTree::GetWorldCentricTabPrefix() const
{
	return TEXT("MindWebEditor");
}

FString FAssetEditor_InheritanceTree::GetDocumentationLink() const
{
	return TEXT("");
}

void FAssetEditor_InheritanceTree::SaveAsset_Execute()
{
	if (EditingGraph != nullptr)
	{
		RebuildGenericGraph();
	}

	FAssetEditorToolkit::SaveAsset_Execute();
}

void FAssetEditor_InheritanceTree::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditingGraph);
	Collector.AddReferencedObject(EditingGraph->EdGraph);
}

UInheritanceTreeEditorSettings* FAssetEditor_InheritanceTree::GetSettings() const
{
	return GenricGraphEditorSettings;
}

TSharedRef<SDockTab> FAssetEditor_InheritanceTree::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FInheritanceTreeAssetEditorTabs::ViewportID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("ViewportTab_Title", "Viewport"));

	if (ViewportWidget.IsValid())
	{
		SpawnedTab->SetContent(ViewportWidget.ToSharedRef());
	}

	return SpawnedTab;
}

TSharedRef<SDockTab> FAssetEditor_InheritanceTree::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FInheritanceTreeAssetEditorTabs::InheritanceTreePropertyID);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("Details_Title", "Property"))
		[
			PropertyWidget.ToSharedRef()
		];
}

TSharedRef<SDockTab> FAssetEditor_InheritanceTree::SpawnTab_EditorSettings(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FInheritanceTreeAssetEditorTabs::InheritanceTreeEditorSettingsID);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("EditorSettings_Title", "Brain Web Editor Setttings"))
		[
			EditorSettingsWidget.ToSharedRef()
		];
}

void FAssetEditor_InheritanceTree::CreateInternalWidgets()
{
	ViewportWidget = CreateViewportWidget();

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.NotifyHook = this;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyWidget = PropertyModule.CreateDetailView(Args);
	PropertyWidget->SetObject(EditingGraph);
	PropertyWidget->OnFinishedChangingProperties().AddSP(this, &FAssetEditor_InheritanceTree::OnFinishedChangingProperties);

	EditorSettingsWidget = PropertyModule.CreateDetailView(Args);
	EditorSettingsWidget->SetObject(GenricGraphEditorSettings);
}

TSharedRef<SGraphEditor> FAssetEditor_InheritanceTree::CreateViewportWidget()
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_GenericGraph", "Web Graph");

	CreateCommandList();

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FAssetEditor_InheritanceTree::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FAssetEditor_InheritanceTree::OnNodeDoubleClicked);

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(EditingGraph->EdGraph)
		.GraphEvents(InEvents)
		.AutoExpandActionMenu(true)
		.ShowGraphStateOverlay(false);
}

void FAssetEditor_InheritanceTree::BindCommands()
{
	
}

#pragma optimize("", off)
void FAssetEditor_InheritanceTree::CreateEdGraph()
{
	if (EditingGraph->EdGraph == nullptr)
	{
		EditingGraph->EdGraph = CastChecked<UEdGraph_InheritanceTree>(FBlueprintEditorUtils::CreateNewGraph(EditingGraph, NAME_None, UEdGraph_InheritanceTree::StaticClass(), UAssetGraphSchema_InheritanceTree::StaticClass()));
		EditingGraph->EdGraph->bAllowDeletion = false;

		// Give the schema a chance to fill out any required nodes (like the results node)
		const UEdGraphSchema* Schema = EditingGraph->EdGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*EditingGraph->EdGraph);
	}
}
#pragma optimize("", on)

void FAssetEditor_InheritanceTree::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}

	GraphEditorCommands = MakeShareable(new FUICommandList);

	// Can't use CreateSP here because derived editor are already implementing TSharedFromThis<FAssetEditorToolkit>
	// however it should be safe, since commands are being used only within this editor
	// if it ever crashes, this function will have to go away and be reimplemented in each derived class
	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FAssetEditor_InheritanceTree::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_InheritanceTree::CanSelectAllNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FAssetEditor_InheritanceTree::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_InheritanceTree::CanDeleteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FAssetEditor_InheritanceTree::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_InheritanceTree::CanCopyNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FAssetEditor_InheritanceTree::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_InheritanceTree::CanCutNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FAssetEditor_InheritanceTree::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_InheritanceTree::CanPasteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FAssetEditor_InheritanceTree::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_InheritanceTree::CanDuplicateNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Rename,
		FExecuteAction::CreateSP(this, &FAssetEditor_InheritanceTree::OnRenameNode),
		FCanExecuteAction::CreateSP(this, &FAssetEditor_InheritanceTree::CanRenameNodes)
	);
}

TSharedPtr<SGraphEditor> FAssetEditor_InheritanceTree::GetCurrGraphEditor() const
{
	return ViewportWidget;
}

FGraphPanelSelectionSet FAssetEditor_InheritanceTree::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	TSharedPtr<SGraphEditor> FocusedGraphEd = GetCurrGraphEditor();
	if (FocusedGraphEd.IsValid())
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}

	return CurrentSelection;
}

void FAssetEditor_InheritanceTree::RebuildGenericGraph()
{

	UEdGraph_InheritanceTree* EdGraph = Cast<UEdGraph_InheritanceTree>(EditingGraph->EdGraph);
	check(EdGraph != nullptr);

	EdGraph->RebuildGenericGraph();
}

void FAssetEditor_InheritanceTree::SelectAllNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (CurrentGraphEditor.IsValid())
	{
		CurrentGraphEditor->SelectAllNodes();
	}
}

bool FAssetEditor_InheritanceTree::CanSelectAllNodes()
{
	return true;
}

void FAssetEditor_InheritanceTree::DeleteSelectedNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());

	CurrentGraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* EdNode = Cast<UEdGraphNode>(*NodeIt);
		if (EdNode == nullptr || !EdNode->CanUserDeleteNode())
			continue;;

		if (UEdNode_InheritanceTreeNode* EdNode_Node = Cast<UEdNode_InheritanceTreeNode>(EdNode))
		{
			EdNode_Node->Modify();

			const UEdGraphSchema* Schema = EdNode_Node->GetSchema();
			if (Schema != nullptr)
			{
				Schema->BreakNodeLinks(*EdNode_Node);
			}

			EdNode_Node->DestroyNode();
		}
		else
		{
			EdNode->Modify();
			EdNode->DestroyNode();
		}
	}
}

bool FAssetEditor_InheritanceTree::CanDeleteNodes()
{
	// If any of the nodes can be deleted then we should allow deleting
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node != nullptr && Node->CanUserDeleteNode())
		{
			return true;
		}
	}

	return false;
}

void FAssetEditor_InheritanceTree::DeleteSelectedDuplicatableNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FGraphPanelSelectionSet OldSelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}

	// Delete the duplicated nodes
	DeleteSelectedNodes();

	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}
}

void FAssetEditor_InheritanceTree::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}

bool FAssetEditor_InheritanceTree::CanCutNodes()
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FAssetEditor_InheritanceTree::CopySelectedNodes()
{
	// Export the selected nodes and place the text on the clipboard
	FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	FString ExportedText;

	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node == nullptr)
		{
			SelectedIter.RemoveCurrent();
			continue;
		}

		Node->PrepareForCopying();
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool FAssetEditor_InheritanceTree::CanCopyNodes()
{
	// If any of the nodes can be duplicated then we should allow copying
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			return true;
		}
	}

	return false;
}

void FAssetEditor_InheritanceTree::PasteNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (CurrentGraphEditor.IsValid())
	{
		PasteNodesHere(CurrentGraphEditor->GetPasteLocation());
	}
}

void FAssetEditor_InheritanceTree::PasteNodesHere(const FVector2D& Location)
{
	// Find the graph editor with focus
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}
	// Select the newly pasted stuff
	UEdGraph* EdGraph = CurrentGraphEditor->GetCurrentGraph();

	{
		const FScopedTransaction Transaction(FGenericCommands::Get().Paste->GetDescription());
		EdGraph->Modify();

		// Clear the selection set (newly pasted stuff will be selected)
		CurrentGraphEditor->ClearSelectionSet();

		// Grab the text to paste from the clipboard.
		FString TextToImport;
		FPlatformApplicationMisc::ClipboardPaste(TextToImport);

		// Import the nodes
		TSet<UEdGraphNode*> PastedNodes;
		FEdGraphUtilities::ImportNodesFromText(EdGraph, TextToImport, PastedNodes);

		//Average position of nodes so we can move them while still maintaining relative distances to each other
		FVector2D AvgNodePosition(0.0f, 0.0f);

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UEdGraphNode* Node = *It;
			AvgNodePosition.X += Node->NodePosX;
			AvgNodePosition.Y += Node->NodePosY;
		}

		float InvNumNodes = 1.0f / float(PastedNodes.Num());
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UEdGraphNode* Node = *It;
			CurrentGraphEditor->SetNodeSelection(Node, true);

			Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
			Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

			Node->SnapToGrid(16);

			// Give new node a different Guid from the old one
			Node->CreateNewGuid();
		}
	}

	// Update UI
	CurrentGraphEditor->NotifyGraphChanged();

	UObject* GraphOwner = EdGraph->GetOuter();
	if (GraphOwner)
	{
		GraphOwner->PostEditChange();
		GraphOwner->MarkPackageDirty();
	}
}

bool FAssetEditor_InheritanceTree::CanPasteNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return false;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(CurrentGraphEditor->GetCurrentGraph(), ClipboardContent);
}

void FAssetEditor_InheritanceTree::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FAssetEditor_InheritanceTree::CanDuplicateNodes()
{
	return CanCopyNodes();
}

void FAssetEditor_InheritanceTree::GraphSettings()
{
	PropertyWidget->SetObject(EditingGraph);
}

bool FAssetEditor_InheritanceTree::CanGraphSettings() const
{
	return true;
}

//void FAssetEditor_InheritanceTree::AutoArrange()
//{
//	UEdGraph_InheritanceTree* EdGraph = Cast<UEdGraph_InheritanceTree>(EditingGraph->EdGraph);
//	check(EdGraph != nullptr);
//
//	const FScopedTransaction Transaction(LOCTEXT("GenericGraphEditorAutoArrange", "Generic Graph Editor: Auto Arrange"));
//
//	EdGraph->Modify();
//
//	UAutoLayoutStrategy* LayoutStrategy = nullptr;
//	switch (GenricGraphEditorSettings->AutoLayoutStrategy)
//	{
//	case EAutoLayoutStrategy::Tree:
//		LayoutStrategy = NewObject<UAutoLayoutStrategy>(EdGraph, UTreeLayoutStrategy::StaticClass());
//		break;
//	case EAutoLayoutStrategy::ForceDirected:
//		LayoutStrategy = NewObject<UAutoLayoutStrategy>(EdGraph, UForceDirectedLayoutStrategy::StaticClass());
//		break;
//	default:
//		break;
//	}
//
//	if (LayoutStrategy != nullptr)
//	{
//		LayoutStrategy->Settings = GenricGraphEditorSettings;
//		LayoutStrategy->Layout(EdGraph);
//		LayoutStrategy->ConditionalBeginDestroy();
//	}
//	else
//	{
//		LOG_ERROR(TEXT("FAssetEditor_GenericGraph::AutoArrange LayoutStrategy is null."));
//	}
//}
//
//bool FAssetEditor_InheritanceTree::CanAutoArrange() const
//{
//	return EditingGraph != nullptr && Cast<UEdGraph_GenericGraph>(EditingGraph->EdGraph) != nullptr;
//}

void FAssetEditor_InheritanceTree::OnRenameNode()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (CurrentGraphEditor.IsValid())
	{
		const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
		for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
		{
			UEdGraphNode* SelectedNode = Cast<UEdGraphNode>(*NodeIt);
			if (SelectedNode != NULL && SelectedNode->bCanRenameNode)
			{
				CurrentGraphEditor->IsNodeTitleVisible(SelectedNode, true);
				break;
			}
		}
	}
}

bool FAssetEditor_InheritanceTree::CanRenameNodes() const
{
	return false;
}

void FAssetEditor_InheritanceTree::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	for (UObject* SelectionEntry : NewSelection)
	{
		Selection.Add(SelectionEntry);
	}

	if (Selection.Num() == 0) 
	{
		PropertyWidget->SetObject(EditingGraph);

	}
	else
	{
		PropertyWidget->SetObjects(Selection);
	}
}

void FAssetEditor_InheritanceTree::OnNodeDoubleClicked(UEdGraphNode* Node)
{
	
}

void FAssetEditor_InheritanceTree::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (EditingGraph == nullptr)
		return;

	EditingGraph->EdGraph->GetSchema()->ForceVisualizationCacheClear();
}

void FAssetEditor_InheritanceTree::OnPackageSaved(const FString& PackageFileName, UObject* Outer)
{
	RebuildGenericGraph();
}

void FAssetEditor_InheritanceTree::RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager) 
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}


#undef LOCTEXT_NAMESPACE

