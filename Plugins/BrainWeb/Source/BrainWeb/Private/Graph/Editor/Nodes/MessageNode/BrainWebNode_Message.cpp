#include "Nodes/BrainWebNode_Message.h"
#include "Graph/BrainWebGraph.h"
#include "Nodes/BrainWebNode_Query.h"

#define LOCTEXT_NAMESPACE "BrainWebNode_Message"

UBrainWebNode_Message::UBrainWebNode_Message()
{
#if WITH_EDITORONLY_DATA
	CompatibleGraphType = UBrainWebGraph::StaticClass();
#endif

	BackgroundColor = FLinearColor::White;
	NodeTitle = FText::FromString("Message Node");
	Message = FText::FromString("-= Message Here =-");
}

UBrainWebNode_Message::~UBrainWebNode_Message()
{

}

UBrainWebNode* UBrainWebNode_Message::GetChildNode(int32 Id)
{
	if(ChildrenNodes.Num() > 0)
	{
		return ChildrenNodes[0];
	}

	//Find Last Query
	UBrainWebNode* Parent = ParentNodes[0];
	while(Parent != nullptr)
	{
		if(Cast<UBrainWebNode_Query>(Parent))
		{
			return Parent;
		}

		if(Parent->ParentNodes.Num() > 0)
		{
			Parent = Parent->ParentNodes[0];
		}
		else
		{
			Parent = nullptr;
		}
	}

	return Parent;
}
#undef LOCTEXT_NAMESPACE
