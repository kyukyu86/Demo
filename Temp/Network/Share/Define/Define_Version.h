#pragma once


//#define WXR_PROTOCOL_VERSION		1			// 프로토콜 버전

//-----------------------------------------------------
//	버전 히스토리
//	1		//	[2019/06/25] [홍길동]	: 최초 프로토콜 추가
//-----------------------------------------------------
//#define WXR_PROTOCOL_VERSION		2			// [2019/12/16] [윤승헌]	: 로그아웃 패킷 오류 수정
//#define WXR_PROTOCOL_VERSION		3			// [2019/12/18] [임성태]	: 스킬 쿨타임 에러일 경우 남은 시간설정, 에이다 워프시 워프에 대한 알림 추가
//#define WXR_PROTOCOL_VERSION		4			// [2019/12/19] [윤승헌]	: 속도 변경 알림 패킷. 전체 브로드캐스팅 개념으로 변경
//#define WXR_PROTOCOL_VERSION		5			// [2019/12/20] [임성태]	: 플레이어 이동 시 서버를 위한 이동방향값 추가
//#define WXR_PROTOCOL_VERSION		6			// [2019/12/20] [윤승헌]	: 워프 패킷 변경
//#define WXR_PROTOCOL_VERSION		7			// [2020/01/06] [임성태]	: 플레이어/엔피씨 어피어 정보에 버프 리스트 추가
//#define WXR_PROTOCOL_VERSION		8			// [2020/01/06] [윤승헌]	: 치트 시스템 추가
//#define WXR_PROTOCOL_VERSION		9			// [2020/01/07] [윤승헌]	: 부활
//#define WXR_PROTOCOL_VERSION		10			// [2020/01/09] [윤승헌]	: 치트 처리 패킷 추가
//#define WXR_PROTOCOL_VERSION		11			// [2020/01/09] [윤승헌]	: entermap 패킷 변경
//#define WXR_PROTOCOL_VERSION		12			// [2020/01/13] [이광수]	: 캐릭터 프리셋 선택메뉴 추가/의상 교체 및 연출 패킷 변경/추가
//#define WXR_PROTOCOL_VERSION		13			// [2020/01/13] [이규태]	: NPC 대화 패킷 추가
//#define WXR_PROTOCOL_VERSION		14			// [2020/01/14] [임성태]	: 갈고리 관련 패킷 추가
//#define WXR_PROTOCOL_VERSION		15			// [2020/01/14] [임성태]	: 갈고리 관련 패킷 수정, 스킬오브젝트 관련 패킷 추가
//#define WXR_PROTOCOL_VERSION		16			// [2020/01/17] [이규태]	: NPC 대화 패킷 수정 (로밍여부 값 추가)
//#define WXR_PROTOCOL_VERSION		17			// [2020/01/20] [임성태]	: begin skill 패킷에 collision type 추가
//#define WXR_PROTOCOL_VERSION		18			// [2020/01/28] [임성태]	: 회피 또는 대시를 위한 프로토콜 추가 DynamicMove
//#define WXR_PROTOCOL_VERSION		19			// [2020/01/29] [윤승헌]	: 엘리베이터, 소셜 행동 추가
//#define WXR_PROTOCOL_VERSION		20			// [2020/01/31] [이규태]	: 아이템 관련 패킷 ISN 정보 추가
//#define WXR_PROTOCOL_VERSION		21			// [2020/02/10] [이광수]	: Network Byte Order Little Endian 으로 변경
//#define WXR_PROTOCOL_VERSION		22			// [2020/02/11] [이광수]	: 사다리 이동 기능 추가
//#define WXR_PROTOCOL_VERSION		23			// [2020/02/12] [임성태]	: Use skill 프로토콜에 맴버 추가
//#define WXR_PROTOCOL_VERSION		24			// [2020/02/18] [임성태]	: begin skill 프로토콜에 맴버 추가 attack height
//#define WXR_PROTOCOL_VERSION		25			// [2020/02/18] [이광수]	: 캐릭터 선택 작업
//#define WXR_PROTOCOL_VERSION		26			// [2020/02/19] [윤승헌]	: 플레이어 Appear 탑승체 정보 포함
//#define WXR_PROTOCOL_VERSION		27			// [2020/02/19] [이규태]	: 댄스 시스템 관련 패킷 추가
//#define WXR_PROTOCOL_VERSION		28			// [2020/03/06] [이광수]	: 앉기 상호작용 시스템 관련 패킷 추가
//#define WXR_PROTOCOL_VERSION		29			// [2020/03/09] [이광수]	: Req_ChangePreset_U2G : 해당 패킷에 bool 변수 추가 하여 해당 변수가 true 이면 해당 Actor 가 Disappear 와 Appear 가 다시 될 수 있도록 수정
//#define WXR_PROTOCOL_VERSION		30			// [2020/03/09] [임성태]	: 추락(낙사) 관련 패킷 추가
//#define WXR_PROTOCOL_VERSION		31			// [2020/03/09] [임성태]	: SyncHMD 관련 패킷 추가
//#define WXR_PROTOCOL_VERSION		32			// [2020/03/11] [임성태]	: 추락(낙사) 관련 패킷 수정
//#define WXR_PROTOCOL_VERSION		33			// [2020/03/11] [이광수]	: 아이템 메인, 서브, 장착슬롯 추가 (상호작용관련)
//#define WXR_PROTOCOL_VERSION		34			// [2020/03/12] [이광수]	: 오브젝트 들기 상호작용 관련 Inf_PlayerAppear_G2U 패킷 변경
//#define WXR_PROTOCOL_VERSION		35			// [2020/03/12] [이광수]	: 오브젝트 들기 상호작용 관련 Inf_ChangeEquipAppear_G2U 패킷 변경
//#define WXR_PROTOCOL_VERSION		36			// [2020/03/13] [윤승헌]	: 그룹이동 관련 패킷 추가
//#define WXR_PROTOCOL_VERSION		37			// [2020/03/13] [윤승헌]	: 건배제의 관련 패킷 추가
//#define WXR_PROTOCOL_VERSION		38			// [2020/03/16] [이규태]	: 말풍선 시스템 관련 패킷 추가
//#define WXR_PROTOCOL_VERSION		39			// [2020/03/17] [이광수]	: 건배제의 관련 패킷 추가
//#define WXR_PROTOCOL_VERSION		40			// [2020/03/17] [이광수]	: 건배제의 관련 패킷(Inf_CancelCheers_G2U) 변경
//#define WXR_PROTOCOL_VERSION		41			// [2020/03/17] [이광수]	: 건배제의 관련 패킷(Inf_SuggestCheers_G2U, Inf_AnswerCheers_G2U, Inf_BeginCheers_G2U, Inf_CancelCheers_G2U) 변경
//#define WXR_PROTOCOL_VERSION		42			// [2020/03/19] [임성태]	: 음성인식 명령 패킷 추가, 플레이어의 BattleState 관련 동기화 패킷 추가
//#define WXR_PROTOCOL_VERSION		43			// [2020/03/24] [임성태]	: hit result 패킷에 collision gauge percent 추가
//#define WXR_PROTOCOL_VERSION		44			// [2020/04/06] [임성태]	: Req_SuccessToShield_U2G 패킷에 shieldSkillTID 추가
//#define WXR_PROTOCOL_VERSION		45			// [2020/04/16] [임성태]	: 전투패킷 정리- inf 추가 ack 멤버 정리
//#define WXR_PROTOCOL_VERSION		46			// [2020/04/17] [임성태]	: 전투패킷 정리- inf 추가 ack 멤버 정리
//#define WXR_PROTOCOL_VERSION		47			// [2020/04/17] [윤승헌]	: 무빙워크 패킷 추가 및 관련 항목 수정
//#define WXR_PROTOCOL_VERSION		48			// [2020/04/22] [윤승헌]	: 탑승체 내에서 이동
//#define WXR_PROTOCOL_VERSION		49			// [2020/04/22] [임성태]	: use skill 에 rotate 추가
//#define WXR_PROTOCOL_VERSION		50			// [2020/04/22] [윤승헌]	: 탑승체 내에서 이동 관련 패킷 수정
//#define WXR_PROTOCOL_VERSION		51			// [2020/04/27] [윤승헌]	: 탑승체 내에서 이동 관련으로 인한 appear 패킷 수정
//#define WXR_PROTOCOL_VERSION		52			// [2020/04/28] [윤승헌]	: 탑승체 내에서 이동 관련으로 인한 로컬 좌표 관련 수정
//#define WXR_PROTOCOL_VERSION		53			// [2020/05/06] [이광수]	: 게임내 음성채팅 클라이언트 지원 작업으로 패킷 추가 및 처리
//#define WXR_PROTOCOL_VERSION		54			// [2020/05/07] [이광수]	: 게임내 음성채팅 클라이언트 지원 작업으로 패킷 변경 및 처리
//#define WXR_PROTOCOL_VERSION		55			// [2020/06/04] [윤승헌]	: 무빙워크 동기화
//#define WXR_PROTOCOL_VERSION		56			// [2020/06/09] [이규태]	: 골드 자료형 변경 int64=>int32
//#define WXR_PROTOCOL_VERSION		57			// [2020/06/10] [이규태]	: 재화패킷 추가
//#define WXR_PROTOCOL_VERSION		58			// [2020/06/12] [임성태]	: 탑승체 관련 패킷 추가
//#define WXR_PROTOCOL_VERSION		59			// [2020/06/15] [임성태]	: 음성인식 명령어 관련 시작 대사 출력 여부 패킷에 추가
//#define WXR_PROTOCOL_VERSION		60			// [2020/06/16] [임성태]	: 탑승체 관련 패킷들 이름 수정
//#define WXR_PROTOCOL_VERSION		61			// [2020/06/26] [임성태]	: 구글 번역 관련 코드 및 프로토콜 추가
//#define WXR_PROTOCOL_VERSION		62			// [2020/06/29] [임성태]	: 탑승체 관련 패킷들 이름 수정
//#define WXR_PROTOCOL_VERSION		63			// [2020/06/30] [임성태]	: 탑승체 관련 패킷들 이름 수정
//#define WXR_PROTOCOL_VERSION		64			// [2020/06/30] [이광수]	: 캐릭터 커스터마이즈 관련 작업으로 패킷 변경 및 처리
//#define WXR_PROTOCOL_VERSION		65			// [2020/07/02] [임성태]	: 탑승체 관련 패킷 추가
//#define WXR_PROTOCOL_VERSION		66			// [2020/07/03] [임성태]	: 탑승체 관련 패킷 수정
//#define WXR_PROTOCOL_VERSION		67			// [2020/07/03] [임성태]	: 탑승체 관련 패킷 추가
//#define WXR_PROTOCOL_VERSION		68			// [2020/07/07] [이규태]	: 보이스 메세지 패킷 포커스 대상 변수 추가
//#define WXR_PROTOCOL_VERSION		69			// [2020/07/07] [이규태]	: 인벤토리/아이템 패킷 추가
//#define WXR_PROTOCOL_VERSION		70			// [2020/07/08] [이규태]	: 보이스 챗 패킷 포커스 대상 변수 추가
//#define WXR_PROTOCOL_VERSION		71			// [2020/07/09] [임성태]	: 탑승체 관련 프로토콜 추가, 객체 추가, 비밀대화 관련 프로토콜 추가
//#define WXR_PROTOCOL_VERSION		72			// [2020/07/09] [이규태]	: 인벤토리/아이템 패킷 멤버 수정
//#define WXR_PROTOCOL_VERSION		73			// [2020/07/13] [임성태]	: 비밀대화 관련 프로토콜 추가
//#define WXR_PROTOCOL_VERSION		74			// [2020/07/14] [임성태]	: 비밀대화 관련 프로토콜 추가
//#define WXR_PROTOCOL_VERSION		75			// [2020/07/15] [임성태]	: 비밀대화 관련 프로토콜 추가
//#define WXR_PROTOCOL_VERSION		76			// [2020/07/15] [이규태]	: 일상 이벤트 관련 프로토콜 추가
//#define WXR_PROTOCOL_VERSION		77			// [2020/07/16] [임성태]	: 일상 이벤트 관련 프로토콜 추가
//#define WXR_PROTOCOL_VERSION		78			// [2020/07/16] [윤승헌]	: 맵 이동 흐름 변경
//#define WXR_PROTOCOL_VERSION		79			// [2020/07/20] [임성태]	: 청자 중심 번역
//#define WXR_PROTOCOL_VERSION		80			// [2020/07/21] [임성태]	: 탑승체 소환 관련 프로토콜 수정 및 탐승체 소환해제 추가
//#define WXR_PROTOCOL_VERSION		81			// [2020/07/22] [임성태]	: 청자 중심 번역
//#define WXR_PROTOCOL_VERSION		82			// [2020/07/22] [임성태]	: 청자 중심 번역
//#define WXR_PROTOCOL_VERSION		83			// [2020/07/22] [임성태]	: 청자 중심 번역 - player appear에 language type  추가
//#define WXR_PROTOCOL_VERSION		84			// [2020/07/29] [임성태]	: 화상 통화 관련 프로토콜 추가
//#define WXR_PROTOCOL_VERSION		85			// [2020/07/31] [임성태]	: 화상 통화 관련 프로토콜 정리, 자동차 브레이크에 대한 프로토콜명 수정
//#define WXR_PROTOCOL_VERSION		86			// [2020/08/04] [임성태]	: 마이크 에니메이션 프로토콜 추가
//#define WXR_PROTOCOL_VERSION		87			// [2020/08/04] [임성태]	: 마이크 에니메이션 프로토콜 수정
//#define WXR_PROTOCOL_VERSION		88			// [2020/08/05] [이규태]	: EntryInfo 패킷 추가
//#define WXR_PROTOCOL_VERSION		89			// [2020/08/05] [임성태]	: 자동이동을 위한 이동 요청 패킷 추가
//#define WXR_PROTOCOL_VERSION		90			// [2020/08/05] [임성태]	: 비디오 채팅 번역 패킷 추가
//#define WXR_PROTOCOL_VERSION		91			// [2020/08/06] [이규태]	: 세카이 시스템 추가
//#define WXR_PROTOCOL_VERSION		92			// [2020/08/19] [이규태]	: 소셜 패킷 타겟 추가 및 인게임 진입 시 세카이 정보 전달
//#define WXR_PROTOCOL_VERSION		93			// [2020/08/19] [임성태]	: 보이스 메시지/채팅의 forcusedAID 관련 수정사항 처리, 패킷 수정/ 패킷 추가
//#define WXR_PROTOCOL_VERSION		94			// [2020/08/31] [임성태]	: 비디오체팅시 코스튬 리스트도 보내도록 추가
//#define WXR_PROTOCOL_VERSION		95			// [2020/09/14] [임성태]	: Req_CreatePlayer_U2G 패킷 변경
#define WXR_PROTOCOL_VERSION		96			// [2020/10/08] [이규태]	: 브로드캐스트 테스트용 패킷 추가