// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#define USE_CLIENT_ONLY

#define USE_AUDIOMIXER_BP_LIBRARY 1

#include <string>
#include <vector>
#include <list>
#include <map>

#include "CoreUObject.h"
#include "Runtime/Networking/Public/Networking.h"
#include "Network/Share/Define/Define_Type.h"
#include "WRGlobalFunction.h"
#include "Define/WRTypeDefine.h"

DECLARE_LOG_CATEGORY_EXTERN(WRLog, Log, All);
#define DEF_REMOVE_NOT_USED_FUNCTIONS_VUVLE_200520 0
#define DEF_CONVERT_UI_CREATION_AS_ASYNCMANAGER_BY_VUVLE_200604
#define DEF_USING_NEW_CHARACTER_CREATE_BY_VUVLE_200820

#if !defined(WR_ADD_VOICE)
#define WR_ADD_VOICE 0
#endif 

#if !defined(WR_ADD_3D_VOICE)
#define WR_ADD_3D_VOICE 0
#endif


#if !defined(WR_ADD_VOICE_PORT)
#define WR_ADD_VOICE_PORT 0
#endif

#if !defined(WR_ADD_WORLDCOMPOSITION_ENABLE_UPDATESTATE)
	#define WR_ADD_WORLDCOMPOSITION_ENABLE_UPDATESTATE 0
#endif // WR_ADD_WORLDCOMPOSITION_ENABLE_UPDATESTATE


#if !defined(WR_FIX_WORLDCOMPOSITION_RESCAN)
	#define WR_FIX_WORLDCOMPOSITION_RESCAN 0
#endif // WR_FIX_WORLDCOMPOSITION_RESCAN


#if !defined(WR_FIXED_DYNAMICALLY_CREATED_SKELETALMESH)
	#define WR_FIXED_DYNAMICALLY_CREATED_SKELETALMESH 0
#endif // WR_FIXED_DYNAMICALLY_CREATED_SKELETALMESH

#define REFACTORING_EQUIPMENT_BY_ANIMAL14_
#define REFACTORING_CHARACTER_VISIBILITY_BY_ANIMAL14_

//#define LAP_TID 10001


#define UE_4_25 1
