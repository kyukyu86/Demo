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
	// [ 2019-6-14 : magedoga ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnInitialize()			override;

	// [ 2019-6-14 : magedoga ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnShutdown()			override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelStart()		override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-6-14 : magedoga ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
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