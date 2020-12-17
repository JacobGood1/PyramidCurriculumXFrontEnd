/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "OnlineSubsystemEOS.h"
#include "OnlineIdentityEOS.h"
#include "OnlineSessionEOS.h"
#include "OnlineStatsEOS.h"
#include "OnlineLeaderboardsEOS.h"
#include "OnlineAchievementsEOS.h"
#include "OnlineStoreEOS.h"
#include "OnlineUserEOS.h"
#include "SocketSubsystemEOS.h"
#include "OnlineSubsystemModuleEOS.h"
#include "OnlineSubsystemEOSLogging.h"
#include "OnlineSubsystemEOSPrivatePCH.h"

#if PLATFORM_ANDROID
#include "Android/eos_android.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"

extern JavaVM* GJavaVM;
#endif

#if WITH_EDITOR
#include "Editor.h"
#endif
#include "EOSSettings.h"

DECLARE_CYCLE_STAT(TEXT("Tick"), STAT_EOS_Tick, STATGROUP_EOS);

#define LOCTEXT_NAMESPACE "EOS"

typedef struct _InternalData
{
	int32_t ApiVersion;
	const char* BackendEnvironment;
} InternalData;

void EOS_CALL EOSFree(void* Ptr)
{
	FMemory::Free(Ptr);
}

void* EOS_CALL EOSMalloc(size_t Size, size_t Alignment)
{
	return FMemory::Malloc(Size, Alignment);
}

void* EOS_CALL EOSRealloc(void* Ptr, size_t Size, size_t Alignment)
{
	return FMemory::Realloc(Ptr, Size, Alignment);
}

void EOS_CALL EOSLog(const EOS_LogMessage* InMsg)
{
	if (GLog == nullptr)
	{
		return;
	}

	switch (InMsg->Level)
	{
		case EOS_ELogLevel::EOS_LOG_Fatal:
		{
			UE_LOG_ONLINE(Fatal, TEXT("EOSSDK-%s: %s"), ANSI_TO_TCHAR(InMsg->Category), ANSI_TO_TCHAR(InMsg->Message));
			break;
		}
		case EOS_ELogLevel::EOS_LOG_Error:
		{
			UE_LOG_ONLINE(Error, TEXT("EOSSDK-%s: %s"), ANSI_TO_TCHAR(InMsg->Category), ANSI_TO_TCHAR(InMsg->Message));
			break;
		}
		case EOS_ELogLevel::EOS_LOG_Warning:
		{
			UE_LOG_ONLINE(Warning, TEXT("EOSSDK-%s: %s"), ANSI_TO_TCHAR(InMsg->Category), ANSI_TO_TCHAR(InMsg->Message));
			break;
		}
		case EOS_ELogLevel::EOS_LOG_Verbose:
		{
			UE_LOG_ONLINE(Verbose, TEXT("EOSSDK-%s: %s"), ANSI_TO_TCHAR(InMsg->Category), ANSI_TO_TCHAR(InMsg->Message));
			break;
		}
		case EOS_ELogLevel::EOS_LOG_VeryVerbose:
		{
			UE_LOG_ONLINE(VeryVerbose, TEXT("EOSSDK-%s: %s"), ANSI_TO_TCHAR(InMsg->Category), ANSI_TO_TCHAR(InMsg->Message));
			break;
		}
		case EOS_ELogLevel::EOS_LOG_Info:
		default:
		{
			UE_LOG_ONLINE(Log, TEXT("EOSSDK-%s: %s"), ANSI_TO_TCHAR(InMsg->Category), ANSI_TO_TCHAR(InMsg->Message));
			break;
		}
	}
}

#define EOS_ENCRYPTION_KEY_MAX_LENGTH 64
#define EOS_ENCRYPTION_KEY_MAX_BUFFER_LEN (EOS_ENCRYPTION_KEY_MAX_LENGTH + 1)

struct FEOSPlatformOptions : public EOS_Platform_Options
{
public:
	FEOSPlatformOptions() 
		: EOS_Platform_Options()
	{
		ApiVersion = EOS_PLATFORM_OPTIONS_API_LATEST;
		ProductId = ProductIdAnsi;
		SandboxId = SandboxIdAnsi;
		DeploymentId = DeploymentIdAnsi;
		ClientCredentials.ClientId = ClientIdAnsi;
		ClientCredentials.ClientSecret = ClientSecretAnsi;
		CacheDirectory = CacheDirectoryAnsi;
		EncryptionKey = EncryptionKeyAnsi;
	}

	char ClientIdAnsi[EOS_OSS_STRING_BUFFER_LENGTH];
	char ClientSecretAnsi[EOS_OSS_STRING_BUFFER_LENGTH];
	char ProductIdAnsi[EOS_OSS_STRING_BUFFER_LENGTH];
	char SandboxIdAnsi[EOS_OSS_STRING_BUFFER_LENGTH];
	char DeploymentIdAnsi[EOS_OSS_STRING_BUFFER_LENGTH];
	char CacheDirectoryAnsi[EOS_OSS_STRING_BUFFER_LENGTH];
	char EncryptionKeyAnsi[EOS_ENCRYPTION_KEY_MAX_BUFFER_LEN];
};

static char GProductNameAnsi[EOS_PRODUCTNAME_MAX_BUFFER_LEN];
static char GProductVersionAnsi[EOS_PRODUCTVERSION_MAX_BUFFER_LEN];
FString ProductName;
FString ProductVersion;

EOS_PlatformHandle* GEOSPlatformHandle = NULL;

bool FOnlineSubsystemEOS::ModuleInit()
{
	EOS_InitializeOptions SDKOptions = { };
	SDKOptions.ApiVersion = EOS_INITIALIZE_API_LATEST;
	ProductName = FApp::GetProjectName();
	FCStringAnsi::Strncpy(GProductNameAnsi, TCHAR_TO_UTF8(*ProductName), EOS_PRODUCTNAME_MAX_BUFFER_LEN);
	SDKOptions.ProductName = GProductNameAnsi;
	ProductVersion = FNetworkVersion::GetProjectVersion();

	if (ProductVersion.IsEmpty())
	{
		ProductVersion = TEXT("Unknown");
	}

	FCStringAnsi::Strncpy(GProductVersionAnsi, TCHAR_TO_UTF8(*ProductVersion), EOS_PRODUCTVERSION_MAX_BUFFER_LEN);
	SDKOptions.ProductVersion = GProductVersionAnsi;
	SDKOptions.AllocateMemoryFunction = &EOSMalloc;
	SDKOptions.ReallocateMemoryFunction = &EOSRealloc;
	SDKOptions.ReleaseMemoryFunction = &EOSFree;

#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JavaVM* VM = nullptr;
	Env->GetJavaVM(&VM);
	static EOS_Android_InitializeOptions m_AndroidOptions = { 0 };
	m_AndroidOptions.ApiVersion = EOS_ANDROID_INITIALIZEOPTIONS_API_LATEST;
	m_AndroidOptions.VM = VM;

	/*m_AndroidOptions.OptionalInternalDirectory = androidPath;
	m_AndroidOptions.OptionalExternalDirectory = androidPath;*/
	SDKOptions.SystemInitializeOptions = &m_AndroidOptions;
#endif

	EOS_EResult InitResult = EOS_Initialize(&SDKOptions);
	if (InitResult != EOS_EResult::EOS_Success)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to initialize the EOS SDK with result code (%s)"), ANSI_TO_TCHAR(EOS_EResult_ToString(InitResult)));
		return false;
	}
#if !UE_BUILD_SHIPPING
	InitResult = EOS_Logging_SetCallback(&EOSLog);
	if (InitResult != EOS_EResult::EOS_Success)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init logging with result code %s"), ANSI_TO_TCHAR(EOS_EResult_ToString(InitResult)));
	}
	EOS_Logging_SetLogLevel(EOS_ELogCategory::EOS_LC_ALL_CATEGORIES, UE_BUILD_DEBUG ? EOS_ELogLevel::EOS_LOG_Verbose : EOS_ELogLevel::EOS_LOG_Info);
#endif

	if (IsRunningGame() || IsRunningDedicatedServer())
	{
		GEOSPlatformHandle = FOnlineSubsystemEOS::PlatformCreate();
	}

	return true;
}

EOS_PlatformHandle* FOnlineSubsystemEOS::PlatformCreate()
{
	FEOSPlatformOptions PlatformOptions;
	PlatformOptions.Reserved = nullptr;
	FString CacheDir;

	if (FString(FPlatformProcess::UserDir()).Len() > 0)
	{
		CacheDir = FPlatformProcess::UserDir() / FString("CacheDir");
	}
	else
	{
		LogWarning("CacheDir: %s invalid, attempting to use Project Saved Dir..", *CacheDir);

		CacheDir = FPaths::ProjectSavedDir() / FString("CacheDir");
	}

	FString OverrideCacheDir;
	GConfig->GetString(INI_SECTION, TEXT("OverrideCacheDir"), OverrideCacheDir, GEngineIni);

	if (OverrideCacheDir.Len() > 0)
	{
		LogVerbose("Overriding CacheDir: %s", *OverrideCacheDir);

		CacheDir = OverrideCacheDir;
	}

	LogVerbose("Using CacheDir: %s", *CacheDir);

#if PLATFORM_ANDROID
	extern FString GFilePathBase;
	CacheDir = GFilePathBase + FString("/UE4Game/") + FApp::GetProjectName() + "/" + FApp::GetProjectName() + FString("/");
#endif

	FCStringAnsi::Strncpy(PlatformOptions.CacheDirectoryAnsi, TCHAR_TO_UTF8(*CacheDir), EOS_OSS_STRING_BUFFER_LENGTH);

	//PlatformOptions.bIsServer = IsRunningDedicatedServer() ? EOS_TRUE : EOS_FALSE;

	uint64 OverlayFlags = 0;
	PlatformOptions.Flags = IsRunningGame() ? OverlayFlags : EOS_PF_DISABLE_OVERLAY;

	FString ConfigName;
	FParse::Value(FCommandLine::Get(), TEXT("EOSConfig="), ConfigName);

	FEOSConfig EOSConfig;
	if (!UEOSCoreSettings::GetEOSConfig(ConfigName, EOSConfig))
	{
		UE_LOG_ONLINE(Error, TEXT("PlatformCreate() failed to find configuration: (%s)"), *ConfigName);

		return nullptr;
	}

	FCStringAnsi::Strncpy(PlatformOptions.ClientIdAnsi, TCHAR_TO_UTF8(*EOSConfig.ClientId), EOS_OSS_STRING_BUFFER_LENGTH);
	FCStringAnsi::Strncpy(PlatformOptions.ClientSecretAnsi, TCHAR_TO_UTF8(*EOSConfig.ClientSecret), EOS_OSS_STRING_BUFFER_LENGTH);
	FCStringAnsi::Strncpy(PlatformOptions.ProductIdAnsi, TCHAR_TO_UTF8(*EOSConfig.ProductId), EOS_OSS_STRING_BUFFER_LENGTH);
	FCStringAnsi::Strncpy(PlatformOptions.SandboxIdAnsi, TCHAR_TO_UTF8(*EOSConfig.SandboxId), EOS_OSS_STRING_BUFFER_LENGTH);
	FCStringAnsi::Strncpy(PlatformOptions.DeploymentIdAnsi, TCHAR_TO_UTF8(*EOSConfig.DeploymentId), EOS_OSS_STRING_BUFFER_LENGTH);
	FCStringAnsi::Strncpy(PlatformOptions.EncryptionKeyAnsi, TCHAR_TO_UTF8(*EOSConfig.EncryptionKey), EOS_ENCRYPTION_KEY_MAX_BUFFER_LEN);

	if (EOSConfig.ProductId.Len() == 0)
	{
		LogError("EOSCore missing ProductId in DefaultEngine.ini");
	}

	if (EOSConfig.SandboxId.Len() == 0)
	{
		LogError("EOSCore missing SandboxId in DefaultEngine.ini");
	}

	if (EOSConfig.DeploymentId.Len() == 0)
	{
		LogError("EOSCore missing DeploymentId in DefaultEngine.ini");
	}

	if (EOSConfig.ClientId.Len() == 0)
	{
		LogError("EOSCore missing ClientId in DefaultEngine.ini");
	}

	if (EOSConfig.ClientSecret.Len() == 0)
	{
		LogError("EOSCore missing ClientSecret in DefaultEngine.ini");
	}

	if (EOSConfig.EncryptionKey.Len() == 0)
	{
		LogError("EOSCore missing EncryptKey in DefaultEngine.ini");
	}

	int32 TickBudgetInMilliseconds = 0;
	GConfig->GetInt(INI_SECTION, TEXT("TickBudgetInMilliseconds"), TickBudgetInMilliseconds, GEngineIni);

	PlatformOptions.TickBudgetInMilliseconds = TickBudgetInMilliseconds;

	EOS_PlatformHandle* EOSPlatformHandle = EOS_Platform_Create(&PlatformOptions);

	if (EOSPlatformHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS::PlatformCreate() failed to init EOS platform"));
		return nullptr;
	}

	return EOSPlatformHandle;
}

bool FOnlineSubsystemEOS::IsEnabled() const
{
	bool bEnableSubsystem = FOnlineSubsystemImpl::IsEnabled();

	if (bEnableSubsystem)
	{
		if (!GConfig->GetBool(TEXT("/Script/EOSCore.EOSCoreSettings"), TEXT("bEnabled"), bEnableSubsystem, GEngineIni))
		{
			GConfig->GetBool(TEXT("EOSCore"), TEXT("bEnabled"), bEnableSubsystem, GEngineIni);
		}
	}

/*
	if (bEnableSubsystem)
	{
#if UE_EDITOR
	bEnableSubsystem = FParse::Param(FCommandLine::Get(), TEXT("MultiprocessOSS"));
#endif
	}*/

	return bEnableSubsystem;
}

bool FOnlineSubsystemEOS::Init()
{
	bWasLaunchedByEGS = FParse::Param(FCommandLine::Get(), TEXT("EpicPortal"));
	
	if (IsRunningGame() || IsRunningDedicatedServer())
	{
		m_EOSPlatformHandle = GEOSPlatformHandle;
	}
	else
	{
		m_EOSPlatformHandle = FOnlineSubsystemEOS::PlatformCreate();
	}

	if (m_EOSPlatformHandle == nullptr)
	{
		return false;
	}

	// Get handles for later use
	m_AuthHandle = EOS_Platform_GetAuthInterface(m_EOSPlatformHandle);
	if (m_AuthHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get auth handle"));
		return false;
	}
	m_UserInfoHandle = EOS_Platform_GetUserInfoInterface(m_EOSPlatformHandle);
	if (m_UserInfoHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get user info handle"));
		return false;
	}
	m_FriendsHandle = EOS_Platform_GetFriendsInterface(m_EOSPlatformHandle);
	if (m_FriendsHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get friends handle"));
		return false;
	}
	m_PresenceHandle = EOS_Platform_GetPresenceInterface(m_EOSPlatformHandle);
	if (m_PresenceHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get presence handle"));
		return false;
	}
	m_ConnectHandle = EOS_Platform_GetConnectInterface(m_EOSPlatformHandle);
	if (m_ConnectHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get connect handle"));
		return false;
	}
	m_SessionsHandle = EOS_Platform_GetSessionsInterface(m_EOSPlatformHandle);
	if (m_SessionsHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get sessions handle"));
		return false;
	}
	m_StatsHandle = EOS_Platform_GetStatsInterface(m_EOSPlatformHandle);
	if (m_StatsHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get stats handle"));
		return false;
	}
	m_LeaderboardsHandle = EOS_Platform_GetLeaderboardsInterface(m_EOSPlatformHandle);
	if (m_LeaderboardsHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get leaderboards handle"));
		return false;
	}
	m_MetricsHandle = EOS_Platform_GetMetricsInterface(m_EOSPlatformHandle);
	if (m_MetricsHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get metrics handle"));
		return false;
	}
	m_AchievementsHandle = EOS_Platform_GetAchievementsInterface(m_EOSPlatformHandle);
	if (m_AchievementsHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get achievements handle"));
		return false;
	}
	m_P2PHandle = EOS_Platform_GetP2PInterface(m_EOSPlatformHandle);
	if (m_P2PHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get p2p handle"));
		return false;
	}
	m_LobbyHandle = EOS_Platform_GetLobbyInterface(m_EOSPlatformHandle);
	if (m_LobbyHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get lobby handle"));
		return false;
	}
	m_PlayerDataStorageHandle = EOS_Platform_GetPlayerDataStorageInterface(m_EOSPlatformHandle);
	if (m_PlayerDataStorageHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get player data storage handle"));
		return false;
	}
	m_TitleStorageHandle = EOS_Platform_GetTitleStorageInterface(m_EOSPlatformHandle);
	if (m_TitleStorageHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get title storage handle"));
		return false;
	}
	m_UiHandle = EOS_Platform_GetUIInterface(m_EOSPlatformHandle);
	if (m_UiHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get ui handle"));
		return false;
	}
	m_ModsHandle = EOS_Platform_GetModsInterface(m_EOSPlatformHandle);
	if (m_ModsHandle == nullptr)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get mods handle"));
		//return false;
	}

	if (bWasLaunchedByEGS)
	{
		m_EcomHandle = EOS_Platform_GetEcomInterface(m_EOSPlatformHandle);

		if (m_EcomHandle == nullptr)
		{
			UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to init EOS platform, couldn't get ecom handle"));
			return false;
		}

		m_StoreInterfacePtr = MakeShareable(new FOnlineStoreEOS(this));
	}

	m_SocketSubsystem = MakeShareable(new FSocketSubsystemEOS(this));

	FString ErrorMessage;
	m_SocketSubsystem->Init(ErrorMessage);

	FCStringAnsi::Strncpy(m_ProductNameAnsi, GProductNameAnsi, EOS_PRODUCTNAME_MAX_BUFFER_LEN);
	FCStringAnsi::Strncpy(m_ProductVersionAnsi, GProductVersionAnsi, EOS_PRODUCTVERSION_MAX_BUFFER_LEN);

	m_IdentityInterfacePtr = MakeShareable(new FOnlineIdentityEOS(this));
	m_SessionInterfacePtr = MakeShareable(new FOnlineSessionEOS(this));
	m_SessionInterfacePtr->Init(TCHAR_TO_UTF8(*(ProductName + TEXT("_") + ProductVersion)));
	m_StatsInterfacePtr = MakeShareable(new FOnlineStatsEOS(this));
	m_LeaderboardsInterfacePtr = MakeShareable(new FOnlineLeaderboardsEOS(this));
	m_AchievementsInterfacePtr = MakeShareable(new FOnlineAchievementsEOS(this));

	StartTicker();

#if !UE_SERVER
	FString AuthType;
	FParse::Value(FCommandLine::Get(), TEXT("AUTH_TYPE="), AuthType); 

	if (AuthType.Len() > 0)
	{
		m_IdentityInterfacePtr->AutoLogin(0);
	}
#endif

	return true;
}

bool FOnlineSubsystemEOS::Shutdown()
{
	StopTicker();

	UE_LOG_ONLINE(VeryVerbose, TEXT("FOnlineSubsystemEOS::Shutdown()"));

	FOnlineSubsystemImpl::Shutdown();

#if !WITH_EDITOR
	EOS_EResult ShutdownResult = EOS_Shutdown();
	if (ShutdownResult != EOS_EResult::EOS_Success)
	{
		UE_LOG_ONLINE(Error, TEXT("FOnlineSubsystemEOS: failed to shutdown the EOS SDK with result code (%d)"), (int32)ShutdownResult);
	}
#endif

#define DESTRUCT_INTERFACE(Interface) \
	if (Interface.IsValid()) \
	{ \
		ensure(Interface.IsUnique()); \
		Interface = nullptr; \
	}

	// Destruct the interfaces
	DESTRUCT_INTERFACE(m_IdentityInterfacePtr);
	DESTRUCT_INTERFACE(m_SessionInterfacePtr);
	DESTRUCT_INTERFACE(m_StatsInterfacePtr);
	DESTRUCT_INTERFACE(m_LeaderboardsInterfacePtr);
	DESTRUCT_INTERFACE(m_AchievementsInterfacePtr);
	DESTRUCT_INTERFACE(m_StoreInterfacePtr);

#undef DESTRUCT_INTERFACE

	DestroyEOSSocketSubsystem();
	//EOS_Platform_Release(m_EOSPlatformHandle);

	m_EOSPlatformHandle = nullptr;
	m_AuthHandle = nullptr;
	m_FriendsHandle = nullptr;
	m_UserInfoHandle = nullptr;
	m_PresenceHandle = nullptr;
	m_ConnectHandle = nullptr;
	m_SessionsHandle = nullptr;
	m_StatsHandle = nullptr;
	m_LeaderboardsHandle = nullptr;
	m_MetricsHandle = nullptr;
	m_AchievementsHandle = nullptr;
	m_P2PHandle = nullptr;
	m_EcomHandle = nullptr;
	m_LobbyHandle = nullptr;
	m_PlayerDataStorageHandle = nullptr;
	m_TitleStorageHandle = nullptr;
	m_UiHandle = nullptr;
	m_ModsHandle = nullptr;

	return true;
}

bool FOnlineSubsystemEOS::Tick(float DeltaTime)
{
	if (!bTickerStarted)
	{
		return true;
	}

	{
		FScopeCycleCounter Scope(GET_STATID(STAT_EOS_Tick), true);
		EOS_Platform_Tick(m_EOSPlatformHandle);
	}

	if (!FOnlineSubsystemImpl::Tick(DeltaTime))
	{
		return false;
	}

	if (m_SessionInterfacePtr)
	{
		m_SessionInterfacePtr->Tick(DeltaTime);
	}

	if (m_IdentityInterfacePtr)
	{
		m_IdentityInterfacePtr->Tick(DeltaTime);
	}

	return true;
}

bool FOnlineSubsystemEOS::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	if (FOnlineSubsystemImpl::Exec(InWorld, Cmd, Ar))
	{
		return true;
	}

	bool bWasHandled = false;

	if (FParse::Command(&Cmd, TEXT("EOS")))
	{
		if (m_StoreInterfacePtr != nullptr && FParse::Command(&Cmd, TEXT("ECOM")))
		{
			bWasHandled = m_StoreInterfacePtr->HandleEcomExec(InWorld, Cmd, Ar);
		}
	}

	return bWasHandled;
}

FString FOnlineSubsystemEOS::GetAppId() const
{
	return TEXT("");
}

FText FOnlineSubsystemEOS::GetOnlineServiceName() const
{
	return NSLOCTEXT("OnlineSubsystemEOS", "OnlineServiceName", "EOS");
}

IOnlineSessionPtr FOnlineSubsystemEOS::GetSessionInterface() const
{
	return m_SessionInterfacePtr;
}

IOnlineFriendsPtr FOnlineSubsystemEOS::GetFriendsInterface() const
{
	return nullptr;
}

IOnlineSharedCloudPtr FOnlineSubsystemEOS::GetSharedCloudInterface() const
{
	UE_LOG_ONLINE(Error, TEXT("Shared Cloud Interface Requested"));
	return nullptr;
}

IOnlineUserCloudPtr FOnlineSubsystemEOS::GetUserCloudInterface() const
{
	UE_LOG_ONLINE(Error, TEXT("User Cloud Interface Requested"));
	return nullptr;
}

IOnlineEntitlementsPtr FOnlineSubsystemEOS::GetEntitlementsInterface() const
{
	UE_LOG_ONLINE(Error, TEXT("Entitlements Interface Requested"));
	return nullptr;
};

IOnlineLeaderboardsPtr FOnlineSubsystemEOS::GetLeaderboardsInterface() const
{
	return m_LeaderboardsInterfacePtr;
}

IOnlineVoicePtr FOnlineSubsystemEOS::GetVoiceInterface() const
{
	return nullptr;
}

IOnlineExternalUIPtr FOnlineSubsystemEOS::GetExternalUIInterface() const
{
	return nullptr;
}

IOnlineIdentityPtr FOnlineSubsystemEOS::GetIdentityInterface() const
{
	return m_IdentityInterfacePtr;
}

IOnlineTitleFilePtr FOnlineSubsystemEOS::GetTitleFileInterface() const
{
	UE_LOG_ONLINE(Error, TEXT("Title File Interface Requested"));
	return nullptr;
}

IOnlineStoreV2Ptr FOnlineSubsystemEOS::GetStoreV2Interface() const
{
	return m_StoreInterfacePtr;
}

IOnlinePurchasePtr FOnlineSubsystemEOS::GetPurchaseInterface() const
{
	return m_StoreInterfacePtr;
}

IOnlineAchievementsPtr FOnlineSubsystemEOS::GetAchievementsInterface() const
{
	return m_AchievementsInterfacePtr;
}

IOnlineUserPtr FOnlineSubsystemEOS::GetUserInterface() const
{
	return m_UserInterfacePtr;
}

IOnlinePresencePtr FOnlineSubsystemEOS::GetPresenceInterface() const
{
	return nullptr;
}

IOnlineStatsPtr FOnlineSubsystemEOS::GetStatsInterface() const
{
	return m_StatsInterfacePtr;
}

#undef LOCTEXT_NAMESPACE