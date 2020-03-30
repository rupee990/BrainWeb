#include "EdNode_BrainWebEdge.h"
#include "BrainWebEdge.h"
#include "EdNode_BrainWebNode.h"

#define LOCTEXT_NAMESPACE "EdNode_BrainWebEdge"

void UEdNode_BrainWebEdge::SetEdge(UBrainWebEdge* Edge)
{
	GenericGraphEdge = Edge;
}

void UEdNode_BrainWebEdge::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Edge"), FName(), TEXT("In"));
	Inputs->bHidden = true;
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Edge"), FName(), TEXT("Out"));
	Outputs->bHidden = true;
}

FText UEdNode_BrainWebEdge::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText();
}

void UEdNode_BrainWebEdge::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin->LinkedTo.Num() == 0)
	{
		// Commit suicide; transitions must always have an input and output connection
		Modify();

		// Our parent graph will have our graph in SubGraphs so needs to be modified to record that.
		if (UEdGraph* ParentGraph = GetGraph())
		{
			ParentGraph->Modify();
		}

		DestroyNode();
	}
}

void UEdNode_BrainWebEdge::PrepareForCopying()
{
	GenericGraphEdge->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
}

void UEdNode_BrainWebEdge::CreateConnections(UEdNode_BrainWebNode* Start, UEdNode_BrainWebNode* End)
{
	Pins[0]->Modify();
	Pins[0]->LinkedTo.Empty();

	Start->GetOutputPin()->Modify();
	Pins[0]->MakeLinkTo(Start->GetOutputPin());

	// This to next
	Pins[1]->Modify();
	Pins[1]->LinkedTo.Empty();

	End->GetInputPin()->Modify();
	Pins[1]->MakeLinkTo(End->GetInputPin());
}

UEdNode_BrainWebNode* UEdNode_BrainWebEdge::GetStartNode()
{
	if (Pins[0]->LinkedTo.Num() > 0)
	{
		return Cast<UEdNode_BrainWebNode>(Pins[0]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return nullptr;
	}
}

UEdNode_BrainWebNode* UEdNode_BrainWebEdge::GetEndNode()
{
	if (Pins[1]->LinkedTo.Num() > 0)
	{
		return Cast<UEdNode_BrainWebNode>(Pins[1]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return nullptr;
	}
}

#undef LOCTEXT_NAMESPACE

