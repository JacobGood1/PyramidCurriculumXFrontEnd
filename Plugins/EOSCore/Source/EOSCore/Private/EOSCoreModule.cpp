/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "EOSCoreModule.h"
#include "Core/EOSCoreLibrary.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreAsync.h"
#include <OnlineSubsystem.h>

#define LOCTEXT_NAMESPACE "FEOSCoreModule"

DEFINE_LOG_CATEGORY(LogEOSCore);

IMPLEMENT_MODULE(FEOSCoreModule, EOSCore)

void FEOSCoreModule::StartupModule()
{
	
}

void FEOSCoreModule::ShutdownModule()
{

}

FOnlineSubsystemEOS* UEOSCoreSubsystem::GetSubsystem(UObject* worldContextObject)
{
	if (worldContextObject)
	{
		if (FOnlineSubsystemEOS* Subsystem = static_cast<FOnlineSubsystemEOS*>(Online::GetSubsystem(GEngine->GetWorldFromContextObject(worldContextObject, EGetWorldErrorMode::ReturnNull), EOS_SUBSYSTEM)))
		{
			return Subsystem;
		}
	}

	if (FOnlineSubsystemEOS* Subsystem = static_cast<FOnlineSubsystemEOS*>(IOnlineSubsystem::Get(EOS_SUBSYSTEM)))
	{
		return Subsystem;
	}

	return nullptr;
}

EOS_HPlatform UEOSCoreSubsystem::GetPlatform(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_EOSPlatformHandle ? Subsystem->m_EOSPlatformHandle : nullptr;
	}
	return nullptr;
}

EOS_HAuth UEOSCoreSubsystem::GetAuthHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_AuthHandle ? Subsystem->m_AuthHandle : nullptr;
	}
	return nullptr;
}

EOS_HFriends UEOSCoreSubsystem::GetFriendsHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_FriendsHandle ? Subsystem->m_FriendsHandle : nullptr;
	}
	return nullptr;
}

EOS_HUserInfo UEOSCoreSubsystem::GetUserHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_UserInfoHandle ? Subsystem->m_UserInfoHandle : nullptr;
	}
	return nullptr;
}

EOS_HPresence UEOSCoreSubsystem::GetPresenceHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_PresenceHandle ? Subsystem->m_PresenceHandle : nullptr;
	}
	return nullptr;
}

EOS_HConnect UEOSCoreSubsystem::GetConnectHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_ConnectHandle ? Subsystem->m_ConnectHandle : nullptr;
	}
	return nullptr;
}

EOS_HSessions UEOSCoreSubsystem::GetSessionsHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_SessionsHandle ? Subsystem->m_SessionsHandle : nullptr;
	}
	return nullptr;
}

EOS_HStats UEOSCoreSubsystem::GetStatsHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_StatsHandle ? Subsystem->m_StatsHandle : nullptr;
	}
	return nullptr;
}

EOS_HLeaderboards UEOSCoreSubsystem::GetLeaderboardsHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_LeaderboardsHandle ? Subsystem->m_LeaderboardsHandle : nullptr;
	}
	return nullptr;
}

EOS_HMetrics UEOSCoreSubsystem::GetMetricsHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_MetricsHandle ? Subsystem->m_MetricsHandle : nullptr;
	}
	return nullptr;
}

EOS_HAchievements UEOSCoreSubsystem::GetAchievementsHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_AchievementsHandle ? Subsystem->m_AchievementsHandle : nullptr;
	}
	return nullptr;
}

EOS_HP2P UEOSCoreSubsystem::GetP2PHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_P2PHandle ? Subsystem->m_P2PHandle : nullptr;
	}
	return nullptr;
}

EOS_HEcom UEOSCoreSubsystem::GetEcomHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_EcomHandle ? Subsystem->m_EcomHandle : nullptr;
	}
	return nullptr;
}

EOS_HLobby UEOSCoreSubsystem::GetLobbyHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_LobbyHandle ? Subsystem->m_LobbyHandle : nullptr;
	}
	return nullptr;
}

EOS_HPlayerDataStorage UEOSCoreSubsystem::GetPlayerDataStorageHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_PlayerDataStorageHandle ? Subsystem->m_PlayerDataStorageHandle : nullptr;
	}
	return nullptr;
}

EOS_HTitleStorage UEOSCoreSubsystem::GetTitleStorageHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_TitleStorageHandle ? Subsystem->m_TitleStorageHandle : nullptr;
	}
	return nullptr;
}

EOS_HUI UEOSCoreSubsystem::GetUiHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_UiHandle ? Subsystem->m_UiHandle : nullptr;
	}
	return nullptr;
}

EOS_HUserInfo UEOSCoreSubsystem::GetUserInfoHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_UserInfoHandle ? Subsystem->m_UserInfoHandle : nullptr;
	}
	return nullptr;
}

EOS_HMods UEOSCoreSubsystem::GetModsHandle(UObject* worldContextObject)
{
	if (FOnlineSubsystemEOS* Subsystem = GetSubsystem(worldContextObject))
	{
		return Subsystem->m_ModsHandle ? Subsystem->m_ModsHandle : nullptr;
	}
	return nullptr;
}

#undef LOCTEXT_NAMESPACE