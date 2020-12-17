/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#pragma once

#include "CoreMinimal.h"
#include "OnlineSubsystemNames.h"
#include "OnlineSubsystemImpl.h"
#include "eos_sdk.h"

DECLARE_STATS_GROUP(TEXT("EOS"), STATGROUP_EOS, STATCAT_Advanced);

class FOnlineIdentityEOS;
typedef TSharedPtr<class FOnlineIdentityEOS, ESPMode::ThreadSafe> FOnlineIdentityEOSPtr;

class FOnlineUserEOS;
typedef TSharedPtr<class FOnlineUserEOS, ESPMode::ThreadSafe> FOnlineUserEOSPtr;

class FOnlineSessionEOS;
typedef TSharedPtr<class FOnlineSessionEOS, ESPMode::ThreadSafe> FOnlineSessionEOSPtr;

class FOnlineStatsEOS;
typedef TSharedPtr<class FOnlineStatsEOS, ESPMode::ThreadSafe> FOnlineStatsEOSPtr;

class FOnlineLeaderboardsEOS;
typedef TSharedPtr<class FOnlineLeaderboardsEOS, ESPMode::ThreadSafe> FOnlineLeaderboardsEOSPtr;

class FOnlineAchievementsEOS;
typedef TSharedPtr<class FOnlineAchievementsEOS, ESPMode::ThreadSafe> FOnlineAchievementsEOSPtr;

class FOnlineStoreEOS;
typedef TSharedPtr<class FOnlineStoreEOS, ESPMode::ThreadSafe> FOnlineStoreEOSPtr;

#ifndef EOS_PRODUCTNAME_MAX_BUFFER_LEN
	#define EOS_PRODUCTNAME_MAX_BUFFER_LEN 64
#endif

#ifndef EOS_PRODUCTVERSION_MAX_BUFFER_LEN
	#define EOS_PRODUCTVERSION_MAX_BUFFER_LEN 64
#endif

#define EOS_SUBSYSTEM FName(TEXT("EOS"))

class ONLINESUBSYSTEMEOS_API FOnlineSubsystemEOS : public FOnlineSubsystemImpl
{
public:
	virtual ~FOnlineSubsystemEOS() = default;
private:
	FOnlineSubsystemEOS() = delete;
PACKAGE_SCOPE:
	explicit FOnlineSubsystemEOS(FName InInstanceName) 
		: FOnlineSubsystemImpl(EOS_SUBSYSTEM, InInstanceName)
		, m_EOSPlatformHandle(nullptr)
		, m_AuthHandle(nullptr)
		, m_FriendsHandle(nullptr)
		, m_UserInfoHandle(nullptr)
		, m_PresenceHandle(nullptr)
		, m_ConnectHandle(nullptr)
		, m_SessionsHandle(nullptr)
		, m_StatsHandle(nullptr)
		, m_LeaderboardsHandle(nullptr)
		, m_MetricsHandle(nullptr)
		, m_AchievementsHandle(nullptr)
		, m_P2PHandle(nullptr)
		, m_EcomHandle(nullptr)
		, m_LobbyHandle(nullptr)
		, m_PlayerDataStorageHandle(nullptr)
		, m_TitleStorageHandle(nullptr)
		, m_UiHandle(nullptr)
		, m_ModsHandle(nullptr)
		, m_IdentityInterfacePtr(nullptr)
		, m_UserInterfacePtr(nullptr)
		, m_SessionInterfacePtr(nullptr)
		, m_LeaderboardsInterfacePtr(nullptr)
		, m_AchievementsInterfacePtr(nullptr)
		, m_StoreInterfacePtr(nullptr)
		, bWasLaunchedByEGS(false)
	{
		StopTicker();
	}

public:
// IOnlineSubsystem
	virtual IOnlineSessionPtr GetSessionInterface() const override;
	virtual IOnlineFriendsPtr GetFriendsInterface() const override;
	virtual IOnlineSharedCloudPtr GetSharedCloudInterface() const override;
	virtual IOnlineUserCloudPtr GetUserCloudInterface() const override;
	virtual IOnlineEntitlementsPtr GetEntitlementsInterface() const override;
	virtual IOnlineLeaderboardsPtr GetLeaderboardsInterface() const override;
	virtual IOnlineVoicePtr GetVoiceInterface() const override;
	virtual IOnlineExternalUIPtr GetExternalUIInterface() const override;	
	virtual IOnlineIdentityPtr GetIdentityInterface() const override;
	virtual IOnlineTitleFilePtr GetTitleFileInterface() const override;
	virtual IOnlineStoreV2Ptr GetStoreV2Interface() const override;
	virtual IOnlinePurchasePtr GetPurchaseInterface() const override;
	virtual IOnlineAchievementsPtr GetAchievementsInterface() const override;
	virtual IOnlineUserPtr GetUserInterface() const override;
	virtual IOnlinePresencePtr GetPresenceInterface() const override;
	virtual FText GetOnlineServiceName() const override;
	virtual IOnlineStatsPtr GetStatsInterface() const override;
	virtual bool Exec(class UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

	virtual IOnlineGroupsPtr GetGroupsInterface() const override { return nullptr; }
	virtual IOnlinePartyPtr GetPartyInterface() const override { return nullptr; }
	virtual IOnlineTimePtr GetTimeInterface() const override { return nullptr; }
	virtual IOnlineEventsPtr GetEventsInterface() const override { return nullptr; }
	virtual IOnlineSharingPtr GetSharingInterface() const override { return nullptr; }
	virtual IOnlineMessagePtr GetMessageInterface() const override { return nullptr; }
	virtual IOnlineChatPtr GetChatInterface() const override { return nullptr; }
	virtual IOnlineTurnBasedPtr GetTurnBasedInterface() const override { return nullptr; }
	virtual IOnlineTournamentPtr GetTournamentInterface() const override { return nullptr; }
//~IOnlineSubsystem

public:
	virtual bool Init() override;
	virtual bool Shutdown() override;
	virtual FString GetAppId() const override;
	static bool ModuleInit();
	static EOS_PlatformHandle* PlatformCreate();
	virtual bool IsEnabled() const override;
	virtual bool Tick(float DeltaTime) override;

PACKAGE_SCOPE:
	char m_ProductNameAnsi[EOS_PRODUCTNAME_MAX_BUFFER_LEN];
	char m_ProductVersionAnsi[EOS_PRODUCTVERSION_MAX_BUFFER_LEN];

PACKAGE_SCOPE:
	EOS_HPlatform m_EOSPlatformHandle;
	EOS_HAuth m_AuthHandle;
	EOS_HFriends m_FriendsHandle;
	EOS_HUserInfo m_UserInfoHandle;
	EOS_HPresence m_PresenceHandle;
	EOS_HConnect m_ConnectHandle;
	EOS_HSessions m_SessionsHandle;
	EOS_HStats m_StatsHandle;
	EOS_HLeaderboards m_LeaderboardsHandle;
	EOS_HMetrics m_MetricsHandle;
	EOS_HAchievements m_AchievementsHandle;
	EOS_HP2P m_P2PHandle;
	EOS_HEcom m_EcomHandle;
	EOS_HLobby m_LobbyHandle;
	EOS_HPlayerDataStorage m_PlayerDataStorageHandle;
	EOS_HTitleStorage m_TitleStorageHandle;
	EOS_HUI m_UiHandle;
	EOS_HMods m_ModsHandle;
PACKAGE_SCOPE:
	FOnlineIdentityEOSPtr m_IdentityInterfacePtr;
	FOnlineUserEOSPtr m_UserInterfacePtr;
	FOnlineSessionEOSPtr m_SessionInterfacePtr;
	FOnlineStatsEOSPtr m_StatsInterfacePtr;
	FOnlineLeaderboardsEOSPtr m_LeaderboardsInterfacePtr;
	FOnlineAchievementsEOSPtr m_AchievementsInterfacePtr;
	FOnlineStoreEOSPtr m_StoreInterfacePtr;
PACKAGE_SCOPE:
	bool bWasLaunchedByEGS;
PACKAGE_SCOPE:
	TSharedPtr<class FSocketSubsystemEOS, ESPMode::ThreadSafe> m_SocketSubsystem;
};

typedef TSharedPtr<FOnlineSubsystemEOS, ESPMode::ThreadSafe> FOnlineSubsystemEOSPtr;