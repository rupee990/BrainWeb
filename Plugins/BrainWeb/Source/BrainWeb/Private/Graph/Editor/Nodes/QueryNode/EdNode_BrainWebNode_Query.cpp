#include "EdNode_BrainWebNode_Query.h"

#include "Graph/Editor/Graph/EdGraph_BrainWeb.h"
#include "Graph/Editor/Nodes/BaseNode/SEdNode_BrainWebNode.h"
#include "Nodes/BrainWebNode_Query.h"

#define LOCTEXT_NAMESPACE "EdNode_BrainWeb_Message"

struct FPinConnector
{
	TArray<UEdGraphPin*> ToPins;
};

UEdNode_BrainWebNode_Query::UEdNode_BrainWebNode_Query()
{
	bCanRenameNode = true;

	
}

UEdNode_BrainWebNode_Query::~UEdNode_BrainWebNode_Query()
{

}

#pragma optimize("", off)
void UEdNode_BrainWebNode_Query::SetGenericGraphNode(UBrainWebNode* InNode)
{
	Super::SetGenericGraphNode(InNode);

	if(UBrainWebNode_Query* QueryNode = Cast<UBrainWebNode_Query>(InNode))
	{
		QueryNode->OnNodeChanged.AddDynamic(this, &UEdNode_BrainWebNode_Query::GeneratePins);
	}
}

void UEdNode_BrainWebNode_Query::AllocateDefaultPins()
{
	//Create Default Input Pin
	FCreatePinParams Params;
	Params.Index = 0;
	CreatePin(EGPD_Input, "Input", FName(), TEXT("In"), Params);
}

void UEdNode_BrainWebNode_Query::GeneratePins()
{
	//AllocateDefaultPins();
	
	UBrainWebNode_Query* QueryNode = Cast<UBrainWebNode_Query>(BrainWebNode);
	int32 NewIndex = QueryNode->Queries.Num();
	
	if(NewIndex == 0)
	{
		Pins.Empty();
		AllocateDefaultPins();
		
		SEdNode->UpdateGraphNode();
		return;
	}
	
	// If We have Removed Output Pins
	if(NewIndex < Pins.Num() - 1)
	{
		int32 difference = QueryNode->Queries.Num() - (Pins.Num() - 1);
		int32 Index = Pins.Num() - 1;
		for(int32 i = 0; i < difference; i++)
		{
			Pins.RemoveAt(Index);
			Index--;
		}
	}
	// Else if we are adding Pins
	else if(NewIndex > Pins.Num() - 1)
	{
		FName PinName("Out_" + FString::FromInt(NewIndex));
		FCreatePinParams Params;
		Params.Index = NewIndex;
		CreatePin(EGPD_Output, "Outputs", FName(), PinName, Params);
	}

	SEdNode->UpdateGraphNode();
}

#pragma optimize("", on)

UEdGraphPin* UEdNode_BrainWebNode_Query::GetOutputPinFromName(const FName& PinName)
{
	const FString Number = PinName.ToString().RightChop(3);
	const int32 id = FCString::Atoi(*Number);

	if(Pins.IsValidIndex(id + 1))
	{
		return Pins[id + 1];
	}

	return Pins[1];
}

void UEdNode_BrainWebNode_Query::PostInitProperties()
{
	Super::PostInitProperties();

	
}
