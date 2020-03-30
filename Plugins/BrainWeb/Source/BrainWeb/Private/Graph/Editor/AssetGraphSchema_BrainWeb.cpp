#include "AssetGraphSchema_BrainWeb.h"
//#include "GenericGraphEditorPCH.h"
#include "EdNode_BrainWebNode.h"
#include "EdNode_BrainWebEdge.h"
#include "ConnectionDrawingPolicy_BrainWeb.h"
#include "GraphEditorActions.h"
#include "Framework/Commands/GenericCommands.h"
#include "ScopedTransaction.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraph/EdGraph.h"
#include "UObjectIterator.h"
#include "MultiBoxBuilder.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"

#define LOCTEXT_NAMESPACE "AssetSchema_GenericGraph"

int32 UAssetGraphSchema_BrainWeb::CurrentCacheRefreshID = 0;

class FNodeVisitorCycleChecker
{
public:
	/** Check whether a loop in the graph would be caused by linking the passed-in nodes */
	bool CheckForLoop(UEdGraphNode* StartNode, UEdGraphNode* EndNode)
	{
		VisitedNodes.Add(StartNode);

		return TraverseInputNodesToRoot(EndNode);
	}

private:
	bool TraverseInputNodesToRoot(UEdGraphNode* Node)
	{
		VisitedNodes.Add(Node);

		for (int32 PinIndex = 0; PinIndex < Node->Pins.Num(); ++PinIndex)
		{
			UEdGraphPin* MyPin = Node->Pins[PinIndex];

			if (MyPin->Direction == EGPD_Output)
			{
				for (int32 LinkedPinIndex = 0; LinkedPinIndex < MyPin->LinkedTo.Num(); ++LinkedPinIndex)
				{
					UEdGraphPin* OtherPin = MyPin->LinkedTo[LinkedPinIndex];
					if (OtherPin)
					{
						UEdGraphNode* OtherNode = OtherPin->GetOwningNode();
						if (VisitedNodes.Contains(OtherNode))
						{
							return false;
						}
						else
						{
							return TraverseInputNodesToRoot(OtherNode);
						}
					}
				}
			}
		}

		return true;
	}

	TSet<UEdGraphNode*> VisitedNodes;
};

UEdGraphNode* FAssetSchemaAction_BrainWeb_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	UEdGraphNode* ResultNode = nullptr;

	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("BrainWebEditorNewNode", "Brain Web Editor: New Node"));
		ParentGraph->Modify();
		if (FromPin != nullptr)
			FromPin->Modify();

		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->BrainWebNode->SetFlags(RF_Transactional);
		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

void FAssetSchemaAction_BrainWeb_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

UEdGraphNode* FAssetSchemaAction_BrainWeb_NewEdge::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	UEdGraphNode* ResultNode = nullptr;

	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("GenericGraphEditorNewEdge", "Brain Web Editor: New Edge"));
		ParentGraph->Modify();
		if (FromPin != nullptr)
			FromPin->Modify();

		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->GenericGraphEdge->SetFlags(RF_Transactional);
		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;
	}
	
	return ResultNode;
}

void FAssetSchemaAction_BrainWeb_NewEdge::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

void UAssetGraphSchema_BrainWeb::GetBreakLinkToSubMenuActions(class FMenuBuilder& MenuBuilder, class UEdGraphPin* InGraphPin)
{
	// Make sure we have a unique name for every entry in the list
	TMap< FString, uint32 > LinkTitleCount;

	// Add all the links we could break from
	for (TArray<class UEdGraphPin*>::TConstIterator Links(InGraphPin->LinkedTo); Links; ++Links)
	{
		UEdGraphPin* Pin = *Links;
		FString TitleString = Pin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString();
		FText Title = FText::FromString(TitleString);
		if (Pin->PinName != TEXT(""))
		{
			TitleString = FString::Printf(TEXT("%s (%s)"), *TitleString, *Pin->PinName.ToString());

			// Add name of connection if possible
			FFormatNamedArguments Args;
			Args.Add(TEXT("NodeTitle"), Title);
			Args.Add(TEXT("PinName"), Pin->GetDisplayName());
			Title = FText::Format(LOCTEXT("BreakDescPin", "{NodeTitle} ({PinName})"), Args);
		}

		uint32 &Count = LinkTitleCount.FindOrAdd(TitleString);

		FText Description;
		FFormatNamedArguments Args;
		Args.Add(TEXT("NodeTitle"), Title);
		Args.Add(TEXT("NumberOfNodes"), Count);

		if (Count == 0)
		{
			Description = FText::Format(LOCTEXT("BreakDesc", "Break link to {NodeTitle}"), Args);
		}
		else
		{
			Description = FText::Format(LOCTEXT("BreakDescMulti", "Break link to {NodeTitle} ({NumberOfNodes})"), Args);
		}
		++Count;

		MenuBuilder.AddMenuEntry(Description, Description, FSlateIcon(), FUIAction(
			FExecuteAction::CreateUObject(this, &UAssetGraphSchema_BrainWeb::BreakSinglePinLink, const_cast<UEdGraphPin*>(InGraphPin), *Links)));
	}
}

EGraphType UAssetGraphSchema_BrainWeb::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return GT_StateMachine;
}

void UAssetGraphSchema_BrainWeb::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	UBrainWebGraph* Graph = CastChecked<UBrainWebGraph>(ContextMenuBuilder.CurrentGraph->GetOuter());

	if (Graph->NodeType == nullptr)
	{
		return;
	}

	const bool bNoParent = (ContextMenuBuilder.FromPin == NULL);

	const FText AddToolTip = LOCTEXT("NewBrainWebNodeTooltip", "Add node here");

	TSet<TSubclassOf<UBrainWebNode> > Visited;

	FText Desc = Graph->NodeType.GetDefaultObject()->ContextMenuName;

	if (Desc.IsEmpty())
	{
		FString Title = Graph->NodeType->GetName();
		Title.RemoveFromEnd("_C");
		Desc = FText::FromString(Title);
	}

	if (!Graph->NodeType->HasAnyClassFlags(CLASS_Abstract))
	{
		TSharedPtr<FAssetSchemaAction_BrainWeb_NewNode> NewNodeAction(new FAssetSchemaAction_BrainWeb_NewNode(LOCTEXT("GenericGraphNodeAction", "Brain Web Node"), Desc, AddToolTip, 0));
		NewNodeAction->NodeTemplate = NewObject<UEdNode_BrainWebNode>(ContextMenuBuilder.OwnerOfTemporaries);
		NewNodeAction->NodeTemplate->BrainWebNode = NewObject<UBrainWebNode>(NewNodeAction->NodeTemplate, Graph->NodeType);
		NewNodeAction->NodeTemplate->BrainWebNode->Graph = Graph;
		ContextMenuBuilder.AddAction(NewNodeAction);

		Visited.Add(Graph->NodeType);
	}

	for (TObjectIterator<UClass> It; It; ++It)
	{
		if (It->IsChildOf(Graph->NodeType) && !It->HasAnyClassFlags(CLASS_Abstract) && !Visited.Contains(*It))
		{
			TSubclassOf<UBrainWebNode> NodeType = *It;

			if (It->GetName().StartsWith("REINST") || It->GetName().StartsWith("SKEL"))
				continue;

			if (!Graph->GetClass()->IsChildOf(NodeType.GetDefaultObject()->CompatibleGraphType))
				continue;

			Desc = NodeType.GetDefaultObject()->ContextMenuName;

			if (Desc.IsEmpty())
			{
				FString Title = NodeType->GetName();
				Title.RemoveFromEnd("_C");
				Desc = FText::FromString(Title);
			}

			TSharedPtr<FAssetSchemaAction_BrainWeb_NewNode> Action(new FAssetSchemaAction_BrainWeb_NewNode(LOCTEXT("GenericGraphNodeAction", "BrainWeb Node"), Desc, AddToolTip, 0));
			Action->NodeTemplate = NewObject<UEdNode_BrainWebNode>(ContextMenuBuilder.OwnerOfTemporaries);
			Action->NodeTemplate->BrainWebNode = NewObject<UBrainWebNode>(Action->NodeTemplate, NodeType);
			Action->NodeTemplate->BrainWebNode->Graph = Graph;
			ContextMenuBuilder.AddAction(Action);

			Visited.Add(NodeType);
		}
	}
}

void UAssetGraphSchema_BrainWeb::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, class FMenuBuilder* MenuBuilder, bool bIsDebugging) const
{
	if (InGraphPin != nullptr)
	{
		MenuBuilder->BeginSection("GenericGraphAssetGraphSchemaNodeActions", LOCTEXT("PinActionsMenuHeader", "Pin Actions"));
		{
			// Only display the 'Break Link' option if there is a link to break!
			if (InGraphPin->LinkedTo.Num() > 0)
			{
				MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakPinLinks);

				// add sub menu for break link to
				if (InGraphPin->LinkedTo.Num() > 1)
				{
					MenuBuilder->AddSubMenu(
						LOCTEXT("BreakLinkTo", "Break Link To..."),
						LOCTEXT("BreakSpecificLinks", "Break a specific link..."),
						FNewMenuDelegate::CreateUObject((UAssetGraphSchema_BrainWeb*const)this, &UAssetGraphSchema_BrainWeb::GetBreakLinkToSubMenuActions, const_cast<UEdGraphPin*>(InGraphPin)));
				}
				else
				{
					((UAssetGraphSchema_BrainWeb*const)this)->GetBreakLinkToSubMenuActions(*MenuBuilder, const_cast<UEdGraphPin*>(InGraphPin));
				}
			}
		}
		MenuBuilder->EndSection();
	}
	else if(InGraphNode != nullptr)
	{
		MenuBuilder->BeginSection("GenericGraphAssetGraphSchemaNodeActions", LOCTEXT("NodeActionsMenuHeader", "Node Actions"));
		{
			MenuBuilder->AddMenuEntry(FGenericCommands::Get().Delete);
			MenuBuilder->AddMenuEntry(FGenericCommands::Get().Cut);
			MenuBuilder->AddMenuEntry(FGenericCommands::Get().Copy);
			MenuBuilder->AddMenuEntry(FGenericCommands::Get().Duplicate);

			MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
		}
		MenuBuilder->EndSection();
	}

	Super::GetContextMenuActions(CurrentGraph, InGraphNode, InGraphPin, MenuBuilder, bIsDebugging);
}

const FPinConnectionResponse UAssetGraphSchema_BrainWeb::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	// Make sure the pins are not on the same node
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorSameNode", "Both are on the same node"));
	}

	// Compare the directions
	//if ((A->Direction == EGPD_Input) && (B->Direction == EGPD_Input))
	//{
	//	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorInput", "Can't connect input node to input node"));
	//}
	//else if ((A->Direction == EGPD_Output) && (B->Direction == EGPD_Output))
	//{
	//	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorOutput", "Can't connect output node to output node"));
	//}

	//// check for cycles
	//FNodeVisitorCycleChecker CycleChecker;
	//if (!CycleChecker.CheckForLoop(A->GetOwningNode(), B->GetOwningNode()))
	//{
	//	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorCycle", "Can't create a graph cycle"));
	//}

    UEdNode_BrainWebNode* EdNode_A = Cast<UEdNode_BrainWebNode>(A->GetOwningNode());
    UEdNode_BrainWebNode* EdNode_B = Cast<UEdNode_BrainWebNode>(B->GetOwningNode());

    if (EdNode_A == nullptr || EdNode_B == nullptr)
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinError", "Not a valid UGenericGraphEdNode"));
    }

	//FText ErrorMessage;
	//if (A->Direction == EGPD_Input)
	//{
	//	if (!EdNode_A->GenericGraphNode->CanCreateConnection(EdNode_B->GenericGraphNode, ErrorMessage))
	//	{
	//		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, ErrorMessage);
	//	}
	//}
	//else
	//{
	//	if (!EdNode_B->GenericGraphNode->CanCreateConnection(EdNode_A->GenericGraphNode, ErrorMessage))
	//	{
	//		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, ErrorMessage);
	//	}
	//}

    if (EdNode_A->BrainWebNode->GetGraph()->bEdgeEnabled)
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, LOCTEXT("PinConnect", "Connect nodes with edge"));
    }
    else
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, LOCTEXT("PinConnect", "Connect nodes"));
    }
}

bool UAssetGraphSchema_BrainWeb::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* A, UEdGraphPin* B) const
{
	UEdNode_BrainWebNode* NodeA = Cast<UEdNode_BrainWebNode>(A->GetOwningNode());
	UEdNode_BrainWebNode* NodeB = Cast<UEdNode_BrainWebNode>(B->GetOwningNode());

	if (NodeA == nullptr || NodeB == nullptr)
		return false;

	if (NodeA->GetInputPin() == nullptr || NodeA->GetOutputPin() == nullptr || NodeB->GetInputPin() == nullptr || NodeB->GetOutputPin() == nullptr)
		return false;

	UBrainWebGraph* Graph = NodeA->BrainWebNode->GetGraph();

	FVector2D InitPos((NodeA->NodePosX + NodeB->NodePosX) / 2, (NodeA->NodePosY + NodeB->NodePosY) / 2);

	FAssetSchemaAction_BrainWeb_NewEdge Action;
	Action.NodeTemplate = NewObject<UEdNode_BrainWebEdge>(NodeA->GetGraph());
	Action.NodeTemplate->SetEdge(NewObject<UBrainWebEdge>(Action.NodeTemplate, Graph->EdgeType));
	UEdNode_BrainWebEdge* EdgeNode = Cast<UEdNode_BrainWebEdge>(Action.PerformAction(NodeA->GetGraph(), nullptr, InitPos, false));

	if (A->Direction == EGPD_Output)
	{
		EdgeNode->CreateConnections(NodeA, NodeB);
	}
	else
	{
		EdgeNode->CreateConnections(NodeB, NodeA);
	}

	return true;
}

class FConnectionDrawingPolicy* UAssetGraphSchema_BrainWeb
    ::CreateConnectionDrawingPolicy(int32 InBackLayerID, 
        int32 InFrontLayerID, 
        float InZoomFactor, 
        const FSlateRect& InClippingRect, 
        class FSlateWindowElementList& InDrawElements, 
        class UEdGraph* InGraphObj) const
{
	return new FConnectionDrawingPolicy_BrainWeb(InBackLayerID, 
        InFrontLayerID, 
        InZoomFactor, 
        InClippingRect, 
        InDrawElements, 
        InGraphObj);
}

FLinearColor UAssetGraphSchema_BrainWeb::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FColor::White;
}

void UAssetGraphSchema_BrainWeb::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakNodeLinks", "Break Node Links"));

	Super::BreakNodeLinks(TargetNode);
}

void UAssetGraphSchema_BrainWeb::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links"));

	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UAssetGraphSchema_BrainWeb::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakSinglePinLink", "Break Pin Link"));

	Super::BreakSinglePinLink(SourcePin, TargetPin);
}

UEdGraphPin* UAssetGraphSchema_BrainWeb::DropPinOnNode(UEdGraphNode* InTargetNode, const FName& InSourcePinName, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection) const
{
	UEdNode_BrainWebNode* EdNode = Cast<UEdNode_BrainWebNode>(InTargetNode);
	switch (InSourcePinDirection)
	{
	case EGPD_Input:
		return EdNode->GetOutputPin();
	case EGPD_Output:
		return EdNode->GetInputPin();
	default:
		return nullptr;
	}
}

bool UAssetGraphSchema_BrainWeb::SupportsDropPinOnNode(UEdGraphNode* InTargetNode, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection, FText& OutErrorMessage) const
{
	return Cast<UEdNode_BrainWebNode>(InTargetNode) != nullptr;
}

bool UAssetGraphSchema_BrainWeb::IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const
{
	return CurrentCacheRefreshID != InVisualizationCacheID;
}

int32 UAssetGraphSchema_BrainWeb::GetCurrentVisualizationCacheID() const
{
	return CurrentCacheRefreshID;
}

void UAssetGraphSchema_BrainWeb::ForceVisualizationCacheClear() const
{
	++CurrentCacheRefreshID;
}

#if WITH_EDITOR
bool UAssetGraphSchema_BrainWeb::TryCreateConnection(UEdGraphPin* a_A, UEdGraphPin* a_B) const
{
	UEdNode_BrainWebNode* NodeA = Cast<UEdNode_BrainWebNode>(a_A->GetOwningNode());
	UEdNode_BrainWebNode* NodeB = Cast<UEdNode_BrainWebNode>(a_B->GetOwningNode());
	UEdGraphPin* OutputA = NodeA->GetOutputPin();
	UEdGraphPin* InputB = NodeB->GetInputPin();
	if (!OutputA || !InputB)
	{
		return false;
	}

	bool bConnectionMade = UEdGraphSchema::TryCreateConnection(OutputA, InputB);

	return true;

}
#endif

#undef LOCTEXT_NAMESPACE
