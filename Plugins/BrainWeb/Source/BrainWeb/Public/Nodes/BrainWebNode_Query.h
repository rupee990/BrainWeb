#pragma once

#include "CoreMinimal.h"

#include "BrainWebNode.h"
#include "BrainWebNode_Query.generated.h"

class UBrainWebGraph;
class UBrainWebEdge;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNodeChanged);

UCLASS(Blueprintable)
class BRAINWEB_API UBrainWebNode_Query : public UBrainWebNode
{
	GENERATED_BODY()

public:
	UBrainWebNode_Query();
	virtual ~UBrainWebNode_Query();

	virtual void Execute() override;

	UFUNCTION(BlueprintCallable)
	void MoveToBranch(int32 Branch);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	FOnNodeChanged OnNodeChanged;
	
	UPROPERTY(BlueprintReadWrite)
	int32 Choice;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText QuestionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FText> Queries;
};
