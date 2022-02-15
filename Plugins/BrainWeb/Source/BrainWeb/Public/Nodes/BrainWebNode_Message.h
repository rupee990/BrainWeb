#pragma once

#include "CoreMinimal.h"

#include "BrainWebNode.h"
#include "BrainWebNode_Message.generated.h"

class UBrainWebGraph;
class UBrainWebEdge;

UCLASS(Blueprintable)
class BRAINWEB_API UBrainWebNode_Message : public UBrainWebNode
{

	GENERATED_BODY()

public:
	UBrainWebNode_Message();
	virtual ~UBrainWebNode_Message();
	virtual UBrainWebNode* GetChildNode(int32 Id) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor TextColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	int32 FontSize = 12;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TypeSpeed = 0.05f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DelayBeforeTransfer = 1.0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutoNextMesage = true;
};
