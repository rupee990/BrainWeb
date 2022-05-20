#include "Graph/BrainWebGraph.h"
//#include "GenericGraphRuntimePCH.h"
#include "Engine/Engine.h"
#include "Nodes/BrainWebNode_End.h"
#include "Nodes/BrainWebNode_Message.h"
#include "Nodes/BrainWebNode_Query.h"
#include "Nodes/BrainWebNode_Start.h"

#define LOCTEXT_NAMESPACE "BrainWebGraph"

UBrainWebGraph::UBrainWebGraph()
{
	NodeTypes.Add(UBrainWebNode_Message::StaticClass());
	NodeTypes.Add(UBrainWebNode_Start::StaticClass());
	NodeTypes.Add(UBrainWebNode_End::StaticClass());
	NodeTypes.Add(UBrainWebNode_Query::StaticClass());

	bEdgeEnabled = true;

#if WITH_EDITORONLY_DATA
	EdGraph = nullptr;

	bCanRenameNode = true;
#endif
}

UBrainWebGraph::~UBrainWebGraph()
{

}

TSubclassOf<UBrainWebNode> UBrainWebGraph::GetNodeType(int32 Index)
{
	if(NodeTypes.IsValidIndex(Index))
	{
		return NodeTypes[Index];
	}

	return UBrainWebNode::StaticClass();
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
	int CurrLevel = 0;
	TArray<UBrainWebNode*> NextLevelNodes;

	Nodes = RootNodes;

	while (Nodes.Num() != 0)
	{
		if (CurrLevel == Level)
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
		++CurrLevel;
	}
}

void UBrainWebGraph::ClearGraph()
{
	for (int i = 0; i < AllNodes.Num(); ++i)
	{
		UBrainWebNode* Node = AllNodes[i];

		Node->ParentNodes.Empty();
		Node->ChildrenNodes.Empty();
	}

	AllNodes.Empty();
	RootNodes.Empty();
}

UBrainWebNode* UBrainWebGraph::GetStartNodeByIndex(const int32 ID, bool& bWasFound)
{
	for(int32 i = 0; i < AllNodes.Num(); i++)
	{
		if(UBrainWebNode_Start* Node = Cast <UBrainWebNode_Start>( AllNodes[i]))
		{
			if(Node->SpeechIndex == ID)
			{
				bWasFound = true;
				return Node;
			}
		}
	}
	
	bWasFound = false;
	return nullptr;
}

void UBrainWebGraph::NextNode()
{
	if(CurrentNode == nullptr)
	{
		return;
	}

	if(UBrainWebNode* Child = CurrentNode->GetChildNode(0))
	{
		LastNode = CurrentNode;
		CurrentNode = Child;
	}
	else if(CurrentNode->ChildrenNodes.IsValidIndex(0))
	{
		LastNode = CurrentNode;
		CurrentNode = CurrentNode->ChildrenNodes[0];
	}
	else
	{
		if(LastNode != nullptr)
		{
			CurrentNode = LastNode;
		}
	}

	OnNodeEnter(CurrentNode);
}

void UBrainWebGraph::SetNextNode(UBrainWebNode* NextNode)
{
	if(IsValid(NextNode))
	{
		LastNode = CurrentNode;
		CurrentNode = NextNode;

		OnNodeEnter(CurrentNode);
	}
}

void UBrainWebGraph::OnNodeEnter_Implementation(UBrainWebNode* Node)
{
	OnNodeExecuteDelegate.Broadcast(Node, Node->StaticClass());
	Node->Execute();
}

void UBrainWebGraph::OnNodeExit(UBrainWebNode* Node)
{
}

void UBrainWebGraph::StartDialogue(int32 DialogueIndex)
{
	bool bWasFound = false;
	CurrentNode = GetStartNodeByIndex(DialogueIndex, bWasFound);

	if(bWasFound)
	{
		if(UBrainWebNode_Start* StartNode = Cast<UBrainWebNode_Start>(CurrentNode))
		{
			OnDialogStart(StartNode);
			OnDialogueStartDelegate.Broadcast(StartNode);

			StartNode->Execute();
		}
	}
}

void UBrainWebGraph::EndDialogue()
{
	UBrainWebNode_End* EndNode = Cast<UBrainWebNode_End>(CurrentNode);
	OnDialogEnd(EndNode);
	OnDialogueEndDelegate.Broadcast(EndNode);
}

void UBrainWebGraph::OnDialogStart_Implementation(UBrainWebNode_Start* StartNode)
{
	CurrentNode = StartNode;
}

void UBrainWebGraph::OnDialogEnd_Implementation(UBrainWebNode_End* EndNode)
{
	CurrentNode = nullptr;
	
}

#undef LOCTEXT_NAMESPACE
