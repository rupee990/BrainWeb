#include "EdGraph_InheritanceTree.h"
//#include "GenericGraphEditorPCH.h"
#include "Graph/InheritanceTreeGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "Graph/Editor/Nodes/BaseNode/EdNode_InheritanceTreeNode.h"

UEdGraph_InheritanceTree::UEdGraph_InheritanceTree()
{

}

UEdGraph_InheritanceTree::~UEdGraph_InheritanceTree()
{

}

void UEdGraph_InheritanceTree::RebuildGenericGraph()
{
	UInheritanceTreeGraph* Graph = GetGenericGraph();

	Clear();

	for (int i = 0; i < Nodes.Num(); ++i)
	{
		if (UEdNode_InheritanceTreeNode* EdNode = Cast<UEdNode_InheritanceTreeNode>(Nodes[i]))
		{
			if (EdNode->InheritanceTreeNode == nullptr)
				continue;

			//EdNode->GeneratePins();

			UInheritanceTreeNode* GenericGraphNode = EdNode->InheritanceTreeNode;

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
					
					UInheritanceTreeNode* ChildNode = nullptr;
					if (UEdNode_InheritanceTreeNode* EdNode_Child = Cast<UEdNode_InheritanceTreeNode>(Pin->LinkedTo[LinkToIdx]->GetOwningNode()))
					{
						ChildNode = EdNode_Child->InheritanceTreeNode;
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
		UInheritanceTreeNode* Node = Graph->AllNodes[i];
		if (Node->ParentNodes.Num() == 0)
		{
			Graph->RootNodes.Add(Node);

			SortNodes(Node);
		}

		Node->Graph = Graph;
		Node->Rename(nullptr, Graph, REN_DontCreateRedirectors | REN_DoNotDirty);
	}

	Graph->RootNodes.Sort([&](const UInheritanceTreeNode& L, const UInheritanceTreeNode& R)
	{
		UEdGraphNode* EdNode_LNode = NodeMap[&L];
		UEdGraphNode* EdNode_RNode = NodeMap[&R];
		return EdNode_LNode->NodePosX < EdNode_RNode->NodePosX;
	});
}

UInheritanceTreeGraph* UEdGraph_InheritanceTree::GetGenericGraph() const
{
	return CastChecked<UInheritanceTreeGraph>(GetOuter());
}

bool UEdGraph_InheritanceTree::Modify(bool bAlwaysMarkDirty /*= true*/)
{
	bool Rtn = Super::Modify(bAlwaysMarkDirty);

	GetGenericGraph()->Modify();

	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		Nodes[i]->Modify();
	}

	return Rtn;
}

void UEdGraph_InheritanceTree::Clear()
{
	if(UInheritanceTreeGraph* Graph = GetGenericGraph())
	{
		Graph->ClearGraph();
		NodeMap.Reset();

		for (int i = 0; i < Nodes.Num(); ++i)
		{
			if (UEdNode_InheritanceTreeNode* EdNode = Cast<UEdNode_InheritanceTreeNode>(Nodes[i]))
			{
				UInheritanceTreeNode* GenericGraphNode = EdNode->InheritanceTreeNode;
				GenericGraphNode->ParentNodes.Reset();
				GenericGraphNode->ChildrenNodes.Reset();
			}
		}
	}
}

void UEdGraph_InheritanceTree::SortNodes(UInheritanceTreeNode* RootNode)
{
	int Level = 0;
	TArray<UInheritanceTreeNode*> CurrLevelNodes = { RootNode };
	TArray<UInheritanceTreeNode*> NextLevelNodes;

	while (CurrLevelNodes.Num() != 0)
	{
		int32 LevelWidth = 0;
		for (int i = 0; i < CurrLevelNodes.Num(); ++i)
		{
			UInheritanceTreeNode* Node = CurrLevelNodes[i];

			auto Comp = [&](const UInheritanceTreeNode& L, const UInheritanceTreeNode& R)
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

void UEdGraph_InheritanceTree::PostEditUndo()
{
	Super::PostEditUndo();

	NotifyGraphChanged();
}

