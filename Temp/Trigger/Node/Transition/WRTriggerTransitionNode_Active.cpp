#include "WRTriggerTransitionNode_Active.h"
#include "../../WRTriggerSystem.h"


WRTriggerTransitionNode_Active::WRTriggerTransitionNode_Active(/*int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType*/)//:
	//WRTriggerTransitionNodeBase(InUid, InNodeName, InNodeType)
{

}

WRTriggerTransitionNode_Active::~WRTriggerTransitionNode_Active()
{

}

bool WRTriggerTransitionNode_Active::UpdateActive(float DeltaTime, WRTriggerNodeBase* CallFrom)
{
	if (UpdateData.LoopCount == 0)
	{
		return false;
	}

	AccTime += DeltaTime;

	// [2017-1-14 : magedoga] 최초 DelayTime 체크
	if (UpdateData.bProcessedFirstTime == false && UpdateData.FirstTimeDelay > AccTime)
		return true;

	// [2017-1-14 : magedoga] 최초실행
	if (UpdateData.bProcessedFirstTime == false)
	{
		AccTime = 0.0f;
		UpdateData.bProcessedFirstTime = true;
		--UpdateData.LoopCount;
		WRTriggerTransitionNodeBase::Signal(CallFrom);
		return true;
	}

	// [2017-1-14 : magedoga] Loop실행
	if (UpdateData.LoopingDelay <= AccTime)
	{
		--UpdateData.LoopCount;
		AccTime = 0.0f;
		WRTriggerTransitionNodeBase::Signal(CallFrom);
		return true;
	}

	return true;
}

void WRTriggerTransitionNode_Active::Signal(WRTriggerNodeBase* CallFrom/*, WRTriggerBaseNode* PreNode*/)
{

	// [2017-1-14 : magedoga] System에 Active 처리관련 등록
	TriggerSystem->RegisterActiveTransition(this, CallFrom);

	// [2017-1-14 : magedoga] 데이터 초기화
	UpdateData = LoadedData;
	AccTime = 0.0f;

}

void WRTriggerTransitionNode_Active::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerTransitionNodeBase::ImportDataFromJson(InJsonObject);

	double Temp;


	if ((*InJsonObject)->TryGetNumberField("loopingdelay", Temp) == true)
	{
		LoadedData.LoopingDelay = (float)Temp;
	}

	if ((*InJsonObject)->TryGetNumberField("firsttimedelay", Temp) == true)
	{
		LoadedData.FirstTimeDelay = (float)Temp;
	}

	(*InJsonObject)->TryGetNumberField("loopcount", LoadedData.LoopCount);
}

// void WRTriggerTransitionNode_Active::SendSignalToTriggerNode(WRTriggerNodeBase* InTriggerNode)
// {
// 	InTriggerNode->OnActive();
// }

