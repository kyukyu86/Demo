#pragma once

// 
// UENUM(BlueprintType)
// enum class EWRMapType : uint8
// {
// 	None,			// 없음
// 	Lab,			// 1.연구실
// 	Field,			// 2.중세필드
// 	Senario,
// 	Stage,
// 	Town,
// 	MAX,
// };
// 
// // [ 2019-10-28 : magedoga ] 게임모드도 어차피 레벨 종속이라 해당 파일에 추가
 UENUM(BlueprintType)
 enum class EWRGameMode : uint8
 {
	Start		UMETA(DisplayName = "WRGameModeStart"),
 	Lab			UMETA(DisplayName = "WRGameModeLab"),
 	Field		UMETA(DisplayName = "WRGameModeField"),
 	Town		UMETA(DisplayName = "WRGameModeTown"),
 };


