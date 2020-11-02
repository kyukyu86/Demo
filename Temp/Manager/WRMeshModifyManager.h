// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SingletonClass/WRSingleton.h"
#include "Actor/Character/WRCharacter.h"

/**
*
*/
class WR_API WRMeshModifyManager : public WRSingleton<WRMeshModifyManager>
{
public:
	WRMeshModifyManager();
	virtual ~WRMeshModifyManager();
	
public:
	//====================================================================================
	// [ 2019-6-14 : magedoga ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-6-14 : magedoga ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
	virtual void OnRenderStart() 		override;
	//====================================================================================

	virtual bool OnTick(float InDeltaTime) override;

private:
	void RemovModifiedSkelMeshs();

private:
	TMap<uint32 ,AWRCharacter*> SlicedCharacters;

public:

	FORCEINLINE void AddSlicedCharacters(AWRCharacter* InCharacter) { if (InCharacter != nullptr)SlicedCharacters.Add(InCharacter->GetUniqueID(),InCharacter); }
	FORCEINLINE void RemoveSlicedCharacters(AWRCharacter* InCharacter) 
	{ 
		if (InCharacter != nullptr)
		{
			SlicedCharacters.Contains(InCharacter->GetUniqueID());
			SlicedCharacters.Remove(InCharacter->GetUniqueID());
			SlicedCharacters.Compact();
		}
	}
};