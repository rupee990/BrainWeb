#pragma once

#include "CoreMinimal.h"
#include "BrainWebNode.h"
#include "BrainWebEdge.generated.h"

class UBrainWebGraph;

UCLASS(Blueprintable)
class BRAINWEB_API UBrainWebEdge : public UObject
{
	GENERATED_BODY()

public:
	UBrainWebEdge();
	virtual ~UBrainWebEdge();

	UPROPERTY(VisibleAnywhere, Category = "BrainWebNode")
	UBrainWebGraph* Graph;

	UPROPERTY(BlueprintReadOnly, Category = "BrainWebEdge")
	UBrainWebNode* StartNode;

	UPROPERTY(BlueprintReadOnly, Category = "BrainWebEdge")
	UBrainWebNode* EndNode;

	UFUNCTION(BlueprintPure, Category = "BrainWebEdge")
	UBrainWebGraph* GetGraph() const;
};
