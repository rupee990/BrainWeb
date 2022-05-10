#include "Graph/InheritanceTreeGraph.h"
//#include "GenericGraphRuntimePCH.h"
#include "EdGraph_InheritanceTree.h"
#include "Engine/Engine.h"
#include "Graph/Editor/Nodes/BaseNode/EdNode_InheritanceTreeNode.h"

#define LOCTEXT_NAMESPACE "InheritanceTreeGraph"

UInheritanceTreeGraph::UInheritanceTreeGraph()
{
	bEdgeEnabled = true;

#if WITH_EDITORONLY_DATA
	EdGraph = nullptr;
#endif
}

UInheritanceTreeGraph::~UInheritanceTreeGraph()
{

}

TSubclassOf<UInheritanceTreeNode> UInheritanceTreeGraph::GetNodeType(int32 Index)
{
	return UInheritanceTreeNode::StaticClass();
}

void UInheritanceTreeGraph::Print(bool ToConsole /*= true*/, bool ToScreen /*= true*/)
{
	int Level = 0;
	TArray<UInheritanceTreeNode*> CurrLevelNodes = RootNodes;
	TArray<UInheritanceTreeNode*> NextLevelNodes;

	while (CurrLevelNodes.Num() != 0)
	{
		for (int i = 0; i < CurrLevelNodes.Num(); ++i)
		{
			UInheritanceTreeNode* Node = CurrLevelNodes[i];
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

int UInheritanceTreeGraph::GetLevelNum() const
{
	int Level = 0;
	TArray<UInheritanceTreeNode*> CurrLevelNodes = RootNodes;
	TArray<UInheritanceTreeNode*> NextLevelNodes;

	while (CurrLevelNodes.Num() != 0)
	{
		for (int i = 0; i < CurrLevelNodes.Num(); ++i)
		{
			UInheritanceTreeNode* Node = CurrLevelNodes[i];
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

void UInheritanceTreeGraph::GetNodesByLevel(int Level, TArray<UInheritanceTreeNode*>& Nodes)
{
	int CurrLevel = 0;
	TArray<UInheritanceTreeNode*> NextLevelNodes;

	Nodes = RootNodes;

	while (Nodes.Num() != 0)
	{
		if (CurrLevel == Level)
			break;

		for (int i = 0; i < Nodes.Num(); ++i)
		{
			UInheritanceTreeNode* Node = Nodes[i];
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

#pragma optimize("", off)
void UInheritanceTreeGraph::BuildInheritanceGraph()
{	
	if(!IsValid(EdGraph))
	{
		return;
	}

	ClearGraph();
	
	// Look For all the child classes
	TArray<UClass*> Found;
	for(TObjectIterator<UClass> It; It; ++It)
	{
		if(It->IsChildOf(Source) && !It->HasAnyClassFlags(CLASS_Abstract))
		{
			Found.Add(*It);
		}
	}

	FVector2D Start = FVector2D(0.0f, 0.0f);
	// Create Root node
	UEdNode_InheritanceTreeNode* EdRootNode = NewObject<UEdNode_InheritanceTreeNode>(EdGraph, UEdNode_InheritanceTreeNode::StaticClass(), TEXT("EdNode_Root"));
	if(IsValid(EdRootNode))
	{
		EdGraph->AddNode(EdRootNode);
		
		EdRootNode->NodePosX = Start.X;
		EdRootNode->NodePosY = Start.Y;
		EdRootNode->Class = Source;

		EdRootNode->InheritanceTreeNode = NewObject<UInheritanceTreeNode>(EdGraph, UInheritanceTreeNode::StaticClass());
		EdRootNode->InheritanceTreeNode->Graph = this;
		EdRootNode->InheritanceTreeNode->NodeColor = FLinearColor::Blue;

		EdRootNode->CreateNewGuid();
		EdRootNode->PostPlacedNewNode();
		EdRootNode->AllocateDefaultPins();
		
		Start = FVector2D(Start.X, Start.Y + 200);
	}

	bool bExit = false;
	CurrentNum = 0;
	BuildInheritance_Recursive(Source, EdRootNode, Start, 0, bExit);
	
	// For Every Node Found Spawn a node
	/*int32 id = 0;
	for(UClass* Class : Found)
	{
		if(Class == Source.Get())
		{
			continue;
		}

		FString NodeName = "Node_" + FString::FromInt(id);
		if(UEdNode_InheritanceTreeNode* EdNode = NewObject<UEdNode_InheritanceTreeNode>(EdGraph, UEdNode_InheritanceTreeNode::StaticClass()))
		{
			EdGraph->AddNode(EdNode);
			
			EdNode->NodePosX = Start.X;
			EdNode->NodePosY = Start.Y;
			EdNode->Class = Class;
			
			EdNode->InheritanceTreeNode = NewObject<UInheritanceTreeNode>(EdGraph, UInheritanceTreeNode::StaticClass());
			EdNode->InheritanceTreeNode->Graph = this;
			EdNode->InheritanceTreeNode->NodeColor = FLinearColor::Gray;
			
			EdNode->CreateNewGuid();
			EdNode->PostPlacedNewNode();
			EdNode->AllocateDefaultPins();
			EdRootNode->GetOutputPin()->MakeLinkTo(EdNode->GetInputPin());
			//EdNode->AutowireNewNode(EdRootNode->GetOutputPin());

			Start = FVector2D(Start.X + 150, Start.Y);
		}
		id++;
	}*/
}

bool UInheritanceTreeGraph::BuildInheritance_Recursive(UClass* InClass, UEdNode_InheritanceTreeNode* EdRootNode, FVector2D& Location, int32 id, bool bExit)
{
	if(bExit)
	{
		return true;
	}
	
	// Look For all the child classes
	TArray<UClass*> Found;
	for(TObjectIterator<UClass> It; It; ++It)
	{
		if(It->IsChildOf(InClass) && !It->HasAnyClassFlags(CLASS_Abstract))
		{
			Found.Add(*It);
		}
	}

	if(Found.Num() == 0)
	{
		bExit = true;
		return true;
	}

	for(UClass* Class : Found)
	{	
		if(Class == InClass)
		{
			continue;
		}

		if(UsedClass.Contains(Class))
		{
			continue;
		}

		UsedClass.Add(Class);
		FString NodeName = "Node_" + FString::FromInt(id);
		if(UEdNode_InheritanceTreeNode* EdNode = NewObject<UEdNode_InheritanceTreeNode>(EdGraph, UEdNode_InheritanceTreeNode::StaticClass()))
		{
			EdGraph->AddNode(EdNode);
			
			EdNode->NodePosX = Location.X;
			EdNode->NodePosY = Location.Y;
			EdNode->Class = Class;
			
			EdNode->InheritanceTreeNode = NewObject<UInheritanceTreeNode>(EdGraph, UInheritanceTreeNode::StaticClass());
			EdNode->InheritanceTreeNode->Graph = this;
			EdNode->InheritanceTreeNode->NodeColor = FLinearColor::Gray;
			
			EdNode->CreateNewGuid();
			EdNode->PostPlacedNewNode();
			EdNode->AllocateDefaultPins();
			EdRootNode->GetOutputPin()->MakeLinkTo(EdNode->GetInputPin());

			Location = FVector2D(Location.X + 150, Location.Y);

			CurrentNum++;
			if(CurrentNum > MaxExit)
			{
				return true;
			}

			FVector2D InLocation = Location + FVector2D((Found.Num() / 2) * -150, 100);
			if(BuildInheritance_Recursive(Class, EdNode, InLocation, ++id, bExit))
			{
				return true;
			}
		}
	}

	return false;
}
#pragma optimize("", on)

void UInheritanceTreeGraph::ClearGraph()
{
	for (int i = 0; i < AllNodes.Num(); ++i)
	{
		UInheritanceTreeNode* Node = AllNodes[i];

		Node->ParentNodes.Empty();
		Node->ChildrenNodes.Empty();
	}

	AllNodes.Empty();
	RootNodes.Empty();
}

UInheritanceTreeNode* UInheritanceTreeGraph::GetStartNodeByIndex(int32 ID)
{
	// IMPLEMENT GET START NODE

	return nullptr;
}

#undef LOCTEXT_NAMESPACE
