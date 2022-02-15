#include "Nodes/BrainWebNode_Query.h"
#include "Graph/BrainWebGraph.h"

#define LOCTEXT_NAMESPACE "BrainWebNode_Message"

UBrainWebNode_Query::UBrainWebNode_Query()
{
#if WITH_EDITORONLY_DATA
	CompatibleGraphType = UBrainWebGraph::StaticClass();
#endif

	BackgroundColor = FLinearColor::Yellow;
	NodeTitle = FText::FromString("Query Node");
	QuestionText = FText::FromString("Question Here?");
}

UBrainWebNode_Query::~UBrainWebNode_Query()
{

}

void UBrainWebNode_Query::Execute()
{
	Super::Execute();
}

void UBrainWebNode_Query::MoveToBranch(int32 Branch)
{
	if(ChildrenNodes.IsValidIndex(Branch))
	{
		Graph->SetNextNode(ChildrenNodes[Branch]);
	}
}

void UBrainWebNode_Query::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if(PropertyChangedEvent.GetPropertyName() == "Queries")
	{
		OnNodeChanged.Broadcast();
	}
		
}
#undef LOCTEXT_NAMESPACE
