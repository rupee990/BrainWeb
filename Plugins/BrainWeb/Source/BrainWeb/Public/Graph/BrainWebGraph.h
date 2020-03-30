#pragma once

#include "CoreMinimal.h"
#include "BrainWebNode.h"
#include "BrainWebEdge.h"
#include "GameplayTagContainer.h"
#include "BrainWebGraph.generated.h"

UCLASS(Blueprintable)
class BRAINWEB_API UBrainWebGraph : public UObject
{
	GENERATED_BODY()

public:
	UBrainWebGraph();
	virtual ~UBrainWebGraph();

	UPROPERTY(EditDefaultsOnly, Category = "BrainWebGraph")
	FString Name;

	UPROPERTY(EditDefaultsOnly, Category = "BrainWebGraph")
	TSubclassOf<UBrainWebNode> NodeType;

	UPROPERTY(EditDefaultsOnly, Category = "BrainWebGraph")
	TSubclassOf<UBrainWebEdge> EdgeType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BrainWebGraph")
	FGameplayTagContainer GraphTags;

	UPROPERTY(BlueprintReadOnly, Category = "BrainWebGraph")
	TArray<UBrainWebNode*> RootNodes;

	UPROPERTY(BlueprintReadOnly, Category = "BrainWebGraph")
	TArray<UBrainWebNode*> AllNodes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BrainWebGraph")
	bool bEdgeEnabled;

	UFUNCTION(BlueprintCallable, Category = "BrainWebGraph")
	void Print(bool ToConsole = true, bool ToScreen = true);

	UFUNCTION(BlueprintCallable, Category = "BrainWebGraph")
	int GetLevelNum() const;

	UFUNCTION(BlueprintCallable, Category = "BrainWebGraph")
	void GetNodesByLevel(int Level, TArray<UBrainWebNode*>& Nodes);

	void ClearGraph();

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	class UEdGraph* EdGraph;

	UPROPERTY(EditDefaultsOnly, Category = "BrainWeb_Editor")
	bool bCanRenameNode;
#endif
};
