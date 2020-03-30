#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph_BrainWeb.generated.h"

class UBrainWebGraph;
class UBrainWebNode;
class UBrainWebEdge;
class UEdNode_BrainWebNode;
class UEdNode_BrainWebEdge;

UCLASS()
class UEdGraph_BrainWeb : public UEdGraph
{
	GENERATED_BODY()

public:
	UEdGraph_BrainWeb();
	virtual ~UEdGraph_BrainWeb();

	virtual void RebuildGenericGraph();

	UBrainWebGraph* GetGenericGraph() const;

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void PostEditUndo() override;

	UPROPERTY(Transient)
	TMap<UBrainWebNode*, UEdNode_BrainWebNode*> NodeMap;

	UPROPERTY(Transient)
	TMap<UBrainWebEdge*, UEdNode_BrainWebEdge*> EdgeMap;

protected:
	void Clear();

	void SortNodes(UBrainWebNode* RootNode);
};
