#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "EdNode_BrainWebEdge.generated.h"

class UBrainWebNode;
class UBrainWebEdge;
class UEdNode_BrainWebNode;

UCLASS(MinimalAPI)
class UEdNode_BrainWebEdge : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class UEdGraph* Graph;

	UPROPERTY(VisibleAnywhere, Instanced, Category = "BrainWeb")
	UBrainWebEdge* GenericGraphEdge;

	void SetEdge(UBrainWebEdge* Edge);

	virtual void AllocateDefaultPins() override;

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	virtual void PrepareForCopying() override;

	virtual UEdGraphPin* GetInputPin() const { return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const { return Pins[1]; }

	void CreateConnections(UEdNode_BrainWebNode* Start, UEdNode_BrainWebNode* End);

	UEdNode_BrainWebNode* GetStartNode();
	UEdNode_BrainWebNode* GetEndNode();
};
