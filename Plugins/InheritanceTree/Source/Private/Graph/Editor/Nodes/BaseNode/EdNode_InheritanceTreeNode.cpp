#include "EdNode_InheritanceTreeNode.h"
#include "Graph/Editor/Graph/EdGraph_InheritanceTree.h"

#define LOCTEXT_NAMESPACE "EdNode_InheritanceTree"

UEdNode_InheritanceTreeNode::UEdNode_InheritanceTreeNode()
{
	bCanRenameNode = true;
}

UEdNode_InheritanceTreeNode::~UEdNode_InheritanceTreeNode()
{

}

void UEdNode_InheritanceTreeNode::AllocateDefaultPins()
{
	FCreatePinParams Params;
	Params.Index = 0;
	CreatePin(EGPD_Input, "MultipleNodes", FName(), TEXT("In"),Params);
	Params.Index = 1;
	CreatePin(EGPD_Output, "MultipleNodes", FName(), TEXT("Out"), Params);
}

UEdGraph_InheritanceTree* UEdNode_InheritanceTreeNode::GetGenericGraphEdGraph()
{
	return Cast<UEdGraph_InheritanceTree>(GetGraph());
}

FText UEdNode_InheritanceTreeNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString(*Class->GetName());
}

void UEdNode_InheritanceTreeNode::PrepareForCopying()
{
	InheritanceTreeNode->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
}

void UEdNode_InheritanceTreeNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin != nullptr)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}

void UEdNode_InheritanceTreeNode::GeneratePins()
{
	Pins.Empty();
	AllocateDefaultPins();
}

void UEdNode_InheritanceTreeNode::SetGenericGraphNode(UInheritanceTreeNode* InNode)
{
	InheritanceTreeNode = InNode;
}

FLinearColor UEdNode_InheritanceTreeNode::GetBackgroundColor() const
{
	return InheritanceTreeNode == nullptr ? FLinearColor::Black : InheritanceTreeNode->GetBackgroundColor();
}

UEdGraphPin* UEdNode_InheritanceTreeNode::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* UEdNode_InheritanceTreeNode::GetOutputPin() const
{
	return Pins[1];
}

void UEdNode_InheritanceTreeNode::PostEditUndo()
{
	UEdGraphNode::PostEditUndo();
}

#undef LOCTEXT_NAMESPACE
