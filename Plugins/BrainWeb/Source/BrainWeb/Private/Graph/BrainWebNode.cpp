#include "BrainWebNode.h"
#include "BrainWebGraph.h"

#define LOCTEXT_NAMESPACE "BrainWebNode"

UBrainWebNode::UBrainWebNode()
{
#if WITH_EDITORONLY_DATA
	CompatibleGraphType = UBrainWebGraph::StaticClass();

	
	BackgroundColor = FLinearColor::Black;
#endif
}

UBrainWebNode::~UBrainWebNode()
{

}

UBrainWebEdge* UBrainWebNode::GetEdge(UBrainWebNode* ChildNode)
{
	return Edges.Contains(ChildNode) ? Edges.FindChecked(ChildNode) : nullptr;
}

FText UBrainWebNode::GetDescription_Implementation() const
{
	return LOCTEXT("NodeDesc", "Brain Web Node");
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

bool UBrainWebNode::IsLeafNode() const
{
	return ChildrenNodes.Num() == 0;
}

UBrainWebGraph* UBrainWebNode::GetGraph() const
{
	return Graph;
}

#undef LOCTEXT_NAMESPACE
