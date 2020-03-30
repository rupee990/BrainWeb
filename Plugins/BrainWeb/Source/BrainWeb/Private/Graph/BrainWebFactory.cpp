#include "BrainWebFactory.h"
#include "BrainWebGraph.h"

#define LOCTEXT_NAMESPACE "BrainWebGraphFactory"

UBrainWebFactory::UBrainWebFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UBrainWebGraph::StaticClass();
}

UBrainWebFactory::~UBrainWebFactory()
{

}

UObject* UBrainWebFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UObject>(InParent, Class, Name, Flags | RF_Transactional);
}

#undef LOCTEXT_NAMESPACE
