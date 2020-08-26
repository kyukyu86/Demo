// Fill out your copyright notice in the Description page of Project Settings.


#include "DMUIPanel_BitMaskTest.h"

void UDMUIPanel_BitMaskTest::NativeConstruct()
{
	Super::NativeConstruct();

	bool bCorrect = false;

	//	if (Flags1 & ETestFlag1::A)					// ������ ���� (��ġ�ϴ� & ������ ����)
	//	if ((ETestFlag1)Flags1 & ETestFlag1::A)		// ������ ���� (��ġ�ϴ� & ������ ����)
	if (Flags1 & (int32)ETestFlag1::A)
	{
		bCorrect = true;
	}
	ETestFlag1 Temp1;
	//	Temp1 |= ETestFlag1::A;								// ������ ���� (����� ���� ������ �Լ� �ʿ�)
	Temp1 = ETestFlag1::None;
	//	Temp1 = ETestFlag1::B | ETestFlag1::C;				// ������ ���� (��ġ�ϴ� | ������ ����)
	//	if ((Temp1 & ETestFlag1::B) != ETestFlag1::None)	// ������ ���� (��ġ�ϴ� & ������ ����)
	{
		bCorrect = true;
	}



	//	if (Flags2 & ETestFlag2::A)							// ������ ���� (��ġ�ϴ� & ������ ����)
	//	if ((ETestFlag2)Flags2 & ETestFlag2::A)				// ������ ���� (��ġ�ϴ� & ������ ����)
	if (Flags2 & (int32)ETestFlag2::A)
	{
		bCorrect = true;
	}
	ETestFlag2 Temp2;
	Temp2 = ETestFlag2::None;
	Temp2 |= ETestFlag2::B;
	Temp2 = ETestFlag2::B | ETestFlag2::C;
	//	if (Temp2 & ETestFlag2::B)							// ������ ���� (bool�� ��ȯ�� �� �ִ� �����̿����Ѵ�)
	if ((Temp2 & ETestFlag2::B) != ETestFlag2::None)
	{
		bCorrect = true;
	}



	//	if (Flags3 & ETestFlag3::A)							// ������ ���� (��ġ�ϴ� & ������ ����)
	//	if ((ETestFlag3)Flags3 & ETestFlag3::A)				// ������ ���� (��ġ�ϴ� & ������ ����)
	if (Flags3 & (int32)ETestFlag3::A)
	{
		bCorrect = true;
	}
	ETestFlag3 Temp3;
	Temp3 = ETestFlag3::None;
	Temp3 |= ETestFlag3::B;
	Temp3 = ETestFlag3::B | ETestFlag3::C;
	//	if (Temp3 & ETestFlag3::B)							// ������ ���� (bool�� ��ȯ�� �� �ִ� �����̿����Ѵ�)
	if ((Temp3 & ETestFlag3::B) != ETestFlag3::None)
	{
		bCorrect = true;
	}



	//	if (Flags4 & ETestFlag4::A)							// ������ ���� (��ġ�ϴ� & ������ ����)
	//	if ((ETestFlag4)Flags4 & ETestFlag4::A)				// ������ ���� (��ġ�ϴ� & ������ ����)
	if (Flags4 & (int32)ETestFlag4::A)
	{
		bCorrect = true;
	}
	ETestFlag4 Temp4;
	Temp4 = ETestFlag4::None;
	Temp4 |= ETestFlag4::B;
	Temp4 = ETestFlag4::B | ETestFlag4::C;
	//	if (Temp4 & ETestFlag4::B)							// ������ ���� (bool�� ��ȯ�� �� �ִ� �����̿����Ѵ�)
	if ((Temp4 & ETestFlag4::B) != ETestFlag4::None)
	{
		bCorrect = true;
	}
	if ((int32)(Temp4 & ETestFlag4::B) > 0)
	{
		bCorrect = true;
	}


	ETestFlag5 Temp5;
	Temp5 = ETestFlag5::None;
	Temp5 |= ETestFlag5::B;
	Temp5 = ETestFlag5::B | ETestFlag5::C;
	if ((Temp5 & ETestFlag5::B) != ETestFlag5::None)
	{
		bCorrect = true;
	}
	if ((int32)(Temp5 & ETestFlag5::B) > 0)
	{
		bCorrect = true;
	}


	ETestBitFlagForBP TempForBP = ETestBitFlagForBP::A;
	ETestBitFlag::en TempFlag = ETestBitFlag::A;
}

void UDMUIPanel_BitMaskTest::NativeDestruct()
{
	Super::NativeDestruct();
}
