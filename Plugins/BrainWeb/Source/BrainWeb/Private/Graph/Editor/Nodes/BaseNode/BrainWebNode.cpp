#include "Nodes/BrainWebNode.h"
#include "Graph/BrainWebGraph.h"

#define LOCTEXT_NAMESPACE "BrainWebNode_Start"

UBrainWebNode::UBrainWebNode()
{
#if WITH_EDITORONLY_DATA
	CompatibleGraphType = UBrainWebGraph::StaticClass();
#endif

	NodeTitle = FText::FromString("Node");
}

UBrainWebNode::~UBrainWebNode()
{
}

void UBrainWebNode::Execute()
{
	OnExecute.Broadcast();
}

FText UBrainWebNode::GetDescription_Implementation() const
{
	return LOCTEXT("NodeDesc", "Dialog Start");
}

#if WITH_EDITOR

FLinearColor UBrainWebNode::GetBackgroundColor() const
{
	return BackgroundColor;
}

FText UBrainWebNode::GetNodeTitle() const
{
	return NodeTitle.IsEmpty() ? GetDescription() : NodeTitle;
}

void UBrainWebNode::SetNodeTitle(const FText& NewTitle)
{
	NodeTitle = NewTitle;
}

bool UBrainWebNode::CanCreateConnection(UBrainWebNode* Other, FText& ErrorMessage)
{
	return true;
}

#endif

UBrainWebNode* UBrainWebNode::GetParentNode(const int32 Id)
{
	if(ParentNodes.IsValidIndex(Id))
	{
		return ParentNodes[Id];
	}

	return nullptr;
}

UBrainWebNode* UBrainWebNode::GetChildNode(const int32 Id)
{
	if(ChildrenNodes.IsValidIndex(Id))
	{
		return ChildrenNodes[Id];
	}

	return nullptr;
}

bool UBrainWebNode::IsLeafNode() const
{
	return ChildrenNodes.Num() == 0;
}

UBrainWebGraph* UBrainWebNode::GetGraph() const
{
	return Graph;
}

#undef LOCTEXT_NAMESPACE
