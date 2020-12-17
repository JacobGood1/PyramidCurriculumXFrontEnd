/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "OnlineSubsystemModuleEOS.h"
#include "OnlineSubsystemModule.h"
#include "OnlineSubsystemNames.h"
#include "OnlineSubsystemEOS.h"
#include "OnlineSubsystemEOSLogging.h"
#include "OnlineSubsystemEOSPrivatePCH.h"
#include "OnlineSubsystemTypesEOS.h"
#include "OnlineSessionEOS.h"
#include "InternetAddrEOS.h"
#include <GameFramework/PlayerController.h>

#define LOCTEXT_NAMESPACE "EOS"

DEFINE_LOG_CATEGORY(LogEOSCoreSubsystem);

IMPLEMENT_MODULE(FOnlineSubsystemEOSModule, OnlineSubsystemEOS);

class FOnlineFactoryEOS : public IOnlineFactory
{
public:
	FOnlineFactoryEOS() {}
	virtual ~FOnlineFactoryEOS() {}

public:
	virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName)
	{
		FOnlineSubsystemEOSPtr OnlineSub = MakeShared<FOnlineSubsystemEOS, ESPMode::ThreadSafe>(InstanceName);

		if (OnlineSub && OnlineSub->IsEnabled())
		{
			if (!OnlineSub->Init())
			{
				LogError("EOS SUBSYSTEM failed to initialize!");
				OnlineSub->Shutdown();
				OnlineSub = nullptr;
			}
		}
		else
		{
			LogVerbose("EOS SUBSYSTEM is not enabled, disabling OSS.");

			if (OnlineSub.IsValid())
			{
				OnlineSub->Shutdown();
				OnlineSub = nullptr;
			}
		}

		return OnlineSub;
	}
};

void FOnlineSubsystemEOSModule::StartupModule()
{
	m_EOSFactory = new FOnlineFactoryEOS();

	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.RegisterPlatformService(EOS_SUBSYSTEM, m_EOSFactory);

	FString BaseDir = IPluginManager::Get().FindPlugin("EOSCore")->GetBaseDir();

	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/EOSLibrary/Bin/EOSSDK-Win64-Shipping.dll"));
#elif PLATFORM_LINUX
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/EOSLibrary/Bin/libEOSSDK-Linux-Shipping.so"));
#elif PLATFORM_MAC
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/EOSLibrary/Bin/libEOSSDK-Mac-Shipping.dylib"));
#elif PLATFORM_ANDROID
	LibraryPath = FString("libEOSSDK.so");
#endif

	m_EOSHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	FString EOSCorePluginVersion = IPluginManager::Get().FindPlugin("EOSCore")->GetDescriptor().VersionName;

	if (!m_EOSHandle)
	{
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load EOS third party library"));
	}
	else
	{
		LogVerbose("EOS SDK Loaded! Core Version: %s", *EOSCorePluginVersion);
	}

	FOnlineSubsystemEOS::ModuleInit();
}

void FOnlineSubsystemEOSModule::ShutdownModule()
{
	FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
	OSS.UnregisterPlatformService(EOS_SUBSYSTEM);

	delete m_EOSFactory;
	m_EOSFactory = nullptr;
}

#undef LOCTEXT_NAMESPACE

static FDelegateHandle JoinSessionCallbackDelegate;
static FOnSessionUserInviteAcceptedDelegate OnSessionUserInviteAcceptedDelegate;
static FDelegateHandle OnSessionUserInviteAcceptedDelegateHandle;

void UEOSSubsystemLibrary::ListenForSessionInvites(APlayerController* playerController, const FOnSessionInviteCallback& callback)
{
	if (playerController)
	{
		if (FOnlineSubsystemEOS* m_Subsystem = static_cast<FOnlineSubsystemEOS*>(Online::GetSubsystem(GEngine->GetWorldFromContextObject(playerController, EGetWorldErrorMode::ReturnNull), EOS_SUBSYSTEM)))
		{
			if (m_Subsystem->m_SessionInterfacePtr)
			{
				OnSessionUserInviteAcceptedDelegate = FOnSessionUserInviteAcceptedDelegate::CreateLambda([m_Subsystem, callback, playerController](bool bWasSuccessful, int32 ControllerId, TSharedPtr<const FUniqueNetId> UserId, const FOnlineSessionSearchResult& SearchResult)
				{
					if (SearchResult.IsValid())
					{
						FBlueprintSessionResult BlueprintResult;
						BlueprintResult.OnlineResult = SearchResult;

						callback.ExecuteIfBound(true, playerController, BlueprintResult);
					}
					else
					{
						callback.ExecuteIfBound(false, playerController, FBlueprintSessionResult());
					}
				});

				OnSessionUserInviteAcceptedDelegateHandle = m_Subsystem->m_SessionInterfacePtr->AddOnSessionUserInviteAcceptedDelegate_Handle(OnSessionUserInviteAcceptedDelegate);
			}
		}
	}
}

void UEOSSubsystemLibrary::StopListeningForSessionInvites(APlayerController* playerController)
{
	if (playerController)
	{
		if (FOnlineSubsystemEOS* m_Subsystem = static_cast<FOnlineSubsystemEOS*>(Online::GetSubsystem(GEngine->GetWorldFromContextObject(playerController, EGetWorldErrorMode::ReturnNull), EOS_SUBSYSTEM)))
		{
			if (m_Subsystem->m_SessionInterfacePtr)
			{
				m_Subsystem->m_SessionInterfacePtr->ClearOnSessionUserInviteAcceptedDelegate_Handle(OnSessionUserInviteAcceptedDelegateHandle);
			}
		}
	}
}