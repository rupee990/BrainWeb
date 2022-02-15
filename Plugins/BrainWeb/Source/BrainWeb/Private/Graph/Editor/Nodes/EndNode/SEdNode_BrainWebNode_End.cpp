#include "SEdNode_BrainWebNode_End.h"

#include "EdNode_BrainWebNode_End.h"
#include "Graph/Editor/Nodes/BaseNode/EdNode_BrainWebNode.h"

#define LOCTEXT_NAMESPACE "EdNode_BrainWeb_Message"

//////////////////////////////////////////////////////////////////////////
void SEdNode_BrainWebNode_End::Construct(const FArguments& InArgs, UEdNode_BrainWebNode_End* InNode) 
{
	SEdNode_BrainWebNode::Construct(SEdNode_BrainWebNode::FArguments(), InNode);
}


bool SEdNode_BrainWebNode_End::IsNameReadOnly() const
{
	return true;
}
#undef LOCTEXT_NAMESPACE
