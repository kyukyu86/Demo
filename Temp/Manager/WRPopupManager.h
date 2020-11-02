// Copyright 2019-2024 WemadeXR Inc. All rights reserved.
// [ 2019-8-16 : kyu ] Created

#pragma once

#include "SingletonClass/WRSingleton.h"

#include "CoreMinimal.h"

#include "UI/Panel/WRUIPanel_Popup.h"
#include "Struct/WRUIStruct.h"

class UWRUIPanel_IOPopup;
class UWRWidgetComponentBase;
/**
*
*/
class WR_API WRPopupManager : public WRSingleton<WRPopupManager>
{
	CDECLARE_MANAGER(WRPopupManager);
	
private:
	WRPopupManager();
	virtual ~WRPopupManager();
	
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
	
public:
	// [ 2019-8-16 : kyu ] Create 3D	
	void OpenPopup(FWRPopupInfo& IN InInfo, FTransform& IN InTransform);
	void OpenPopupOk(FTransform& IN InTransform, FString IN InContents, FOnPopupButtonDelegate IN InFirstDelegate, EPopupOptionFlag IN InOptionFlag = EPopupOptionFlag::None);
	void OpenPopupOkCancel(FTransform& IN InTransform, FString IN InContents, FOnPopupButtonDelegate IN InFirstDelegate = nullptr , FOnPopupButtonDelegate IN InSecondDelegate = nullptr, EPopupOptionFlag IN InOptionFlag = EPopupOptionFlag::None);
	void OpenPopupScreen(FWRPopupInfo& IN InInfo);
	// [ 2019-9-4 : kyu ] Async Open Widget After
	void OnCompletedOpenPopup(const struct FWRCombinedPanelData& IN InPanelData, const FWRPopupInfo IN InInfo);

	
	// [ 2019-8-16 : kyu ] Delete
 	void ClosePopup(const WRUIHandle IN InUIHandle, bool OUT Out2D = false);
	void ClosePopup();
 	void ClosePopupAll();


	// [ 2019-8-16 : kyu ] Find
	UWRUIPanel_Popup* FindPopupWidget(const WRUIHandle IN InUIHandle);
 	class UWRWidgetComponentBase* FindPopupWidgetComponent(const WRUIHandle IN InUIHandle);


	// [ 2019-8-16 : kyu ] Test Event
	void EventCalled();
	void EventCall_Test_3DPopup();
	void FocusOnUI(class UWidget* IN InWidget);
	void FocusOnGame();


	FTransform GetMyFrontTransform();
};

