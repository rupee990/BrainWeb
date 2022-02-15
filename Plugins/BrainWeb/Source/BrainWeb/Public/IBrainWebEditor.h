#pragma once


DECLARE_LOG_CATEGORY_EXTERN(BrainWebEditor, Log, All);

class IBrainWebEditor : public IModuleInterface
{

public:

	static IBrainWebEditor& Get()
	{
		return FModuleManager::LoadModuleChecked< IBrainWebEditor >( "BrainWebEditor" );
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded( "BrainWebEditor" );
	}
};

