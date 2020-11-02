#include "WRTriggerNode_Box.h"
#include "Utility/WRTriggerUtility.h"
#include "Actor/Character/WRCharacter.h"
#include "Enum/EWRTriggerEnum.h"

WRTriggerNode_Box::WRTriggerNode_Box()//(int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType) :
//	WRTriggerNodeBase(InUid, InNodeName, InNodeType)
{

}

WRTriggerNode_Box::~WRTriggerNode_Box()
{

}

void WRTriggerNode_Box::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerNodeBase::ImportDataFromJson(InJsonObject);

	FString Buffer;
	FVector BoxExtend;
	(*InJsonObject)->TryGetStringField("boxextent", Buffer);	WRTriggerUtility::InitFromString(Buffer, BoxExtend);

	BoxShape = FBox::BuildAABB(Position, BoxExtend*Scale);

	(*InJsonObject)->TryGetBoolField("self_disable", bSelfDisable);
	(*InJsonObject)->TryGetNumberField("quest_tid", QuestTid);
	(*InJsonObject)->TryGetNumberField("save_num", SaveNum);
	(*InJsonObject)->TryGetBoolField("entering_actor_reset", bEnteringActorReset);

	FString Temp;
	if((*InJsonObject)->TryGetStringField("condition_type", Temp) == true)
		ConditionType = EWRTriggerConditionType::GetEnum(Temp);
	
	(*InJsonObject)->TryGetNumberField("condition_value", ConditionValue);

	if (SaveNum != 0)
	{
		if (WRTriggerManager::Get()->IsUsedTriggerData(SaveNum) == true)
			bEnable = false;
	}
}

void WRTriggerNode_Box::OnEnter(const uint64 InUniqueID/* = -1*/)
{
	WRTriggerNodeBase::OnEnter(InUniqueID);
	if (bSelfDisable == true)
	{
		bUsed = true;
	}
}

// void WRTriggerNode_Box::OnEtc()
// {
// 	WRTriggerNodeBase::OnEtc();
// }
// 
// void WRTriggerNode_Box::OnEnter()
// {
// 	WRTriggerNodeBase::OnEnter();
// }
// 
// void WRTriggerNode_Box::OnExit()
// {
// 	WRTriggerNodeBase::OnExit();
// }
// 
// void WRTriggerNode_Box::OnActive()
// {
// 	WRTriggerNodeBase::OnActive();
// }
// 
// void WRTriggerNode_Box::OnDeactive()
// {
// 	WRTriggerNodeBase::OnDeactive();
// }
// 
// void WRTriggerNode_Box::OnEnable()
// {
// 	WRTriggerNodeBase::OnEnable();
// }
// 
// void WRTriggerNode_Box::OnDisable()
// {
// 	WRTriggerNodeBase::OnDisable();
// }

bool WRTriggerNode_Box::IsIntersect(AWRCharacter* InCharacter)
{
	if (InCharacter == nullptr)
		return false;

	// [ 2019-9-23 : hansang88 ] 상현임시 Range 값 임시
	FSphere TargetSphere = FSphere(InCharacter->GetActorLocation(), 50.f);
	TargetSphere.Center -= Position;
	FRotator ReverseRot = FRotator::ZeroRotator - Rotation;
	TargetSphere.Center = ReverseRot.RotateVector(TargetSphere.Center);
	TargetSphere.Center += Position;

	bool bIntersection = FMath::SphereAABBIntersection(TargetSphere, BoxShape);
	if (bIntersection == true)
	{
		EnteringActor = InCharacter;
	}
	else
	{
		EnteringActor = nullptr;
	}

	return bIntersection;
}


FVector WRTriggerNode_Box::GetRandomPositionFromShape(const EWRTriggerRandomPosLockAxis::Type& LockAxis)
{
	// [2017-1-18 : magedoga]  Get Random Position in FBox
	FVector ResultVector = FMath::RandPointInBox(BoxShape);

	FVector BoxCenter = BoxShape.GetCenter();
	// [2017-1-18 : magedoga] Lock XAxis
	if (LockAxis & EWRTriggerRandomPosLockAxis::XAxis)
	{
		ResultVector.X = BoxCenter.X;
	}

	// [2017-1-18 : magedoga] Lock YAxis
	if (LockAxis & EWRTriggerRandomPosLockAxis::YAxis)
	{
		ResultVector.Y = BoxCenter.Y;
	}

	// [2017-1-18 : magedoga] Lock ZAxis
	if (LockAxis & EWRTriggerRandomPosLockAxis::ZAxis)
	{
		ResultVector.Z = BoxCenter.Z;
	}

	return ResultVector;
}

void WRTriggerNode_Box::EnteringActorReset(bool bForced/* = false*/)
{
	if (bEnteringActorReset || bForced)
	{
		EnteringActor = nullptr;
	}
}

void WRTriggerNode_Box::EnteringActorResetCheckUsed()
{
	if(bUsed == false)
		EnteringActor = nullptr;
}

#ifdef WITH_EDITOR
void WRTriggerNode_Box::DrawTriggerShape()
{
	// [ 2019-9-23 : hansang88 ] 상현임시 임시로 주석
	//UKismetSystemLibrary::DrawDebugBox(UIMGameInstance::GetInstance(), BoxShape.GetCenter(), BoxShape.GetExtent(), FLinearColor::Blue, Rotation, 0.02f, 1.0f);
}
#endif // WITH_EDITOR

