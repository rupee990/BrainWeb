#include "EdNode_BrainWebNode_Start.h"

#define LOCTEXT_NAMESPACE "EdNode_BrainWeb_Start"

UEdNode_BrainWebNode_Start::UEdNode_BrainWebNode_Start()
{
	bCanRenameNode = true;
}

UEdNode_BrainWebNode_Start::~UEdNode_BrainWebNode_Start()
{

}

void UEdNode_BrainWebNode_Start::AllocateDefaultPins()
{
	FCreatePinParams Params;
	Params.Index = 0;
	CreatePin(EGPD_Output, "MultipleNodes", FName(), TEXT("Out"), Params);
}

UEdGraphPin* UEdNode_BrainWebNode_Start::GetInputPin() const
{
	return nullptr;
}

UEdGraphPin* UEdNode_BrainWebNode_Start::GetOutputPin() const
{
	return Pins[0];
}
