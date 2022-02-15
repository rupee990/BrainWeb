#include "EdGraph_BrainWeb.h"
//#include "GenericGraphEditorPCH.h"
#include "Graph/BrainWebGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "Graph/Editor/Nodes/BaseNode/EdNode_BrainWebNode.h"

UEdGraph_BrainWeb::UEdGraph_BrainWeb()
{

}

UEdGraph_BrainWeb::~UEdGraph_BrainWeb()
{

}

void UEdGraph_BrainWeb::RebuildGenericGraph()
{
	UBrainWebGraph* Graph = GetGenericGraph();

	Clear();

	for (int i = 0; i < Nodes.Num(); ++i)
	{
		if (UEdNode_BrainWebNode* EdNode = Cast<UEdNode_BrainWebNode>(Nodes[i]))
		{
			if (EdNode->BrainWebNode == nullptr)
				continue;

			//EdNode->GeneratePins();

			UBrainWebNode* GenericGraphNode = EdNode->BrainWebNode;

			NodeMap.Add(GenericGraphNode, EdNode);

			Graph->AllNodes.Add(GenericGraphNode);

			for (int PinIdx = 0; PinIdx < EdNode->Pins.Num(); ++PinIdx)
			{
				UEdGraphPin* Pin = EdNode->Pins[PinIdx];

				if (Pin->Direction != EEdGraphPinDirection::EGPD_Output)
					continue;

				for (int LinkToIdx = 0; LinkToIdx < Pin->LinkedTo.Num(); ++LinkToIdx)
				{
					if(Pin->LinkedTo[LinkToIdx] == nullptr)
					{
						Pin->LinkedTo.RemoveAt(LinkToIdx);
						LinkToIdx--;
						continue;	
					}
					
					UBrainWebNode* ChildNode = nullptr;
					if (UEdNode_BrainWebNode* EdNode_Child = Cast<UEdNode_BrainWebNode>(Pin->LinkedTo[LinkToIdx]->GetOwningNode()))
					{
						ChildNode = EdNode_Child->BrainWebNode;
					}

					if (ChildNode != nullptr)
					{
						GenericGraphNode->ChildrenNodes.Add(ChildNode);

						ChildNode->ParentNodes.Add(GenericGraphNode);
					}
				}
			}
		}
	}

	for (int i = 0; i < Graph->AllNodes.Num(); ++i)
	{
		UBrainWebNode* Node = Graph->AllNodes[i];
		if (Node->ParentNodes.Num() == 0)
		{
			Graph->RootNodes.Add(Node);

			SortNodes(Node);
		}

		Node->Graph = Graph;
		Node->Rename(nullptr, Graph, REN_DontCreateRedirectors | REN_DoNotDirty);
	}

	Graph->RootNodes.Sort([&](const UBrainWebNode& L, const UBrainWebNode& R)
	{
		UEdGraphNode* EdNode_LNode = NodeMap[&L];
		UEdGraphNode* EdNode_RNode = NodeMap[&R];
		return EdNode_LNode->NodePosX < EdNode_RNode->NodePosX;
	});
}

UBrainWebGraph* UEdGraph_BrainWeb::GetGenericGraph() const
{
	return CastChecked<UBrainWebGraph>(GetOuter());
}

bool UEdGraph_BrainWeb::Modify(bool bAlwaysMarkDirty /*= true*/)
{
	bool Rtn = Super::Modify(bAlwaysMarkDirty);

	GetGenericGraph()->Modify();

	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		Nodes[i]->Modify();
	}

	return Rtn;
}

void UEdGraph_BrainWeb::Clear()
{
	UBrainWebGraph* Graph = GetGenericGraph();

	Graph->ClearGraph();
	NodeMap.Reset();

	for (int i = 0; i < Nodes.Num(); ++i)
	{
		if (UEdNode_BrainWebNode* EdNode = Cast<UEdNode_BrainWebNode>(Nodes[i]))
		{
			UBrainWebNode* GenericGraphNode = EdNode->BrainWebNode;
			GenericGraphNode->ParentNodes.Reset();
			GenericGraphNode->ChildrenNodes.Reset();
		}
	}
}

void UEdGraph_BrainWeb::SortNodes(UBrainWebNode* RootNode)
{
	int Level = 0;
	TArray<UBrainWebNode*> CurrLevelNodes = { RootNode };
	TArray<UBrainWebNode*> NextLevelNodes;

	while (CurrLevelNodes.Num() != 0)
	{
		int32 LevelWidth = 0;
		for (int i = 0; i < CurrLevelNodes.Num(); ++i)
		{
			UBrainWebNode* Node = CurrLevelNodes[i];

			auto Comp = [&](const UBrainWebNode& L, const UBrainWebNode& R)
			{
				UEdGraphNode* EdNode_LNode = NodeMap[&L];
				UEdGraphNode* EdNode_RNode = NodeMap[&R];
				return EdNode_LNode->NodePosX < EdNode_RNode->NodePosX;
			};

			Node->ChildrenNodes.Sort(Comp);
			Node->ParentNodes.Sort(Comp);

			for (int j = 0; j < Node->ChildrenNodes.Num(); ++j)
			{
				NextLevelNodes.Add(Node->ChildrenNodes[j]);
			}
		}

		CurrLevelNodes = NextLevelNodes;
		NextLevelNodes.Reset();
		++Level;
	}
}

void UEdGraph_BrainWeb::PostEditUndo()
{
	Super::PostEditUndo();

	NotifyGraphChanged();
}

