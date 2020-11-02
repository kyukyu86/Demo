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
	// [ 2019-8-14 : hansang88 ] 매니저 생성 시점에 Initialize 호출 시 호출되는 이벤트
	virtual void OnInitialize()			override;

	// [ 2019-8-14 : hansang88 ] 게임 종료 시점에 Shutdown 호출 시 호출되는 이벤트
	virtual void OnShutdown()			override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 레벨 변경 전 호출되는 이벤트
	virtual void OnLoadLevelStart()		override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 레벨 변경 완료 후 호출되는 이벤트
	virtual void OnLoadLevelComplete() 	override;

	// [ 2019-8-14 : hansang88 ] 레벨 변경 시 렌더링이 시작될 때 호출되는 이벤트
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
	EWRInput BeforeInputForDoubleClick = EWRInput::None;	// 바로전에 입력된 키(해당값은 더블클릭에 대한 처리를 위한 값으로 **금방 초기화** 된다)
	bool bFlowDoubleClickTime = false;
	float FlowDoubleClickTime = 0.f;

	EWRInput InputForPressHolding = EWRInput::None;
	float FlowPressHoldingTime = 0.f;
};