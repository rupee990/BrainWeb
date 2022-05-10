#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "InheritanceTreeFactory.generated.h"

UCLASS()
class INHERITANCETREE_API UInheritanceTreeFactory : public UFactory
{
	GENERATED_BODY()

public:
	UInheritanceTreeFactory();
	virtual ~UInheritanceTreeFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
