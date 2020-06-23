

#include "DMPreviewStruct.h"
#include "../UI/Slot/DMUISlot_Preview.h"
#include "DMPreviewStudio.h"

void FDMPreviewInfo::SetPreviewWidget(class UDMUISlot_Preview* IN InWidget)
{
	if (InWidget == nullptr)
		return;

	PreviewWidget = InWidget;
//	BaseInfo = InWidget->GetPreviewBaseInfo();
}

void FDMPreviewInfo::Release(const bool IN InParentDestroy /*= false*/)
{
	Type = EDMPreviewType::Static;
	BPPath = "";
	Anim = nullptr;
	PreviewWidget = nullptr;
	
	if (PreviewStudio->IsValidLowLevel())
	{
		PreviewStudio->Destroy();
	}
	PreviewStudio = nullptr;

	if (InParentDestroy == false)
	{
		PreviewExitDelegate.ExecuteIfBound();
	}
	PreviewExitDelegate.Unbind();

	BaseInfo = FDMPreviewBaseInfo();
}