#pragma once


DECLARE_LOG_CATEGORY_EXTERN(InheritanceTreeEditor, Log, All);

class IInheritanceTreeEditor : public IModuleInterface
{

public:

	static IInheritanceTreeEditor& Get()
	{
		return FModuleManager::LoadModuleChecked< IInheritanceTreeEditor >( "InheritanceTreeEditor" );
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded( "InheritanceTreeEditor" );
	}
};

