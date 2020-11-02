// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#define NULL_STRING L""

#define MAX_UNIQUE_HANDLE_LOWBIT_NUMBER		16		// [ 2019-9-10 : vuvle ] : Handle �� Generate �Ҷ� Unique Handle �� ����� ���� ��Ʈ ���� ���� (48 + 16)
#define LOHANDLE(Handle64Bit)    ((uint32)((Handle64Bit) & 0x0000ffff))
#define HIHANDLE(Handle64Bit)    ((uint64)(((Handle64Bit) >> 16) & 0xffffffffffffffff))

typedef FPlatformTypes::int32		WRTableID;
#define INVALID_TABLE_TID 0
typedef FPlatformTypes::uint64		WRUIHandle;
typedef FPlatformTypes::uint8		BitFlag8;		// [ 2019-8-27 : firefirefox ] : ��Ʈ ����� ���Ǵ� 8��Ʈ flag
typedef FPlatformTypes::uint16		BitFlag16;		// [ 2019-8-27 : vuvle ] : ��Ʈ ����� ���Ǵ� 16��Ʈ flag
typedef FPlatformTypes::uint32		BitFlag32;		// [ 2019-8-27 : vuvle ] : ��Ʈ ����� ���Ǵ� 32��Ʈ flag
typedef FPlatformTypes::uint64		BitFlag64;		// [ 2019-8-27 : vuvle ] : ��Ʈ ����� ���Ǵ� 64��Ʈ flag

// [ 2019-8-27 : vuvle ] : Character Stat ��
typedef FPlatformTypes::int32		ATK_t;
typedef FPlatformTypes::int32		DEF_t;
typedef FPlatformTypes::int32		HP_t;


typedef FPlatformTypes::int32		WRCompassMarkHandle;
#define INVALID_COMPASS_MARK_HANDLE 0
typedef FPlatformTypes::int32		WRTargetLocationGuideHandle;
#define INVALID_TARGET_LOCATION_GUIDE_HANDLE 0

static const float MAX_THROWING_SHILED_ROLL= 30.0f;
