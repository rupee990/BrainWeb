#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "Graph/Editor/Nodes/BaseNode/SEdNode_BrainWebNode.h"

class SEditableTextBlock;
class SMultiLineEditableTextBox;

class SEdNode_BrainWebNode_End : public SEdNode_BrainWebNode
{

	SLATE_BEGIN_ARGS(SEdNode_BrainWebNode_End) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, class UEdNode_BrainWebNode_End* InNode);

	virtual bool IsNameReadOnly() const override;
};
