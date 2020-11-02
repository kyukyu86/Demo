#pragma once

UENUM(BlueprintType)
enum class EWRStringTableName : uint8
{
	None,
	ClientErrorStringTable,
	CommonStringTable,
	ItemStringTable,
	MiscStringTable,
	NetworkErrorStringTable,
	NPCStringTable,
	QuestStringTable,
	TutorialStringTable,
};