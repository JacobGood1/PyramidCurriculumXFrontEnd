/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#pragma once

#include "Modules/ModuleInterface.h"
#include "eos_sdk.h"
#include "eos_auth_types.h"
#include "eos_friends_types.h"
#include "eos_userinfo_types.h"
#include "eos_presence_types.h"
#include "eos_connect_types.h"
#include "eos_sessions_types.h"
#include "eos_stats_types.h"
#include "eos_leaderboards_types.h"
#include "eos_metrics_types.h"
#include "eos_achievements_types.h"
#include "eos_p2p_types.h"
#include "eos_ecom_types.h"
#include "EOSCoreModule.generated.h"

class FOnlineSubsystemEOS;

class FEOSCoreModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override { return true; }
public:
	static FEOSCoreModule* Get() { return FModuleManager::GetModulePtr<FEOSCoreModule>(FName("EOSCore")); }
};

UCLASS()
class EOSCORE_API UEOSCoreSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override {}
	virtual void Deinitialize() override {}
	virtual bool ShouldCreateSubsystem(UObject* Outer) const { return true; }
public:
	static FOnlineSubsystemEOS* GetSubsystem(UObject* worldContextObject);
	static EOS_HPlatform GetPlatform(UObject* worldContextObject);
	static EOS_HAuth GetAuthHandle(UObject* worldContextObject);
	static EOS_HFriends GetFriendsHandle(UObject* worldContextObject);
	static EOS_HUserInfo GetUserHandle(UObject* worldContextObject);
	static EOS_HPresence GetPresenceHandle(UObject* worldContextObject);
	static EOS_HConnect GetConnectHandle(UObject* worldContextObject);
	static EOS_HSessions GetSessionsHandle(UObject* worldContextObject);
	static EOS_HStats GetStatsHandle(UObject* worldContextObject);
	static EOS_HLeaderboards GetLeaderboardsHandle(UObject* worldContextObject);
	static EOS_HMetrics GetMetricsHandle(UObject* worldContextObject);
	static EOS_HAchievements GetAchievementsHandle(UObject* worldContextObject);
	static EOS_HP2P GetP2PHandle(UObject* worldContextObject);
	static EOS_HEcom GetEcomHandle(UObject* worldContextObject);
	static EOS_HLobby GetLobbyHandle(UObject* worldContextObject);
	static EOS_HPlayerDataStorage GetPlayerDataStorageHandle(UObject* worldContextObject);
	static EOS_HTitleStorage GetTitleStorageHandle(UObject* worldContextObject);
	static EOS_HUI GetUiHandle(UObject* worldContextObject);
	static EOS_HUserInfo GetUserInfoHandle(UObject* worldContextObject);
	static EOS_HMods GetModsHandle(UObject* worldContextObject);
};