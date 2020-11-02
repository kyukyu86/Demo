#include "WRTriggerNode_MainAreaBox.h"
// #include "FrameWork/IMGameInstance.h"
// #include "Utility/WRTriggerUtility.h"


WRTriggerNode_MainAreaBox::WRTriggerNode_MainAreaBox()
: AreaGroupID(0)
{

}

WRTriggerNode_MainAreaBox::~WRTriggerNode_MainAreaBox()
{

}

void WRTriggerNode_MainAreaBox::ImportDataFromJson(const TSharedPtr<FJsonObject>* InJsonObject)
{
	WRTriggerNode_Box::ImportDataFromJson(InJsonObject);

	(*InJsonObject)->TryGetNumberField("area_group_id", AreaGroupID);

	(*InJsonObject)->TryGetStringField("local_key", LocalKey);

	(*InJsonObject)->TryGetBoolField("use_minimap_name", bUseMinimapName);

}

#ifdef WITH_EDITOR
void WRTriggerNode_MainAreaBox::DrawTriggerShape()
{
	// [ 2019-9-23 : hansang88 ] 상현임시 임시로 주석
	//UKismetSystemLibrary::DrawDebugBox(UIMGameInstance::GetInstance(), BoxShape.GetCenter(), BoxShape.GetExtent(), FLinearColor::Red, Rotation, 0.02f, 1.0f);
}
#endif // WITH_EDITOR

