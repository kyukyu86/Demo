// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

//Enum ∏≈≈©∑Œ
#define DeclareMapTable()\
	public:\
		typedef TMap<FString, en> _Map;\
	private:\
		static FORCEINLINE _Map* GetTable(bool IsShort)\
		{\
			static _Map MapTable; static _Map MapTableShort;\
			return IsShort ? &MapTableShort : &MapTable;\
		}\
		static FORCEINLINE void	InsertMap(en e, const FString& FullName, const FString& ShortName = "", bool bLower = true)\
		{\
			FString KeyString = FullName;\
			if (bLower)	\
				KeyString = FullName.ToLower(); \
			GetTable(false)->Add(KeyString, e);\
			if (false == ShortName.IsEmpty())\
			{\
				KeyString = ShortName; \
				if (bLower)	\
					KeyString = ShortName.ToLower();\
				GetTable(true)->Add(KeyString, e);\
			}\
		}\
	public:\
		static inline _Map* GetMap(bool bAb)\
		{\
			static bool IsInitialized = false;\
			if (false == IsInitialized)\
			{\
				BuildMap();\
				IsInitialized = true;\
			}\
			return GetTable(bAb);\
		}\
		static FORCEINLINE en GetEnum(const FString& Name, bool bLower = true)\
		{\
			FString KeyString = Name;\
			if (bLower)	\
				KeyString = Name.ToLower(); \
			_Map* mapTable = GetMap(false);\
			if (mapTable->Contains(KeyString) == false)\
				return MAX;\
			return mapTable->FindRef(Name);\
		}\
		static FORCEINLINE const FString GetStr(en e)\
		{\
			_Map* mapTable = GetMap(false);\
			for (auto It = mapTable->CreateConstIterator(); It; ++It)\
			{\
				if (e == It.Value())\
					return It.Key();\
			}\
			return TEXT("Invalid"); \
		}\
		static FORCEINLINE en GetEnumAb(const FString& Name, bool bLower = true)\
		{\
			FString KeyString = Name;\
			if (bLower)	\
				KeyString = Name.ToLower(); \
			_Map* mapTable = GetMap(true);\
			if (mapTable->Contains(KeyString) == false)\
				return MAX;\
			return mapTable->FindRef(Name);\
		}\
		static inline const FString GetStrAb(en e)\
		{\
			_Map* mapTable = GetMap(true);\
			for (auto It = mapTable->CreateConstIterator(); It; ++It)\
			{\
				if (e == It.Value())\
					return It.Key();\
			}\
			return TEXT("Invalid"); \
		}\
		static FORCEINLINE const int32 GetMax()\
		{\
		return MAX;\
		}


// 
#define DeclareMapTableWithMaxCount(MaxCount)\
	public:\
		typedef TMap<FString, en> _Map;\
	private:\
		static FORCEINLINE _Map* GetTable(bool IsShort)\
		{\
			static _Map MapTable; static _Map MapTableShort;\
			return IsShort ? &MapTableShort : &MapTable;\
		}\
		static FORCEINLINE void	InsertMap(en e, const FString& FullName, const FString& ShortName = "", bool bLower = true)\
		{\
			FString KeyString = FullName;\
			if (bLower)	\
				KeyString = FullName.ToLower(); \
			GetTable(false)->Add(KeyString, e);\
			if (false == ShortName.IsEmpty())\
			{\
				KeyString = ShortName; \
				if (bLower)	\
					KeyString = ShortName.ToLower();\
				GetTable(true)->Add(KeyString, e);\
			}\
		}\
	public:\
		static inline _Map* GetMap(bool bAb)\
		{\
			static bool IsInitialized = false;\
			if (false == IsInitialized)\
			{\
				BuildMap();\
				IsInitialized = true;\
			}\
			return GetTable(bAb);\
		}\
		static FORCEINLINE en GetEnum(const FString& Name, bool bLower = true)\
		{\
			FString KeyString = Name;\
			if (bLower)	\
				KeyString = Name.ToLower(); \
			_Map* mapTable = GetMap(false);\
			if (mapTable->Contains(KeyString) == false)\
				return MaxCount;\
			return mapTable->FindRef(Name);\
		}\
		static FORCEINLINE const FString GetStr(en e)\
		{\
			_Map* mapTable = GetMap(false);\
			for (auto It = mapTable->CreateConstIterator(); It; ++It)\
			{\
				if (e == It.Value())\
					return It.Key();\
			}\
			return TEXT("Invalid"); \
		}\
		static FORCEINLINE en GetEnumAb(const FString& Name, bool bLower = true)\
		{\
			FString KeyString = Name;\
			if (bLower)	\
				KeyString = Name.ToLower(); \
			_Map* mapTable = GetMap(true);\
			if (mapTable->Contains(KeyString) == false)\
				return MaxCount;\
			return mapTable->FindRef(Name);\
		}\
		static inline const FString GetStrAb(en e)\
		{\
			_Map* mapTable = GetMap(true);\
			for (auto It = mapTable->CreateConstIterator(); It; ++It)\
			{\
				if (e == It.Value())\
					return It.Key();\
			}\
			return TEXT("Invalid"); \
		}