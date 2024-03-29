#include "EdNode_BrainWebNode_End.h"
#define LOCTEXT_NAMESPACE "EdNode_BrainWeb_Message"

UEdNode_BrainWebNode_End::UEdNode_BrainWebNode_End()
{
	bCanRenameNode = true;
}

void UEdNode_BrainWebNode_End::AllocateDefaultPins()
{
	FCreatePinParams Params;
	Params.Index = 0;
	CreatePin(EGPD_Input, "MultipleNodes", FName(), TEXT("In"), Params);
}

UEdGraphPin* UEdNode_BrainWebNode_End::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* UEdNode_BrainWebNode_End::GetOutputPin() const
{
	return nullptr;
}

