// Copyright 2019-2024 WemadeXR Inc. All rights reserved.


#include "WRGamePlayStatics.h"
#include "Table/Base/WRTableManager.h"
#include "Manager/WRMapManager.h"

UWRGamePlayStatics::UWRGamePlayStatics(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

// bool UWRGamePlayStatics::GoToMap(EWRMapType InMapType, int32 InTID)
// {
// 	switch (InMapType)
// 	{
// 	case EWRMapType::Field:
// 	{
// 
// 		WRMapManager::Get()->LoadLevelByTable(InTID, WRTableManager::Get()->FindRow<FWRMapFieldTable>(InTID));
// 
// 	} break;
// 	case EWRMapType::Lab:
// 	{
// 
// 		WRMapManager::Get()->LoadLevelByTable(InTID, WRTableManager::Get()->FindRow<FWRMapLapTable>(InTID));
// 
// 	} break;
// 	}
// 
// 	return true;
// }
