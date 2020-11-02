#pragma once


enum class EWRReasonForThrowTemporaryItemAway : uint8
{
	None, 
	MainWeaponEquip,
	SubWeaponEquip,
	ThrowOutManual,
	Damage,
	Ladder,
	Dance,
	ChangeEquip,
	IOOccupy,
};

enum class EWRClassificationForTrade : uint8
{
	None,
	Purchase,
	Sell,
};