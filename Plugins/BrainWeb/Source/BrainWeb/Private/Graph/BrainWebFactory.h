#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "BrainWebFactory.generated.h"

UCLASS()
class BRAINWEB_API UBrainWebFactory : public UFactory
{
	GENERATED_BODY()

public:
	UBrainWebFactory();
	virtual ~UBrainWebFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
