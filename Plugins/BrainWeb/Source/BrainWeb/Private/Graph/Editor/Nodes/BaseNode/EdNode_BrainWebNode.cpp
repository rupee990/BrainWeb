#include "EdNode_BrainWebNode.h"
#include "Graph/Editor/Graph/EdGraph_BrainWeb.h"

#define LOCTEXT_NAMESPACE "EdNode_BrainWeb"

UEdNode_BrainWebNode::UEdNode_BrainWebNode()
{
	bCanRenameNode = true;
}

UEdNode_BrainWebNode::~UEdNode_BrainWebNode()
{

}

void UEdNode_BrainWebNode::AllocateDefaultPins()
{
	FCreatePinParams Params;
	Params.Index = 0;
	CreatePin(EGPD_Input, "MultipleNodes", FName(), TEXT("In"),Params);
	Params.Index = 1;
	CreatePin(EGPD_Output, "MultipleNodes", FName(), TEXT("Out"), Params);
}

UEdGraph_BrainWeb* UEdNode_BrainWebNode::GetGenericGraphEdGraph()
{
	return Cast<UEdGraph_BrainWeb>(GetGraph());
}

FText UEdNode_BrainWebNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (BrainWebNode == nullptr)
	{
		return Super::GetNodeTitle(TitleType);
	}
	else
	{
		return BrainWebNode->GetNodeTitle();
	}
}

void UEdNode_BrainWebNode::PrepareForCopying()
{
	BrainWebNode->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
}

void UEdNode_BrainWebNode::AutowireNewNode(UEdGraphPin* FromPin)
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

void UEdNode_BrainWebNode::GeneratePins()
{
	Pins.Empty();
	AllocateDefaultPins();
}

void UEdNode_BrainWebNode::SetGenericGraphNode(UBrainWebNode* InNode)
{
	BrainWebNode = InNode;
}

FLinearColor UEdNode_BrainWebNode::GetBackgroundColor() const
{
	return BrainWebNode == nullptr ? FLinearColor::Black : BrainWebNode->GetBackgroundColor();
}

UEdGraphPin* UEdNode_BrainWebNode::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* UEdNode_BrainWebNode::GetOutputPin() const
{
	return Pins[1];
}

void UEdNode_BrainWebNode::PostEditUndo()
{
	UEdGraphNode::PostEditUndo();
}

#undef LOCTEXT_NAMESPACE
