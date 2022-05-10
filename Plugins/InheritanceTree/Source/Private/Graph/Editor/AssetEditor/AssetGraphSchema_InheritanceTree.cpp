#include "AssetGraphSchema_InheritanceTree.h"
//#include "GenericGraphEditorPCH.h"#include "Graph/Editor/Style/ "

#include "Graph/Editor/Nodes/BaseNode/EdNode_InheritanceTreeNode.h"
#include "Graph/Editor/Style/ConnectionDrawingPolicy_InheritanceTree.h"
#include "ScopedTransaction.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "Graph/InheritanceTreeGraph.h"

#define LOCTEXT_NAMESPACE "AssetSchema_GenericGraph"

int32 UAssetGraphSchema_InheritanceTree::CurrentCacheRefreshID = 0;

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

UEdGraphNode* FAssetSchemaAction_InheritanceTree_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode /*= true*/)
{
	UEdGraphNode* ResultNode = nullptr;

	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("InheritanceTreeEditorNewNode", "Brain Web Editor: New Node"));
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

		NodeTemplate->InheritanceTreeNode->SetFlags(RF_Transactional);
		NodeTemplate->SetFlags(RF_Transactional);

		NodeTemplate->SetGenericGraphNode(NodeTemplate->InheritanceTreeNode);
		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

void FAssetSchemaAction_InheritanceTree_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);
}

void UAssetGraphSchema_InheritanceTree::GetBreakLinkToSubMenuActions(class FMenuBuilder& MenuBuilder, class UEdGraphPin* InGraphPin)
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
			FExecuteAction::CreateUObject(this, &UAssetGraphSchema_InheritanceTree::BreakSinglePinLink, const_cast<UEdGraphPin*>(InGraphPin), *Links)));
	}
}

EGraphType UAssetGraphSchema_InheritanceTree::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return GT_StateMachine;
}

void UAssetGraphSchema_InheritanceTree::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	UInheritanceTreeGraph* Graph = CastChecked<UInheritanceTreeGraph>(ContextMenuBuilder.CurrentGraph->GetOuter());

	TSubclassOf<UInheritanceTreeNode> Class;
		
	if (Class == nullptr)
	{
		return;
	}

	const bool bNoParent = (ContextMenuBuilder.FromPin == NULL);
	const FText AddToolTip = LOCTEXT("NewInheritanceTreeNodeTooltip", "Add node here");
	TSet<TSubclassOf<UInheritanceTreeNode> > Visited;
	FText Desc = FText();
	if (Desc.IsEmpty())
	{
		FString Title = Class->GetName();
		Title.RemoveFromEnd("_C");
		Desc = FText::FromString(Title);
	}
	
	if (!Class->HasAnyClassFlags(CLASS_Abstract))
	{
		TSharedPtr<FAssetSchemaAction_InheritanceTree_NewNode> NewNodeAction(new FAssetSchemaAction_InheritanceTree_NewNode(LOCTEXT("GenericGraphNodeAction", "Brain Web Node"), Desc, AddToolTip, 0));
		TSubclassOf<UEdNode_InheritanceTreeNode> UEdNode = UEdNode_InheritanceTreeNode::StaticClass();
		
		NewNodeAction->NodeTemplate = NewObject<UEdNode_InheritanceTreeNode>(ContextMenuBuilder.OwnerOfTemporaries, UEdNode);
		NewNodeAction->NodeTemplate->InheritanceTreeNode = NewObject<UInheritanceTreeNode>(NewNodeAction->NodeTemplate, Class);
		NewNodeAction->NodeTemplate->InheritanceTreeNode->Graph = Graph;
		ContextMenuBuilder.AddAction(NewNodeAction);
		Visited.Add(Class);
	}
}

void UAssetGraphSchema_InheritanceTree::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	Super::GetContextMenuActions(Menu, Context);
}

const FPinConnectionResponse UAssetGraphSchema_InheritanceTree::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	// Make sure the pins are not on the same node
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorSameNode", "Both are on the same node"));
	}

	// Compare the directions
    UEdNode_InheritanceTreeNode* EdNode_A = Cast<UEdNode_InheritanceTreeNode>(A->GetOwningNode());
    UEdNode_InheritanceTreeNode* EdNode_B = Cast<UEdNode_InheritanceTreeNode>(B->GetOwningNode());

    if (EdNode_A == nullptr || EdNode_B == nullptr)
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinError", "Not a valid UGenericGraphEdNode"));
    }

	FText ErrorMessage;
	if (A->Direction == EGPD_Input)
	{
		if (!EdNode_A->InheritanceTreeNode->CanCreateConnection(EdNode_B->InheritanceTreeNode, ErrorMessage))
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, ErrorMessage);
		}
	}
	else
	{
		if (!EdNode_B->InheritanceTreeNode->CanCreateConnection(EdNode_A->InheritanceTreeNode, ErrorMessage))
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, ErrorMessage);
		}
	}

    if (EdNode_A->InheritanceTreeNode->GetGraph()->bEdgeEnabled)
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, LOCTEXT("PinConnect", "Connect nodes with edge"));
    }
    else
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, LOCTEXT("PinConnect", "Connect nodes"));
    }
}

bool UAssetGraphSchema_InheritanceTree::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* A, UEdGraphPin* B) const
{

	if(A == nullptr || B == nullptr)
	{
		return false;
	}
	
	UEdNode_InheritanceTreeNode* NodeA = Cast<UEdNode_InheritanceTreeNode>(A->GetOwningNode());
	UEdNode_InheritanceTreeNode* NodeB = Cast<UEdNode_InheritanceTreeNode>(B->GetOwningNode());

	if (NodeA == nullptr || NodeB == nullptr)
		return false;
	
	NodeA->Modify();
	A->Modify();
	A->SetOwningNode(NodeA);
	A->LinkedTo.Empty();
	A->MakeLinkTo(B);

	return true;
}

class FConnectionDrawingPolicy* UAssetGraphSchema_InheritanceTree
    ::CreateConnectionDrawingPolicy(int32 InBackLayerID, 
        int32 InFrontLayerID, 
        float InZoomFactor, 
        const FSlateRect& InClippingRect, 
        class FSlateWindowElementList& InDrawElements, 
        class UEdGraph* InGraphObj) const
{
	return new FConnectionDrawingPolicy_InheritanceTree(InBackLayerID, 
        InFrontLayerID, 
        InZoomFactor, 
        InClippingRect, 
        InDrawElements, 
        InGraphObj);
}

FLinearColor UAssetGraphSchema_InheritanceTree::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FColor::White;
}

void UAssetGraphSchema_InheritanceTree::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakNodeLinks", "Break Node Links"));

	Super::BreakNodeLinks(TargetNode);
}

void UAssetGraphSchema_InheritanceTree::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links"));

	if(TargetPin.LinkedTo.Num() > 0)
	{
		for(int32 i = 0; i < TargetPin.LinkedTo.Num(); i++)
		{
			if(TargetPin.LinkedTo[i] == nullptr)
			{
				TargetPin.LinkedTo.RemoveAt(i);
			}
		}
	}
	
	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UAssetGraphSchema_InheritanceTree::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakSinglePinLink", "Break Pin Link"));

	Super::BreakSinglePinLink(SourcePin, TargetPin);
}

UEdGraphPin* UAssetGraphSchema_InheritanceTree::DropPinOnNode(UEdGraphNode* InTargetNode, const FName& InSourcePinName, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection) const
{
	UEdNode_InheritanceTreeNode* EdNode = Cast<UEdNode_InheritanceTreeNode>(InTargetNode);
	switch (InSourcePinDirection)
	{
	case EGPD_Input:
	{
		return EdNode->GetOutputPin();
	}
	case EGPD_Output:
	{
		return EdNode->GetInputPin();
	}
	default:
		return nullptr;
	}
}

bool UAssetGraphSchema_InheritanceTree::SupportsDropPinOnNode(UEdGraphNode* InTargetNode, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection, FText& OutErrorMessage) const
{
	return Cast<UEdNode_InheritanceTreeNode>(InTargetNode) != nullptr;
}

bool UAssetGraphSchema_InheritanceTree::IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const
{
	return CurrentCacheRefreshID != InVisualizationCacheID;
}

int32 UAssetGraphSchema_InheritanceTree::GetCurrentVisualizationCacheID() const
{
	return CurrentCacheRefreshID;
}

void UAssetGraphSchema_InheritanceTree::ForceVisualizationCacheClear() const
{
	++CurrentCacheRefreshID;
}

#if WITH_EDITOR
#pragma optimize("", off)
bool UAssetGraphSchema_InheritanceTree::TryCreateConnection(UEdGraphPin* a_A, UEdGraphPin* a_B) const
{
	UEdNode_InheritanceTreeNode* NodeA = Cast<UEdNode_InheritanceTreeNode>(a_A->GetOwningNode());
	UEdNode_InheritanceTreeNode* NodeB = Cast<UEdNode_InheritanceTreeNode>(a_B->GetOwningNode());
	UEdGraphPin* OutputA = a_A;
	UEdGraphPin* InputB = NodeB->GetInputPin();
	if (!OutputA || !InputB)
	{
		return false;
	}
			
	return UEdGraphSchema::TryCreateConnection(OutputA, InputB);;

}
#pragma optimize("", on)
#endif

#undef LOCTEXT_NAMESPACE
