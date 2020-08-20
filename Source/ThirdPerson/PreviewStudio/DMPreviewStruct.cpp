

#include "DMPreviewStruct.h"
#include "../UI/Slot/DMUISlot_Preview.h"
#include "DMPreviewStudio.h"

void FDMPreviewInfo::SetPreviewWidget(class UDMUISlot_Preview* IN InWidget)
{
	if (InWidget == nullptr)
		return;

	PreviewWidget = InWidget;
	PreviewData = InWidget->GetPreviewData();
}

void FDMPreviewInfo::Release(const bool IN InParentDestroy /*= false*/)
{
	Type = EDMPreviewType::Static;
	BPPath = "";
	Anim = nullptr;
	PreviewWidget = nullptr;

	if (PreviewStudio->IsValidLowLevel())
	{
		PreviewStudio->Release();
		PreviewStudio->Destroy();
	}
	PreviewStudio = nullptr;

	if (InParentDestroy == false)
	{
		PreviewExitDelegate.ExecuteIfBound();
	}
	PreviewExitDelegate.Unbind();

	PreviewData = FDMPreviewDataWidget();
}

void FDMPreviewInfo::ModifyBaseData(struct FDMPreviewDataBase& IN InPreviewTablData)
{
	PreviewData = InPreviewTablData;
}

void FDMPreviewCustomActorData::AddItem(const int32 IN InTID)
{
	// CustomActor 용 의상정보 등 저장
}
