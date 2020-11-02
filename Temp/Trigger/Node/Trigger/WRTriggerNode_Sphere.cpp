#include "WRTriggerNode_Sphere.h"
#include "Actor/Character/WRCharacter.h"



WRTriggerNode_Sphere::WRTriggerNode_Sphere()//(int32 InUid, const FString& InNodeName, EWRTriggerNodeType::en InNodeType) :
	//WRTriggerNodeBase(InUid, InNodeName, InNodeType)
{

}

WRTriggerNode_Sphere::~WRTriggerNode_Sphere()
{

}

void WRTriggerNode_Sphere::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerNodeBase::ImportDataFromJson(InJsonObject);

	SphereShape.Center = Position;
	double TempValue;
	if ((*InJsonObject)->TryGetNumberField("radius", TempValue) == false)
	{
		SphereShape.W = (float)TempValue;
	}

	(*InJsonObject)->TryGetBoolField("self_disable", bSelfDisable);
}

// void WRTriggerNode_Sphere::OnEtc()
// {
// 	WRTriggerNodeBase::OnEtc();
// }
// 
// void WRTriggerNode_Sphere::OnEnter()
// {
// 	WRTriggerNodeBase::OnEnter();
// }
// 
// void WRTriggerNode_Sphere::OnExit()
// {
// 	WRTriggerNodeBase::OnEnter();
// 	//InCharacter = nullptr;
// }
// 
// void WRTriggerNode_Sphere::OnActive()
// {
// 	WRTriggerNodeBase::OnActive();
// }
// 
// void WRTriggerNode_Sphere::OnDeactive()
// {
// 	WRTriggerNodeBase::OnDeactive();
// }
// 
// 
// void WRTriggerNode_Sphere::OnEnable()
// {
// 	WRTriggerNodeBase::OnEnable();
// }
// 
// void WRTriggerNode_Sphere::OnDisable()
// {
// 	WRTriggerNodeBase::OnDisable();
// }

bool WRTriggerNode_Sphere::IsIntersect(AWRCharacter* InCharacter)
{
	if (InCharacter == nullptr)
		return false;
	// [ 2019-9-23 : hansang88 ] 상현임시 Range 값 임시
	FSphere TargetSphere = FSphere(InCharacter->GetActorLocation(), 50.f);
	bool bIntersection = SphereShape.Intersects(TargetSphere);
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


FVector WRTriggerNode_Sphere::GetRandomPositionFromShape(const EWRTriggerRandomPosLockAxis::Type& LockAxis)
{
	FVector ResultVector;
	ResultVector.X = LockAxis & EWRTriggerRandomPosLockAxis::XAxis ? SphereShape.Center.X : FMath::RandRange(SphereShape.Center.X - SphereShape.W, SphereShape.Center.X + SphereShape.W);
	ResultVector.Y = LockAxis & EWRTriggerRandomPosLockAxis::YAxis ? SphereShape.Center.Y : FMath::RandRange(SphereShape.Center.Y - SphereShape.W, SphereShape.Center.Y + SphereShape.W);
	ResultVector.Z = LockAxis & EWRTriggerRandomPosLockAxis::ZAxis ? SphereShape.Center.Z : FMath::RandRange(SphereShape.Center.Z - SphereShape.W, SphereShape.Center.Z + SphereShape.W);
	return ResultVector;
}

#ifdef WITH_EDITOR
void WRTriggerNode_Sphere::DrawTriggerShape()
{
	// [ 2019-9-23 : hansang88 ] 상현임시 임시로 주석
	//UKismetSystemLibrary::DrawDebugSphere(UIMGameInstance::GetInstance(), SphereShape.Center, SphereShape.W, 30, FLinearColor::Blue, 0.0f, 1.0f);
}
#endif // WITH_EDITOR

