#include "AssetEditor_BrainWeb.h"
//#include "GenericGraphEditorPCH.h"
#include "AssetEditorToolbar_BrainWeb.h"
#include "AssetGraphSchema_BrainWeb.h"
//#include "EditorCommands_BrainWeb.h"

#include "AssetToolsModule.h"
#include "HAL/PlatformApplicationMisc.h"

#include "GraphEditorActions.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraphUtilities.h"
#include "Graph/Editor/Graph/EdGraph_BrainWeb.h"
#include "Graph/Editor/Nodes/BaseNode/EdNode_BrainWebNode.h"
#include "GraphEditor.h"
#include "ScopedTransaction.h"
#include "Framework/Commands/GenericCommands.h"
#define LOCTEXT_NAMESPACE "AssetEditor_BrainWeb"

const FName GenericGraphEditorAppName = FName(TEXT("BrainWebEditorApp"));

struct FBrainWebAssetEditorTabs
{
	// Tab identifiers
	static const FName BrainWebPropertyID;
	static const FName ViewportID;
	static const FName BrainWebEditorSettingsID;
};

//////////////////////////////////////////////////////////////////////////

const FName FBrainWebAssetEditorTabs::BrainWebPropertyID(TEXT("BrainWebProperty"));
const FName FBrainWebAssetEditorTabs::ViewportID(TEXT("Viewport"));
const FName FBrainWebAssetEditorTabs::BrainWebEditorSettingsID(TEXT("BrainWebEditorSettings"));

//////////////////////////////////////////////////////////////////////////

FAssetEditor_BrainWeb::FAssetEditor_BrainWeb()
{
	EditingGraph = nullptr;

	GenricGraphEditorSettings = NewObject<UBrainWebEditorSettings>(UBrainWebEditorSettings::StaticClass());

	OnPackageSavedDelegateHandle = UPackage::PackageSavedEvent.AddRaw(this, &FAssetEditor_BrainWeb::OnPackageSaved);
}

FAssetEditor_BrainWeb::~FAssetEditor_BrainWeb()
{
	UPackage::PackageSavedEvent.Remove(OnPackageSavedDelegateHandle);
}

void FAssetEditor_BrainWeb::InitBrainWebAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UBrainWebGraph* Graph)
{
	EditingGraph = Graph;
	CreateEdGraph();

	FGenericCommands::Register();
	FGraphEditorCommands::Register();
	//EditorCommands_BrainWeb::Register();

	if (!ToolbarBuilder.IsValid())
	{
		ToolbarBuilder = MakeShareable(new FAssetEditorToolbar_BrainWeb(SharedThis(this)));
	}

	BindCommands();

	CreateInternalWidgets();

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

	ToolbarBuilder->AddGenericGraphToolbar(ToolbarExtender);
	
	// Layout
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_BrainWebEditor_Layout_v1")
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
					->AddTab(FBrainWebAssetEditorTabs::ViewportID, ETabState::OpenedTab)->SetHideTabWell(true)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.7f)
						->AddTab(FBrainWebAssetEditorTabs::BrainWebPropertyID, ETabState::OpenedTab)->SetHideTabWell(true)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.3f)
						->AddTab(FBrainWebAssetEditorTabs::BrainWebEditorSettingsID, ETabState::OpenedTab)
					)
				)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, GenericGraphEditorAppName, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, EditingGraph, false);

	RegenerateMenusAndToolbars();
}

void FAssetEditor_BrainWeb::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_BrainWebEditor", "Brain Web Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FBrainWebAssetEditorTabs::ViewportID, FOnSpawnTab::CreateSP(this, &FAssetEditor_BrainWeb::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("GraphCanvasTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(FBrainWebAssetEditorTabs::BrainWebPropertyID, FOnSpawnTab::CreateSP(this, &FAssetEditor_BrainWeb::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("DetailsTab", "Property"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FBrainWebAssetEditorTabs::BrainWebEditorSettingsID, FOnSpawnTab::CreateSP(this, &FAssetEditor_BrainWeb::SpawnTab_EditorSettings))
		.SetDisplayName(LOCTEXT("EditorSettingsTab", "Brain Web Editor Setttings"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FAssetEditor_BrainWeb::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(FBrainWebAssetEditorTabs::ViewportID);
	InTabManager->UnregisterTabSpawner(FBrainWebAssetEditorTabs::BrainWebPropertyID);
	InTabManager->UnregisterTabSpawner(FBrainWebAssetEditorTabs::BrainWebEditorSettingsID);
}

FName FAssetEditor_BrainWeb::GetToolkitFName() const
{
	return FName("FBrainWebEditor");
}

FText FAssetEditor_BrainWeb::GetBaseToolkitName() const
{
	return LOCTEXT("GenericGraphEditorAppLabel", "Brain Web Editor");
}

FText FAssetEditor_BrainWeb::GetToolkitName() const
{
	const bool bDirtyState = EditingGraph->GetOutermost()->IsDirty();

	FFormatNamedArguments Args;
	Args.Add(TEXT("BrainWebName"), FText::FromString(EditingGraph->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(LOCTEXT("BrainWebEditorToolkitName", "{BrainWebName}{DirtyState}"), Args);
}

FText FAssetEditor_BrainWeb::GetToolkitToolTipText() const
{
	return FAssetEditorToolkit::GetToolTipTextForObject(EditingGraph);
}

FLinearColor FAssetEditor_BrainWeb::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FString FAssetEditor_BrainWeb::GetWorldCentricTabPrefix() const
{
	return TEXT("MindWebEditor");
}

FString FAssetEditor_BrainWeb::GetDocumentationLink() const
{
	return TEXT("");
}

void FAssetEditor_BrainWeb::SaveAsset_Execute()
{
	if (EditingGraph != nullptr)
	{
		RebuildGenericGraph();
	}

	FAssetEditorToolkit::SaveAsset_Execute();
}

void FAssetEditor_BrainWeb::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditingGraph);
	Collector.AddReferencedObject(EditingGraph->EdGraph);
}

UBrainWebEditorSettings* FAssetEditor_BrainWeb::GetSettings() const
{
	return GenricGraphEditorSettings;
}

TSharedRef<SDockTab> FAssetEditor_BrainWeb::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FBrainWebAssetEditorTabs::ViewportID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("ViewportTab_Title", "Viewport"));

	if (ViewportWidget.IsValid())
	{
		SpawnedTab->SetContent(ViewportWidget.ToSharedRef());
	}

	return SpawnedTab;
}

TSharedRef<SDockTab> FAssetEditor_BrainWeb::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FBrainWebAssetEditorTabs::BrainWebPropertyID);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("Details_Title", "Property"))
		[
			PropertyWidget.ToSharedRef()
		];
}

TSharedRef<SDockTab> FAssetEditor_BrainWeb::SpawnTab_EditorSettings(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FBrainWebAssetEditorTabs::BrainWebEditorSettingsID);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("EditorSettings_Title", "Brain Web Editor Setttings"))
		[
			EditorSettingsWidget.ToSharedRef()
		];
}

void FAssetEditor_BrainWeb::CreateInternalWidgets()
{
	ViewportWidget = CreateViewportWidget();

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.NotifyHook = this;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyWidget = PropertyModule.CreateDetailView(Args);
	PropertyWidget->SetObject(EditingGraph);
	PropertyWidget->OnFinishedChangingProperties().AddSP(this, &FAssetEditor_BrainWeb::OnFinishedChangingProperties);

	EditorSettingsWidget = PropertyModule.CreateDetailView(Args);
	EditorSettingsWidget->SetObject(GenricGraphEditorSettings);
}

TSharedRef<SGraphEditor> FAssetEditor_BrainWeb::CreateViewportWidget()
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_GenericGraph", "Web Graph");

	CreateCommandList();

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FAssetEditor_BrainWeb::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FAssetEditor_BrainWeb::OnNodeDoubleClicked);

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(EditingGraph->EdGraph)
		.GraphEvents(InEvents)
		.AutoExpandActionMenu(true)
		.ShowGraphStateOverlay(false);
}

void FAssetEditor_BrainWeb::BindCommands()
{
	//ToolkitCommands->MapAction(FEditorCommands_BrainWeb::Get().GraphSettings,
	//	FExecuteAction::CreateSP(this, &FAssetEditor_BrainWeb::GraphSettings),
	//	FCanExecuteAction::CreateSP(this, &FAssetEditor_BrainWeb::CanGraphSettings)
	//);

	//ToolkitCommands->MapAction(FEditorCommands_BrainWeb::Get().AutoArrange,
	//	FExecuteAction::CreateSP(this, &FAssetEditor_BrainWeb::AutoArrange),
	//	FCanExecuteAction::CreateSP(this, &FAssetEditor_BrainWeb::CanAutoArrange)
	//);
}

void FAssetEditor_BrainWeb::CreateEdGraph()
{
	if (EditingGraph->EdGraph == nullptr)
	{
		EditingGraph->EdGraph = CastChecked<UEdGraph_BrainWeb>(FBlueprintEditorUtils::CreateNewGraph(EditingGraph, NAME_None, UEdGraph_BrainWeb::StaticClass(), UAssetGraphSchema_BrainWeb::StaticClass()));
		EditingGraph->EdGraph->bAllowDeletion = false;

		// Give the schema a chance to fill out any required nodes (like the results node)
		const UEdGraphSchema* Schema = EditingGraph->EdGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*EditingGraph->EdGraph);
	}
}

void FAssetEditor_BrainWeb::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}

	GraphEditorCommands = MakeShareable(new FUICommandList);

	// Can't use CreateSP here because derived editor are already implementing TSharedFromThis<FAssetEditorToolkit>
	// however it should be safe, since commands are being used only within this editor
	// if it ever crashes, this function will have to go away and be reimplemented in each derived class

	//GraphEditorCommands->MapAction(FEditorCommands_BrainWeb::Get().GraphSettings,
	//	FExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::GraphSettings),
	//	FCanExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::CanGraphSettings));

	//GraphEditorCommands->MapAction(FEditorCommands_BrainWeb::Get().AutoArrange,
	//	FExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::AutoArrange),
	//	FCanExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::CanAutoArrange));

	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::CanSelectAllNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::CanDeleteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::CanCopyNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::CanCutNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::CanPasteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_BrainWeb::CanDuplicateNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Rename,
		FExecuteAction::CreateSP(this, &FAssetEditor_BrainWeb::OnRenameNode),
		FCanExecuteAction::CreateSP(this, &FAssetEditor_BrainWeb::CanRenameNodes)
	);
}

TSharedPtr<SGraphEditor> FAssetEditor_BrainWeb::GetCurrGraphEditor() const
{
	return ViewportWidget;
}

FGraphPanelSelectionSet FAssetEditor_BrainWeb::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	TSharedPtr<SGraphEditor> FocusedGraphEd = GetCurrGraphEditor();
	if (FocusedGraphEd.IsValid())
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}

	return CurrentSelection;
}

void FAssetEditor_BrainWeb::RebuildGenericGraph()
{
	//if (EditingGraph == nullptr)
	//{
	//	LOG_WARNING(TEXT("FGenericGraphAssetEditor::RebuildGenericGraph EditingGraph is nullptr"));
	//	return;
	//}

	UEdGraph_BrainWeb* EdGraph = Cast<UEdGraph_BrainWeb>(EditingGraph->EdGraph);
	check(EdGraph != nullptr);

	EdGraph->RebuildGenericGraph();
}

void FAssetEditor_BrainWeb::SelectAllNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (CurrentGraphEditor.IsValid())
	{
		CurrentGraphEditor->SelectAllNodes();
	}
}

bool FAssetEditor_BrainWeb::CanSelectAllNodes()
{
	return true;
}

void FAssetEditor_BrainWeb::DeleteSelectedNodes()
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

		if (UEdNode_BrainWebNode* EdNode_Node = Cast<UEdNode_BrainWebNode>(EdNode))
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

bool FAssetEditor_BrainWeb::CanDeleteNodes()
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

void FAssetEditor_BrainWeb::DeleteSelectedDuplicatableNodes()
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

void FAssetEditor_BrainWeb::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}

bool FAssetEditor_BrainWeb::CanCutNodes()
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FAssetEditor_BrainWeb::CopySelectedNodes()
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

bool FAssetEditor_BrainWeb::CanCopyNodes()
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

void FAssetEditor_BrainWeb::PasteNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (CurrentGraphEditor.IsValid())
	{
		PasteNodesHere(CurrentGraphEditor->GetPasteLocation());
	}
}

void FAssetEditor_BrainWeb::PasteNodesHere(const FVector2D& Location)
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

bool FAssetEditor_BrainWeb::CanPasteNodes()
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

void FAssetEditor_BrainWeb::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FAssetEditor_BrainWeb::CanDuplicateNodes()
{
	return CanCopyNodes();
}

void FAssetEditor_BrainWeb::GraphSettings()
{
	PropertyWidget->SetObject(EditingGraph);
}

bool FAssetEditor_BrainWeb::CanGraphSettings() const
{
	return true;
}

//void FAssetEditor_BrainWeb::AutoArrange()
//{
//	UEdGraph_BrainWeb* EdGraph = Cast<UEdGraph_BrainWeb>(EditingGraph->EdGraph);
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
//bool FAssetEditor_BrainWeb::CanAutoArrange() const
//{
//	return EditingGraph != nullptr && Cast<UEdGraph_GenericGraph>(EditingGraph->EdGraph) != nullptr;
//}

void FAssetEditor_BrainWeb::OnRenameNode()
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

bool FAssetEditor_BrainWeb::CanRenameNodes() const
{
	UEdGraph_BrainWeb* EdGraph = Cast<UEdGraph_BrainWeb>(EditingGraph->EdGraph);
	check(EdGraph != nullptr);

	UBrainWebGraph* Graph = EdGraph->GetGenericGraph();
	check(Graph != nullptr)

	return Graph->bCanRenameNode && GetSelectedNodes().Num() == 1;
}

void FAssetEditor_BrainWeb::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
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

void FAssetEditor_BrainWeb::OnNodeDoubleClicked(UEdGraphNode* Node)
{
	
}

void FAssetEditor_BrainWeb::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (EditingGraph == nullptr)
		return;

	EditingGraph->EdGraph->GetSchema()->ForceVisualizationCacheClear();
}

void FAssetEditor_BrainWeb::OnPackageSaved(const FString& PackageFileName, UObject* Outer)
{
	RebuildGenericGraph();
}

void FAssetEditor_BrainWeb::RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager) 
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}


#undef LOCTEXT_NAMESPACE

