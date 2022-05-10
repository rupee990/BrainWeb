#include "Nodes/InheritanceTreeNode.h"
#include "Graph/InheritanceTreeGraph.h"

#define LOCTEXT_NAMESPACE "InheritanceTreeNode_Start"

UInheritanceTreeNode::UInheritanceTreeNode()
{
}

UInheritanceTreeNode::~UInheritanceTreeNode()
{
}

void UInheritanceTreeNode::Execute()
{
	OnExecute.Broadcast();
}

FText UInheritanceTreeNode::GetDescription_Implementation() const
{
	return LOCTEXT("NodeDesc", "Dialog Start");
}

#if WITH_EDITOR

FLinearColor UInheritanceTreeNode::GetBackgroundColor() const
{
	return NodeColor;
}

FText UInheritanceTreeNode::GetNodeTitle() const
{
	return  GetDescription();
}

void UInheritanceTreeNode::SetNodeTitle(const FText& NewTitle)
{
}

bool UInheritanceTreeNode::CanCreateConnection(UInheritanceTreeNode* Other, FText& ErrorMessage)
{
	return true;
}

#endif

UInheritanceTreeNode* UInheritanceTreeNode::GetParentNode(const int32 Id)
{
	if(ParentNodes.IsValidIndex(Id))
	{
		return ParentNodes[Id];
	}

	return nullptr;
}

UInheritanceTreeNode* UInheritanceTreeNode::GetChildNode(const int32 Id)
{
	if(ChildrenNodes.IsValidIndex(Id))
	{
		return ChildrenNodes[Id];
	}

	return nullptr;
}

bool UInheritanceTreeNode::IsLeafNode() const
{
	return ChildrenNodes.Num() == 0;
}

UInheritanceTreeGraph* UInheritanceTreeNode::GetGraph() const
{
	return Graph;
}

#undef LOCTEXT_NAMESPACE
