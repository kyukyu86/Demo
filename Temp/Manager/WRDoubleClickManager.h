// Copyright 2019-2024 WemadeXR Inc. All rights reserved.

#pragma once

#include "SingletonClass/WRSingleton.h"
#include "Utility/WRUIUtility.h"
#include "Enum/EWRCharacter.h"

/**
*
*/

class WR_API WRDoubleClickManager : public WRSingleton<WRDoubleClickManager>
{
	CDECLARE_MANAGER(WRDoubleClickManager);
	
private:
	WRDoubleClickManager();
	virtual ~WRDoubleClickManager();
	
public:
	//====================================================================================
	// [ 2019-8-14 : hansang88 ] �Ŵ��� ���� ������ Initialize ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnInitialize()			override;

	// [ 2019-8-14 : hansang88 ] ���� ���� ������ Shutdown ȣ�� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnShutdown()			override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� ���� ���� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelStart()		override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� ���� ���� �Ϸ� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-8-14 : hansang88 ] ���� ���� �� �������� ���۵� �� ȣ��Ǵ� �̺�Ʈ
	virtual void OnRenderStart() 		override;

	virtual bool OnTick(float InDeltaTime) override;
	//====================================================================================

public:
	void OnInputPressed(const EWRInput IN InInput);
	void OnInputReleased(const EWRInput IN InInput);
private:
	void OnClicked(const EWRInput IN InInput);
	void OnDoubleClicked(const EWRInput IN InInput);
	void OnPressHolded(const EWRInput IN InInput);

	void StartPressHoding(const EWRInput IN InInput);
	void StopPressHoding();
private:
	EWRInput BeforeInputForDoubleClick = EWRInput::None;	// �ٷ����� �Էµ� Ű(�ش簪�� ����Ŭ���� ���� ó���� ���� ������ **�ݹ� �ʱ�ȭ** �ȴ�)
	bool bFlowDoubleClickTime = false;
	float FlowDoubleClickTime = 0.f;

	EWRInput InputForPressHolding = EWRInput::None;
	float FlowPressHoldingTime = 0.f;
};