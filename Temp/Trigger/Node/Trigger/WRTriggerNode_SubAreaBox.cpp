#include "WRTriggerNode_SubAreaBox.h"
// #include "FrameWork/IMGameInstance.h"
// #include "Utility/WRTriggerUtility.h"


WRTriggerNode_SubAreaBox::WRTriggerNode_SubAreaBox()
	: AreaGroupID(0)
{

}

WRTriggerNode_SubAreaBox::~WRTriggerNode_SubAreaBox()
{

}

void WRTriggerNode_SubAreaBox::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerNode_Box::ImportDataFromJson(InJsonObject);

	(*InJsonObject)->TryGetNumberField("area_group_id", AreaGroupID);
}

#ifdef WITH_EDITOR
void WRTriggerNode_SubAreaBox::DrawTriggerShape()
{
	// [ 2019-9-23 : hansang88 ] 상현임시
	//UKismetSystemLibrary::DrawDebugBox(UIMGameInstance::GetInstance(), BoxShape.GetCenter(), BoxShape.GetExtent(), FLinearColor::Red, Rotation, 0.02f, 1.0f);
}
#endif // WITH_EDITOR

