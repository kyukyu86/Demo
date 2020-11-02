#pragma once

UENUM(BlueprintType)
enum class EWRItemEquipState : uint8
{
	UnEquipped,							// 장착중이지 않음
	Equipped,							// 장착중
};