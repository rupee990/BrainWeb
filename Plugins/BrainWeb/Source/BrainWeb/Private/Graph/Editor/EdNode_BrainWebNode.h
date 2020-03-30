#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "BrainWebNode.h"
#include "EdNode_BrainWebNode.generated.h"

class UEdNode_BrainWebEdge;
class UEdGraph_BrainWeb;
class SEdNode_BrainWebNode;

UCLASS(MinimalAPI)
class UEdNode_BrainWebNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UEdNode_BrainWebNode();
	virtual ~UEdNode_BrainWebNode();

	UPROPERTY(VisibleAnywhere, Instanced, Category = "BrainWeb")
	UBrainWebNode* BrainWebNode;

	void SetGenericGraphNode(UBrainWebNode* InNode);
	UEdGraph_BrainWeb* GetGenericGraphEdGraph();

	SEdNode_BrainWebNode* SEdNode;

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void PrepareForCopying() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	virtual FLinearColor GetBackgroundColor() const;
	virtual UEdGraphPin* GetInputPin() const;
	virtual UEdGraphPin* GetOutputPin() const;

#if WITH_EDITOR
	virtual void PostEditUndo() override;
#endif

};
