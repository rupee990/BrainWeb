#include "Nodes/BrainWebNode_Start.h"
#include "Graph/BrainWebGraph.h"

#define LOCTEXT_NAMESPACE "BrainWebNode_Start"

UBrainWebNode_Start::UBrainWebNode_Start()
{
#if WITH_EDITORONLY_DATA
	CompatibleGraphType = UBrainWebGraph::StaticClass();
#endif

	BackgroundColor = FLinearColor::Green;
	NodeTitle = FText::FromString("Start Node");
}

UBrainWebNode_Start::~UBrainWebNode_Start()
{

}

void UBrainWebNode_Start::Execute()
{
	Super::Execute();

	Graph->NextNode();
}
#undef LOCTEXT_NAMESPACE
