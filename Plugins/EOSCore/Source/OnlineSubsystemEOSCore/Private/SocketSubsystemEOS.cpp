/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "SocketSubsystemEOS.h"
#include "InternetAddrEOS.h"
#include "SocketEOS.h"
#include "OnlineSubsystemEOS.h"
#include "OnlineIdentityEOS.h"
#include "OnlineSubsystemEOSPrivatePCH.h"

FSocketSubsystemEOS::FSocketSubsystemEOS(FOnlineSubsystemEOS* InSubsystemEOS)
	: m_Subsystem(InSubsystemEOS)
	, m_LastSocketError(ESocketErrors::SE_NO_ERROR)
{
}

FSocketSubsystemEOS::~FSocketSubsystemEOS() = default;

bool FSocketSubsystemEOS::Init(FString& Error)
{
	FSocketSubsystemModule& SocketSubsystem = FModuleManager::LoadModuleChecked<FSocketSubsystemModule>("Sockets");
	SocketSubsystem.RegisterSocketSubsystem(EOS_SUBSYSTEM, this, false);

	return true;
}

void FSocketSubsystemEOS::Shutdown()
{
	m_TrackedSockets.Reset();
}

FSocket* FSocketSubsystemEOS::CreateSocket(const FName& SocketTypeName, const FString& SocketDescription, const FName& /*unused*/)
{
	return m_TrackedSockets.Emplace_GetRef(MakeUnique<FSocketEOS>(*this, SocketDescription)).Get();
}

FResolveInfoCached* FSocketSubsystemEOS::CreateResolveInfoCached(TSharedPtr<FInternetAddr> Addr) const
{
	return nullptr;
}

void FSocketSubsystemEOS::DestroySocket(FSocket* Socket)
{
	for (auto It = m_TrackedSockets.CreateIterator(); It; ++It)
	{
		if (It->IsValid() && It->Get() == Socket)
		{
			It.RemoveCurrent();
			return;
		}
	}
}

FAddressInfoResult FSocketSubsystemEOS::GetAddressInfo(const TCHAR* HostName, const TCHAR* ServiceName, EAddressInfoFlags /*unused*/, const FName /*unused*/, ESocketType /*unused*/)
{
	return FAddressInfoResult(HostName, ServiceName);
}

bool FSocketSubsystemEOS::RequiresChatDataBeSeparate()
{
	return false;
}

bool FSocketSubsystemEOS::RequiresEncryptedPackets()
{
	return false;
}

bool FSocketSubsystemEOS::GetHostName(FString& HostName)
{
	return false;
}

TSharedRef<FInternetAddr> FSocketSubsystemEOS::CreateInternetAddr()
{
	return MakeShared<FInternetAddrEOS>();
}

TSharedPtr<FInternetAddr> FSocketSubsystemEOS::GetAddressFromString(const FString& InString)
{
	bool bUnused;
	TSharedPtr<FInternetAddrEOS> NewAddress = StaticCastSharedRef<FInternetAddrEOS>(CreateInternetAddr());
	NewAddress->SetIp(*InString, bUnused);
	return NewAddress;
}

bool FSocketSubsystemEOS::HasNetworkDevice()
{
	return true;
}

const TCHAR* FSocketSubsystemEOS::GetSocketAPIName() const
{
	return TEXT("p2pSocketsEOS");
}

ESocketErrors FSocketSubsystemEOS::GetLastErrorCode()
{
	return TranslateErrorCode(m_LastSocketError);
}

ESocketErrors FSocketSubsystemEOS::TranslateErrorCode(int32 Code)
{
	return static_cast<ESocketErrors>(Code);
}

bool FSocketSubsystemEOS::GetLocalAdapterAddresses(TArray<TSharedPtr<FInternetAddr>>& OutAddresses)
{
	TSharedRef<FInternetAddr> AdapterAddress = GetLocalBindAddr(nullptr, *GLog);
	OutAddresses.Add(AdapterAddress);
	return true;
}

TArray<TSharedRef<FInternetAddr>> FSocketSubsystemEOS::GetLocalBindAddresses()
{
	TArray<TSharedRef<FInternetAddr>> OutAddresses;
	OutAddresses.Add(GetLocalBindAddr(nullptr, *GLog));
	return OutAddresses;
}

TSharedRef<FInternetAddr> FSocketSubsystemEOS::GetLocalBindAddr(FOutputDevice& Out)
{
	return GetLocalBindAddr(nullptr, Out);
}

EOS_HP2P FSocketSubsystemEOS::GetP2PHandle()
{
	check(m_Subsystem != nullptr);
	return m_Subsystem->m_P2PHandle;
}

EOS_ProductUserId FSocketSubsystemEOS::GetLocalUserId()
{
	if (m_Subsystem != nullptr)
	{
		return m_Subsystem->m_IdentityInterfacePtr->GetLocalProductUserId();
	}
	return nullptr;
}

TSharedRef<FInternetAddr> FSocketSubsystemEOS::GetLocalBindAddr(const UWorld* const OwningWorld, FOutputDevice& Out)
{
	TSharedRef<FInternetAddrEOS> BoundAddr = MakeShared<FInternetAddrEOS>();

	EOS_ProductUserId LocalUserId = GetLocalUserId();
	if (LocalUserId == nullptr)
	{
		UE_LOG(LogSocketSubsystemEOS, Error, TEXT("No local user to send p2p packets with"));
		return BoundAddr;
	}
	BoundAddr->SetLocalUserId(LocalUserId);

	FString SessionId;
	// Find our current session id from the Default online subsystem
	if (const IOnlineSubsystem* const DefaultSubsystem = Online::GetSubsystem(OwningWorld))
	{
		const IOnlineSessionPtr DefaultSessionInt = DefaultSubsystem->GetSessionInterface();
		if (DefaultSessionInt.IsValid())
		{
			if (const FNamedOnlineSession* const NamedSession = DefaultSessionInt->GetNamedSession(NAME_GameSession))
			{
				SessionId = NamedSession->GetSessionIdStr();
			}
		}
	}

	if (SessionId.IsEmpty())
	{
		SessionId = FName(NAME_GameSession).ToString();
	}

	BoundAddr->SetSocketName(SessionId);

	return BoundAddr;
}

bool FSocketSubsystemEOS::IsSocketWaitSupported() const
{
	return false;
}

void FSocketSubsystemEOS::SetLastSocketError(const ESocketErrors NewSocketError)
{
	m_LastSocketError = NewSocketError;
}

bool FSocketSubsystemEOS::BindChannel(const FInternetAddrEOS& Address)
{
	if (!Address.IsValid())
	{
		SetLastSocketError(ESocketErrors::SE_EINVAL);
		return false;
	}

	const uint8 Channel = Address.GetChannel();

	FChannelSet& ExistingBoundPorts = m_BoundAddresses.FindOrAdd(Address.GetSocketName());
	if (ExistingBoundPorts.Contains(Channel))
	{
		SetLastSocketError(ESocketErrors::SE_EADDRINUSE);
		return false;
	}

	ExistingBoundPorts.Add(Channel);
	return true;
}

bool FSocketSubsystemEOS::UnbindChannel(const FInternetAddrEOS& Address)
{
	if (!Address.IsValid())
	{
		SetLastSocketError(ESocketErrors::SE_EINVAL);
		return false;
	}

	const FString SocketName = Address.GetSocketName();
	const uint8 Channel = Address.GetChannel();

	// Find our sessions collection of ports
	FChannelSet* ExistingBoundPorts = m_BoundAddresses.Find(SocketName);
	if (!ExistingBoundPorts)
	{
		SetLastSocketError(ESocketErrors::SE_ENOTSOCK);
		return false;
	}

	// Remove our port and check if we had it bound
	if (ExistingBoundPorts->Remove(Channel) == 0)
	{
		SetLastSocketError(ESocketErrors::SE_ENOTSOCK);
		return false;
	}

	// Remove any empty sets
	if (ExistingBoundPorts->Num() == 0)
	{
		m_BoundAddresses.Remove(SocketName);
		ExistingBoundPorts = nullptr;
	}

	return true;
}

void DestroyEOSSocketSubsystem()
{
	FModuleManager& ModuleManager = FModuleManager::Get();

	if (ModuleManager.IsModuleLoaded("Sockets"))
	{
		FSocketSubsystemModule& SSS = FModuleManager::GetModuleChecked<FSocketSubsystemModule>("Sockets");
		SSS.UnregisterSocketSubsystem(EOS_SUBSYSTEM);
	}
}