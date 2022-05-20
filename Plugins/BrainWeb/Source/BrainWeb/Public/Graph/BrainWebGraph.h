#pragma once

#include "CoreMinimal.h"
#include "Nodes/BrainWebNode.h"
#include "GameplayTagContainer.h"
#include "Nodes/BrainWebNode_Start.h"

#include "BrainWebGraph.generated.h"

class UBrainWebNode_Start;
class UBrainWebNode_End;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueStart, UBrainWebNode_Start*, StartNode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueEnd, UBrainWebNode_End*, EndNode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDialogueNodeExecute, UBrainWebNode*, Node, UClass*, Class);

UCLASS(Blueprintable)
class BRAINWEB_API UBrainWebGraph : public UObject
{
	GENERATED_BODY()

public:
	UBrainWebGraph();
	virtual ~UBrainWebGraph();

	virtual TSubclassOf<UBrainWebNode> GetNodeType(int32 Index);
	
	UPROPERTY(EditDefaultsOnly, Category = "BrainWebGraph")
	FString Name;

	UPROPERTY(EditDefaultsOnly, Category = "BrainWebGraph")
	TArray<TSubclassOf<UBrainWebNode>> NodeTypes;

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

	UFUNCTION(BlueprintPure, Category = "BrainWebGraph")
	UBrainWebNode* GetStartNodeByIndex(int32 ID, bool& bWasFound);

	UFUNCTION(BlueprintCallable)
	void NextNode();
	
	UFUNCTION(BlueprintCallable)
	void SetNextNode(UBrainWebNode* NextNode);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnNodeEnter(UBrainWebNode* Node);
	
	UFUNCTION(BlueprintCallable)
	void OnNodeExit(UBrainWebNode* Node);

	UFUNCTION(BlueprintCallable)
	void StartDialogue(int32 DialogueIndex);

	UFUNCTION(BlueprintNativeEvent)
	void OnDialogStart(UBrainWebNode_Start* StartNode);

	UFUNCTION(BlueprintNativeEvent)
	void OnDialogEnd(UBrainWebNode_End* EndNode);

	UFUNCTION(BlueprintImplementableEvent)
	void OnNodeExecute();

	UFUNCTION(BlueprintCallable)
	void EndDialogue();
protected:
	UPROPERTY(BlueprintAssignable)
	FOnDialogueStart OnDialogueStartDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnDialogueEnd OnDialogueEndDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnDialogueNodeExecute OnNodeExecuteDelegate;
	
	UPROPERTY(BlueprintReadWrite)
	UBrainWebNode_Start* Start = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UBrainWebNode_Start* End = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UBrainWebNode* CurrentNode = nullptr;

	UPROPERTY(BlueprintReadOnly)
	UBrainWebNode* LastNode;
	
};
