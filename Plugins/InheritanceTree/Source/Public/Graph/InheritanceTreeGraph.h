#pragma once

#include "CoreMinimal.h"
#include "Nodes/InheritanceTreeNode.h"
#include "GameplayTagContainer.h"
#include "InheritanceTreeGraph.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDialogueNodeExecute, UInheritanceTreeNode*, Node, UClass*, Class);

class UEdNode_InheritanceTreeNode;

UCLASS(Blueprintable)
class INHERITANCETREE_API UInheritanceTreeGraph : public UObject
{
	GENERATED_BODY()

public:
	UInheritanceTreeGraph();
	virtual ~UInheritanceTreeGraph();

	virtual TSubclassOf<UInheritanceTreeNode> GetNodeType(int32 Index);
	
	UPROPERTY(EditDefaultsOnly, Category = "InheritanceTreeGraph")
	FString Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InheritanceTreeGraph")
	FGameplayTagContainer GraphTags;

	UPROPERTY(BlueprintReadOnly, Category = "InheritanceTreeGraph")
	TArray<UInheritanceTreeNode*> RootNodes;

	UPROPERTY(BlueprintReadOnly, Category = "InheritanceTreeGraph")
	TArray<UInheritanceTreeNode*> AllNodes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InheritanceTreeGraph")
	bool bEdgeEnabled;

	// Child relatives depth
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Depth = 2;

	// Show Cousin of this class and their children
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Breadth = 1;

	// Show Parent inheritance
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Height = 0;
	
	UFUNCTION(BlueprintCallable, Category = "InheritanceTreeGraph")
	void Print(bool ToConsole = true, bool ToScreen = true);

	UFUNCTION(BlueprintCallable, Category = "InheritanceTreeGraph")
	int GetLevelNum() const;

	UFUNCTION(BlueprintCallable, Category = "InheritanceTreeGraph")
	void GetNodesByLevel(int Level, TArray<UInheritanceTreeNode*>& Nodes);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "InheritanceTreeGraph")
	void BuildInheritanceGraph();

	UFUNCTION()
	bool BuildInheritance_Recursive(UClass* Class, UEdNode_InheritanceTreeNode* EdRootNode, FVector2D& Location, int32 id, bool bExit);
	
	void ClearGraph();

#if WITH_EDITORONLY_DATA
	UPROPERTY(Transient)
	class UEdGraph* EdGraph;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UObject> Source;

	UInheritanceTreeNode* GetStartNodeByIndex(int32 ID);
protected:
	
	UPROPERTY(BlueprintReadWrite)
	UInheritanceTreeNode* CurrentNode = nullptr;


	int32 MaxExit = 300;
	int32 CurrentNum = 0;

	TArray<UClass*> UsedClass;
};
