#include "InheritanceTreeFactory.h"
#include "Graph/InheritanceTreeGraph.h"

#define LOCTEXT_NAMESPACE "InheritanceTreeGraphFactory"

UInheritanceTreeFactory::UInheritanceTreeFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UInheritanceTreeGraph::StaticClass();
}

UInheritanceTreeFactory::~UInheritanceTreeFactory()
{

}

UObject* UInheritanceTreeFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UObject>(InParent, Class, Name, Flags | RF_Transactional);
}

#undef LOCTEXT_NAMESPACE
