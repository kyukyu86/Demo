#include "WRFSMManager.h"
#include "Actor/FSM/WRFSMIdle.h"
#include "Actor/FSM/WRFSMMove.h"
#include "Actor/FSM/WRFSMAttack.h"
#include "Actor/FSM/WRFSMDamage.h"
#include "Actor/FSM/WRFSMDie.h"
#include "Actor/FSM/WRFSMDead.h"
#include "Actor/FSM/WRFSMStand.h"
#include "Actor/FSM/WRFSMInteract.h"
#include "Actor/FSM/WRFSMRevival.h"
#include "Actor/FSM/WRFSMSpawn.h"
#include "Actor/FSM/WRFSMDown.h"
#include "Actor/FSM/WRFSMCC.h"
#include "Actor/FSM/WRFSMEmotion.h"
#include "Actor/FSM/WRFSMRide.h"
#include "Actor/FSM/WRFSMDismount.h"
#include "Actor/FSM/WRFSMActing.h"
#include "Actor/FSM/WRFSMDefense.h"
#include "Actor/FSM/WRFSMBeBlocked.h"
#include "Actor/FSM/WRFSMParrying.h"
#include "Actor/FSM/WRFSMSkill.h"
#include "Actor/FSM/WRFSMAvoid.h"
#include "Actor/FSM/WRFSMBeParried.h"
#include "Actor/FSM/WRFSMWeaponChange.h"
#include "Actor/FSM/WRFSMTalk.h"
#include "Actor/FSM/WRFSMSocialAnimation.h"
#include "Actor/FSM/WRFSMLadder.h"
#include "Actor/FSM/WRFSMDance.h"
#include "Actor/FSM/WRFSMMaintainingSkill.h"
#include "Actor/FSM/WRFSMDisarm.h"
#include "Actor/FSM/WRFSMArm.h"
#include "Actor/FSM/WRFSMZipline.h"
#include "Actor/FSM/WRFSMEnterVehicle.h"
#include "Actor/FSM/WRFSMExitVehicle.h"
#include "Actor/FSM/WRFSMDrivingIdle.h"
#include "Actor/FSM/WRFSMVehicleInteraction.h"
#include "Actor/FSM/WRFSMEventAction.h"
#include "Actor/FSM/WRFSMVoiceChatMotion.h"




WRFSMManager::WRFSMManager()
{
}


WRFSMManager::~WRFSMManager()
{
}

void WRFSMManager::OnInitialize()
{
	mapFSMInfo.Add(EWRFSM::Idle, new WRFSMIdle());
	mapFSMInfo.Add(EWRFSM::Move, new WRFSMMove());
	mapFSMInfo.Add(EWRFSM::Attack, new WRFSMAttack());
	mapFSMInfo.Add(EWRFSM::Skill, new WRFSMSkill());
	mapFSMInfo.Add(EWRFSM::MaintainingSkill, new WRFSMMaintainingSkill());
	mapFSMInfo.Add(EWRFSM::Damage, new WRFSMDamage());
	mapFSMInfo.Add(EWRFSM::Die, new WRFSMDie());
	mapFSMInfo.Add(EWRFSM::Dead, new WRFSMDead());
	mapFSMInfo.Add(EWRFSM::Stand, new WRFSMStand());
	mapFSMInfo.Add(EWRFSM::Interact, new WRFSMInteract());
	mapFSMInfo.Add(EWRFSM::Revival, new WRFSMRevival());
	mapFSMInfo.Add(EWRFSM::Spawn, new WRFSMSpawn());
	mapFSMInfo.Add(EWRFSM::Down, new WRFSMDown());
	mapFSMInfo.Add(EWRFSM::CC, new WRFSMCC());
	mapFSMInfo.Add(EWRFSM::Emotion, new WRFSMEmotion());
	mapFSMInfo.Add(EWRFSM::Ride, new WRFSMRide());
	mapFSMInfo.Add(EWRFSM::Dismount, new WRFSMDismount());
	mapFSMInfo.Add(EWRFSM::Acting, new WRFSMActing());
	mapFSMInfo.Add(EWRFSM::Defense, new WRFSMDefense());
	mapFSMInfo.Add(EWRFSM::BeBlocked, new WRFSMBeBlocked());
	mapFSMInfo.Add(EWRFSM::Parrying, new WRFSMParrying());
	mapFSMInfo.Add(EWRFSM::Avoiding, new WRFSMAvoid());
	mapFSMInfo.Add(EWRFSM::BeParried, new WRFSMBeParried());
	mapFSMInfo.Add(EWRFSM::WeaponChange, new WRFSMWeaponChange());
	mapFSMInfo.Add(EWRFSM::Arm, new WRFSMArm());
	mapFSMInfo.Add(EWRFSM::Disarm, new WRFSMDisarm());
	mapFSMInfo.Add(EWRFSM::Talk, new WRFSMTalk());
	mapFSMInfo.Add(EWRFSM::SocialAnimation, new WRFSMSocialAnimation());
	mapFSMInfo.Add(EWRFSM::Ladder, new WRFSMLadder());
	mapFSMInfo.Add(EWRFSM::Dance, new WRFSMDance());
	mapFSMInfo.Add(EWRFSM::Zipline, new WRFSMZipline());
	mapFSMInfo.Add(EWRFSM::EnterVehicle, new WRFSMEnterVehicle());
	mapFSMInfo.Add(EWRFSM::ExitVehicle, new WRFSMExitVehicle());
	mapFSMInfo.Add(EWRFSM::DrivingIdle, new WRFSMDrivingIdle());
	mapFSMInfo.Add(EWRFSM::VehicleInteraction, new WRFSMVehicleInteraction());
	mapFSMInfo.Add(EWRFSM::EventAction, new WRFSMEventAction());
	mapFSMInfo.Add(EWRFSM::VoiceChatMotion, new WRFSMVoiceChatMotion());

// 	mapFSMInfo.Add(EWRFSM::BeingTamed, new WRFSMBeingTamed());
// 	mapFSMInfo.Add(EWRFSM::Taming, new WRFSMTaming());
}

void WRFSMManager::OnShutdown()
{
	for (auto It = mapFSMInfo.CreateIterator(); It; ++It)
	{
		delete It.Value();
	}

	mapFSMInfo.Empty();
}

void WRFSMManager::OnLoadLevelStart()
{

}

void WRFSMManager::OnLoadLevelComplete()
{

}

void WRFSMManager::OnRenderStart()
{

}

bool WRFSMManager::OnTick(float InDeltaTime)
{
	return true;
}
