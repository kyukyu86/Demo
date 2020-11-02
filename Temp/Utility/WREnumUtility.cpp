#include "WREnumUtility.h"
#include <Class.h>
#include <UObjectGlobals.h>

int64 WREnumUtility::StringToEnum(const FString& InEnumTypeName, const FString& InString)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *InEnumTypeName, true);
	if (!enumPtr)
	{
		return 0;
	}

	return enumPtr->GetValueByNameString(InString);
}

uint64 WREnumUtility::AddBit(uint64 InFrom, uint64 InTo)
{
	return InFrom | InTo;
}

uint64 WREnumUtility::RemoveBit(uint64 InFrom, uint64 InTo)
{
	return InFrom & InTo ? InFrom ^ InTo : InFrom;
}

uint64 WREnumUtility::ClearBit()
{
	return 0;
}

bool WREnumUtility::IsBit(uint64 InFrom, uint64 InTo)
{
	return InFrom & InTo ? true : false;
}	

bool WREnumUtility::IsNone(uint64 InBit)
{
	return InBit == 0;
}	