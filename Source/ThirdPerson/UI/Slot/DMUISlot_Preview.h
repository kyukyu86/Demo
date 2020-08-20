// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Base/DMUISlot.h"
#include "../../PreviewStudio/DMPreviewStruct.h"
#include "DMUISlot_Preview.generated.h"

// ENUM_CLASS_FLAGS ������ |, & ������ �������� ����
UENUM(Meta = (Bitflags))
enum class ETestFlag1 : uint8
{
	None,
	A,
	B,
	C,
};

// ENUM_CLASS_FLAGS �� ���� |, & ������ ������
UENUM(Meta = (Bitflags))
enum class ETestFlag2 : uint8
{
	None,
	A,
	B,
	C,
};
ENUM_CLASS_FLAGS(ETestFlag2)


// ��Ʈ���갪�� 32�� ������ BP���� ��µ��� �ʴ´�.
UENUM(Meta = (Bitflags))
enum class ETestFlag3 : uint8
{
	None = 0x0000,
	A = 0x0001,
	B = 0x0002,
	C = 0x0004,
};
ENUM_CLASS_FLAGS(ETestFlag3)

// ENUM_CLASS_FLAGS �� ���� |, & ������ ������
UENUM(Meta = (Bitflags))
enum class ETestFlag4
{
	None,
	A = 1 << 1,
	B = 1 << 2,
	C = 1 << 3,
	D = 1 << 4,
	E = 1 << 5,
	F = 1 << 6,
	G = 1 << 7,
	H = 1 << 8,
	I = 1 << 9,
	J = 1 << 10,
	K = 1 << 11,

	MAX = 1 << 30,
};
ENUM_CLASS_FLAGS(ETestFlag4)

UENUM(Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ETestFlag5
{
	None,
	A = 1 << 1,
	B = 1 << 2,
	C = 1 << 3,
	D = 1 << 4,
	E = 1 << 5,
	F = 1 << 6,
	G = 1 << 7,
	H = 1 << 8,
	I = 1 << 9,
	J = 1 << 10,
	K = 1 << 11,

	MAX = 1 << 30,
};
ENUM_CLASS_FLAGS(ETestFlag5)

// ��� : BP������ �ϳ� �����
UENUM(Meta = (Bitflags))
enum class ETestBitFlagForBP
{
	A,
	B,
	C,
	D,
	E,
	F,
	G,
};
ENUM_CLASS_FLAGS(ETestBitFlagForBP)

// ���� �����Ǵ� namespace enum���� ����� enum�� ���� �ڵ忡�� ������ ��
namespace ETestBitFlag 
{
	enum en
	{
		None,
		A = 1 << (int32)ETestBitFlagForBP::A,
		B = 1 << (int32)ETestBitFlagForBP::B,
		C = 1 << (int32)ETestBitFlagForBP::C,
		D = 1 << (int32)ETestBitFlagForBP::D,
		E = 1 << (int32)ETestBitFlagForBP::E,
		F = 1 << (int32)ETestBitFlagForBP::F,
		G = 1 << (int32)ETestBitFlagForBP::G,
	};
}



/**

	�� DMPreviewStudio �� ����մϴ�.

	FDMPreviewInfo ��
	- Type : EDMPreviewType
	- BPPath : String
	- SetPreviewWidget(this)
	�� �ʿ��� ������ �����ϰ�
	DMPreviewManager::Get()->SetPreview( FDMPreviewInfo ); �� ȣ���մϴ�.

	�� ������� �ʰ� �� ���� DMPreviewManager::Get()->ReleasePreview( this, true/false ); �� ȣ���Ͽ� Studio�� �����մϴ�.

	�� ����ϴ� Widget�� ����� �°� FDMPreviewBaseInfo�� SceneSize�� �����ؾ��մϴ�.

 */

UCLASS()
class THIRDPERSON_API UDMUISlot_Preview : public UDMUISlot
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FDMPreviewDataWidget	PreviewData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", Bitmask, BitmaskEnum = "ETestFlag1"))
		int32 Flags1 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", Bitmask, BitmaskEnum = "ETestFlag2"))
		int32 Flags2 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", Bitmask, BitmaskEnum = "ETestFlag3"))
		int32 Flags3 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", Bitmask, BitmaskEnum = "ETestFlag4"))
		int32 Flags4 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", Bitmask, BitmaskEnum = "ETestFlag5"))
		int32 Flags5 = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true", Bitmask, BitmaskEnum = "ETestBitFlagForBP"))
		int32 FlagsFinal = 0;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true", BindWidget))
		class UImage* Image_BindTest;

private:
	void OnExit();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);

	FORCEINLINE FDMPreviewDataWidget& GetPreviewData() { return PreviewData; }

	void OnShow();

	void OnRotateToLeft();
	void OnRotateToRight();
	void OnRotateEnd();
	void OnZoomInStart();
	void OnZoomOutStart();
	void OnZoomEnd();
};
