#pragma once

#include "CoreMinimal.h"
#include "Settings_InheritanceTreeEditor.generated.h"

UENUM(BlueprintType)
  enum class ELayoutStrategy : uint8
  {
  	Tree,
  	ForceDirected
  };

UCLASS()
class UInheritanceTreeEditorSettings : public UObject
{
	GENERATED_BODY()

public:
	UInheritanceTreeEditorSettings();
	virtual ~UInheritanceTreeEditorSettings();

	UPROPERTY(EditDefaultsOnly, Category = "AutoArrange")
	float OptimalDistance;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	ELayoutStrategy AutoLayoutStrategy;

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
