#include "Settings_InheritanceTreeEditor.h"

UInheritanceTreeEditorSettings::UInheritanceTreeEditorSettings()
{
	AutoLayoutStrategy = ELayoutStrategy::Tree;

	bFirstPassOnly = false;

	bRandomInit = false;

	OptimalDistance = 100.f;

	MaxIteration = 50;

	InitTemperature = 10.f;

	CoolDownRate = 10.f;
}

UInheritanceTreeEditorSettings::~UInheritanceTreeEditorSettings()
{

}

