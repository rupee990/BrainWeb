#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph_BrainWeb.generated.h"

class UBrainWebGraph;
class UBrainWebNode;
class UEdNode_BrainWebNode;

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
	TMap<UObject*, UEdGraphNode*> NodeMap;

protected:
	void Clear();

	void SortNodes(UBrainWebNode* RootNode);
};
