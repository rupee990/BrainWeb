#include "BrainWebGraph.h"
//#include "GenericGraphRuntimePCH.h"
#include "Engine/Engine.h"

#define LOCTEXT_NAMESPACE "BrainWebGraph"

UBrainWebGraph::UBrainWebGraph()
{
	NodeType = UBrainWebNode::StaticClass();
	EdgeType = UBrainWebEdge::StaticClass();

	bEdgeEnabled = true;

#if WITH_EDITORONLY_DATA
	EdGraph = nullptr;

	bCanRenameNode = true;
#endif
}

UBrainWebGraph::~UBrainWebGraph()
{

}

void UBrainWebGraph::Print(bool ToConsole /*= true*/, bool ToScreen /*= true*/)
{
	int Level = 0;
	TArray<UBrainWebNode*> CurrLevelNodes = RootNodes;
	TArray<UBrainWebNode*> NextLevelNodes;

	while (CurrLevelNodes.Num() != 0)
	{
		for (int i = 0; i < CurrLevelNodes.Num(); ++i)
		{
			UBrainWebNode* Node = CurrLevelNodes[i];
			check(Node != nullptr);

			FString Message = FString::Printf(TEXT("%s, Level %d"), *Node->GetDescription().ToString(), Level);

			//if (ToConsole)
			//{
			//	LOG_INFO(TEXT("%s"), *Message);
			//}

			if (ToScreen && GEngine != nullptr)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, Message);
			}

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

int UBrainWebGraph::GetLevelNum() const
{
	int Level = 0;
	TArray<UBrainWebNode*> CurrLevelNodes = RootNodes;
	TArray<UBrainWebNode*> NextLevelNodes;

	while (CurrLevelNodes.Num() != 0)
	{
		for (int i = 0; i < CurrLevelNodes.Num(); ++i)
		{
			UBrainWebNode* Node = CurrLevelNodes[i];
			check(Node != nullptr);

			for (int j = 0; j < Node->ChildrenNodes.Num(); ++j)
			{
				NextLevelNodes.Add(Node->ChildrenNodes[j]);
			}
		}

		CurrLevelNodes = NextLevelNodes;
		NextLevelNodes.Reset();
		++Level;
	}

	return Level;
}

void UBrainWebGraph::GetNodesByLevel(int Level, TArray<UBrainWebNode*>& Nodes)
{
	int CurrLEvel = 0;
	TArray<UBrainWebNode*> NextLevelNodes;

	Nodes = RootNodes;

	while (Nodes.Num() != 0)
	{
		if (CurrLEvel == Level)
			break;

		for (int i = 0; i < Nodes.Num(); ++i)
		{
			UBrainWebNode* Node = Nodes[i];
			check(Node != nullptr);

			for (int j = 0; j < Node->ChildrenNodes.Num(); ++j)
			{
				NextLevelNodes.Add(Node->ChildrenNodes[j]);
			}
		}

		Nodes = NextLevelNodes;
		NextLevelNodes.Reset();
		++CurrLEvel;
	}
}

void UBrainWebGraph::ClearGraph()
{
	for (int i = 0; i < AllNodes.Num(); ++i)
	{
		UBrainWebNode* Node = AllNodes[i];

		Node->ParentNodes.Empty();
		Node->ChildrenNodes.Empty();
		Node->Edges.Empty();
	}

	AllNodes.Empty();
	RootNodes.Empty();
}

#undef LOCTEXT_NAMESPACE
