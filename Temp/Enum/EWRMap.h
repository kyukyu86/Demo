#pragma once

// 
// UENUM(BlueprintType)
// enum class EWRMapType : uint8
// {
// 	None,			// ����
// 	Lab,			// 1.������
// 	Field,			// 2.�߼��ʵ�
// 	Senario,
// 	Stage,
// 	Town,
// 	MAX,
// };
// 
// // [ 2019-10-28 : magedoga ] ���Ӹ�嵵 ������ ���� �����̶� �ش� ���Ͽ� �߰�
 UENUM(BlueprintType)
 enum class EWRGameMode : uint8
 {
	Start		UMETA(DisplayName = "WRGameModeStart"),
 	Lab			UMETA(DisplayName = "WRGameModeLab"),
 	Field		UMETA(DisplayName = "WRGameModeField"),
 	Town		UMETA(DisplayName = "WRGameModeTown"),
 };


