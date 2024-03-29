#pragma once

#include "CoreMinimal.h"
#include "Settings_BrainWebEditor.generated.h"

UENUM(BlueprintType)
enum class EAutoLayoutStrategy : uint8
{
	Tree,
	ForceDirected,
};

UCLASS()
class UBrainWebEditorSettings : public UObject
{
	GENERATED_BODY()

public:
	UBrainWebEditorSettings();
	virtual ~UBrainWebEditorSettings();

	UPROPERTY(EditDefaultsOnly, Category = "AutoArrange")
	float OptimalDistance;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	EAutoLayoutStrategy AutoLayoutStrategy;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	int32 MaxIteration;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	bool bFirstPassOnly;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	bool bRandomInit;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	float InitTemperature;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	float CoolDownRate;
};
