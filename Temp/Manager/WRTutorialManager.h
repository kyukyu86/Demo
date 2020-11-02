#pragma once

#include "CoreMinimal.h"
#include "SingletonClass/WRSingleton.h"
#include "Define/WRTypeDefine.h"
#include "InputCoreTypes.h"
#include "Table/WRTutorialTable.h"
#include "Tuple.h"
#include "Define/WRUIDefine.h"

#define DEF_ASYNC_LOAD_TUTORIAL_KYU

/**
*
*/
struct FWRTutorialPreConditionForm
{
	FWRTutorialPreConditionForm() :
		eCondition(EWRTutorialPreConditionType::None)
		, nValue(0)
		, ParentForm(nullptr)
	{

	}
private:
	EWRTutorialPreConditionType eCondition;
	int32 nValue;

	struct FWRTutorialForm* ParentForm;

public:
	void SetParentForm(struct FWRTutorialForm* IN InParentForm) { ParentForm = InParentForm; }
	void SetInvokePreConditionForm(const FWRTutorialInvokePreCondition& IN InCondition);
	bool CheckCondition();
};

struct FWRTutorialConditionForm
{
	FWRTutorialConditionForm() : 
		bDone(false)
		, eCondition(EWRTutorialConditionType::None)
		, nValue(0)
		, eKey(EWRInput::None)
		, nRepeatCount(0), nRepeatCountMax(0)
		, ParentForm(nullptr)
	{

	}
private:
	bool bDone;
	EWRTutorialConditionType eCondition;
	int32 nValue;
	EWRInput eKey;
	int32 nRepeatCount;
	int32 nRepeatCountMax;

	struct FWRTutorialForm* ParentForm;

public:
	void Initialize();
	void SetParentForm(struct FWRTutorialForm* IN InParentForm) { ParentForm = InParentForm; }
	void SetInvokeConditionForm(const FWRTutorialInvokeCondition& IN InCondition);
	void SetConditionForm(const FWRTutorialCondition& IN InCondition);
	bool CheckCondition(const EWRTutorialConditionType IN InConditionType, void* IN InValue, const bool IN InShowAlert);
	void OnSkipCondition();
	bool IsDone();
private:
	void OnCondition();
	FString MakeRepeatCountText();
};

struct FWRTutorialForm
{
	FWRTutorialForm() : bDone(false), TutorialTable(nullptr), UIHandle(INVALID_UI_HANDLE)
		, bAnyKeyInputToClosePopup(false)
		, fElapsedOpeningTime(0.f)
		, bTickPopupTime(false), fElapsedPopupTime(0.f) 
		, bTickLimitTime(false), fElapsedLimitTime(0.f)
		, bFromMenu(false)
	{

	}

	friend FWRTutorialConditionForm;

private:	
	bool bDone;
	FWRTutorialTable* TutorialTable;
	FWRTutorialPreConditionForm			InvokePreConditionForm;
	TArray<FWRTutorialConditionForm>	InvokeConditionForms;
	TArray<FWRTutorialConditionForm>	ConditionForms;
	WRUIHandle UIHandle;

	bool bAnyKeyInputToClosePopup = false;
	EWRTutorialState eState = EWRTutorialState::Init;
	float fElapsedOpeningTime;
	// Popup 뜨는 시간
	bool bTickPopupTime;
	float fElapsedPopupTime;
	// 튜토리얼 동작 제한시간
	bool bTickLimitTime;
	float fElapsedLimitTime;
	// 메뉴창에서 호출
	bool bFromMenu;
#ifdef DEF_ASYNC_LOAD_TUTORIAL_KYU
	// 다시 여는것
	bool bReOpen = false;
#else//DEF_ASYNC_LOAD_TUTORIAL_KYU
#endif//DEF_ASYNC_LOAD_TUTORIAL_KYU

public:	
	FWRTutorialTable* const GetTutorialTable() { return TutorialTable; }
	WRTableID GetCurTID();
	WRTableID GetNextTID();
	FString GetCompleteText();
	EWRTutorialPanel GetTutorialPanelType();
		
	FORCEINLINE EWRTutorialState GetState() { return eState; }
	FORCEINLINE void SetFromMenu(const bool IN InSet) { bFromMenu = InSet; }

	bool MakeTutorialForm(const WRTableID IN InTutorialTID, const bool IN InFromMenu = false);
	void ReleaseTutorial();
	void Initialize();

	bool IsSame(const WRTableID IN InTutorialTID);
	bool IsDone();
	bool IsConditionComplete(int32 IN InConditionIndex);
	bool IsAbleToSkip();
	bool IsFromMenu() { return bFromMenu; }
		
	void OnTick(float InDeltaTime);

#ifdef DEF_ASYNC_LOAD_TUTORIAL_KYU
	void OnOpenPopupCompleteToAsync();
#else//DEF_ASYNC_LOAD_TUTORIAL_KYU
#endif//DEF_ASYNC_LOAD_TUTORIAL_KYU
	void OnStartTutorial();
	void OnReservedTutorial();
	void OnCompleteTutorial();
	bool OnSkipTutorial();	

	bool OnInvokeReadyEvent(const EWRTutorialConditionType IN InConditionType, void* IN InValue);
	void OnEvent(const EWRTutorialConditionType IN InConditionType, void* IN InValue);	

private:
	void OnOpenPopup(const bool IN InAgain = false);
	void OnClosePopup();	// Main유지조건에서 Limit조건으로 이벤트

	void OnInvokeReadyEventAction();
	void OnEventAction();
	
	FORCEINLINE void SetState(const EWRTutorialState IN InSet) { eState = InSet; }

	void SetTutorialMainProgressBar(float IN InRate);
	void SetTutorialMainRepeatCount(FString& IN InText);
	void SetTutorialSubProgressBar(float IN InRate);
};




#define DEF_BLUR_DEFAULT_FOCAL_REGION 2000.0f
#define DEF_BLUR_DEFAULT_MAX_BOKEH_SIZE 0.3f

#define DEF_BLUR_FOCAL_REGION 0.0f
#define DEF_BLUR_MAX_BOKEH_SIZE 0.7f


class WR_API WRTutorialManager : public WRSingleton<WRTutorialManager>
{
	CDECLARE_MANAGER(WRTutorialManager);
	
	friend struct FWRTutorialForm;

private:
	WRTutorialManager();
	virtual ~WRTutorialManager();
	
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

	virtual void EndLoadLevelProcess()	override;
	//====================================================================================

	virtual bool OnTick(float InDeltaTime) override;
	
	// for Cheat
	void ClearAllTutorials();

	void OnNewTutorial(const WRTableID IN InTutorialTID, const bool IN InFromMenu = false);			// 새로 추가하는 부분	
	void OnForceNewTutorial(const WRTableID IN InTutorialTID, const bool IN InFromMenu = false);	// 강제로 앞에 추가하는 부분
	void OnSkip();	
	// Invoke Ready Event
	void OnInvokeReadyEvent(const EWRTutorialConditionType IN InConditionType, int32 IN InValue);
	void OnInvokeReadyEvent(EWRInput IN InKey);
	// Received Event
	void OnEvent(const EWRTutorialConditionType IN InConditionType);
	void OnEvent(const EWRTutorialConditionType IN InConditionType, int32 IN InValue);
	void OnEvent(EWRInput IN InKey);
		
	// Check
	bool IsDuringTutorial();
	
#ifdef DEF_ASYNC_LOAD_TUTORIAL_KYU
	void OpenTutorialMainCompleteToAsync();
#else//DEF_ASYNC_LOAD_TUTORIAL_KYU
#endif//DEF_ASYNC_LOAD_TUTORIAL_KYU
	
	// for Skip UI
	void OpenTutorialSub(const bool IN InShowSkip);
	void CloseTutorialSub();	

	// for Alert
	void OpenTutorialAlert(const float IN InTime = 0.f);
	void CloseTutorialAlert();	
	
	// Get, Set
	void SetTutorialMovable(const bool IN InSet);
	bool IsTutorialMovableBlock();

private:
	void InitializeTutorials();
	void RemoveAllTutorials();

	void AddTutorial(const WRTableID IN InTutorialTID, const bool IN InForceHead = false, const bool IN InFromMenu = false);
	void TryToTutorialStart(const bool IN InForceHead = false);
	FWRTutorialForm* GetTutorialForm(const WRTableID IN InTutorialTID);
	FWRTutorialForm* GetNextTutorial();

	bool IsReservedTutorial(const WRTableID IN InTutorialTID);
	bool IsCompletedTutorial(const WRTableID IN InTutorialTID);

	void OpenTutorialMain(FWRTutorialTable* IN InTable);
	void CloseTutorialMain(FWRTutorialTable* IN InTable);

	void SetTutorialProgressBar(FWRTutorialTable* IN InTable, float IN InRate);
	void SetTutorialRepeatCount(FWRTutorialTable* IN InTable, FString& IN InText);

	void SetBlur(const bool IN InSet);

	void OnTutorialComplete();
	void RemoveAllLinkedReservedTutorial(const WRTableID IN InTutorialTID);
	void RemoveAllLinkedCompletedTutorial(const WRTableID IN InTutorialTID);
	
private:
	TDoubleLinkedList<FWRTutorialForm*> InitTutorials;			// 초기 목록
	TDoubleLinkedList<FWRTutorialForm*> ReservedTutorials;		// 예약 목록
	TDoubleLinkedList<FWRTutorialForm*> CompletedTutorials;		// 완료 목록
	FWRTutorialForm* CurTutorial = nullptr;
	bool bStartOnMap = false;
	bool bMovableBlock = false;
};