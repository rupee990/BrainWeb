#include "Nodes/BrainWebNode_End.h"
#include "Graph/BrainWebGraph.h"

#define LOCTEXT_NAMESPACE "BrainWebNode_Message"

UBrainWebNode_End::UBrainWebNode_End()
{
#if WITH_EDITORONLY_DATA
	CompatibleGraphType = UBrainWebGraph::StaticClass();
#endif

	BackgroundColor = FLinearColor::Red;
	NodeTitle = FText::FromString("End Node");
}

UBrainWebNode_End::~UBrainWebNode_End()
{

}

void UBrainWebNode_End::Execute()
{
	if(Graph)
	{
		Graph->EndDialogue();
	}
}
#undef LOCTEXT_NAMESPACE
