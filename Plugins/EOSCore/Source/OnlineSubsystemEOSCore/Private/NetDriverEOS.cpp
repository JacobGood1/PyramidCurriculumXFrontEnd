/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "NetDriverEOS.h"
#include "NetConnectionEOS.h"
#include "SocketEOS.h"
#include "SocketSubsystemEOS.h"
#include "OnlineSubsystemEOS.h"
#include "OnlineSubsystemModuleEOS.h"
#include "OnlineSubsystemEOSLogging.h"
#include "OnlineSubsystemEOSPrivatePCH.h"

bool UNetDriverEOS::IsAvailable() const
{
	// Use passthrough sockets if we are a dedicated server
	if (IsRunningDedicatedServer())
	{
		bool bSocketsEnabled = false;
		GConfig->GetBool(TEXT("/Script/OnlineSubsystemEOS.NetDriverEOS"), TEXT("bIsUsingP2PSockets"), bSocketsEnabled, GEngineIni);

		if (bSocketsEnabled)
		{
			LogError("You have Sockets enabled for the NetDriverEOS while running a Dedicated Server. This is a unsupported configuration. Make sure you set bIsUsingP2PSockets=false when using a Dedicated Server or you will not be able to connect.");
		}

		return false;
	}

	if (IOnlineSubsystem* Subsystem = Online::GetSubsystem(FindWorld(), EOS_SUBSYSTEM))
	{
		if (ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(EOS_SUBSYSTEM))
		{
			return true;
		}
	}

	return false;
}

bool UNetDriverEOS::InitBase(bool bInitAsClient, FNetworkNotify* InNotify, const FURL& URL, bool bReuseAddressAndPort, FString& Error)
{
/*
	if (!URL.Host.StartsWith(EOS_CONNECTION_URL_PREFIX))
	{
		LogError("Attempted to connect to an invalid EOS address (%s)", *URL.ToString(true));
		return false;
	}*/

	if (bIsPassthrough)
	{
		LogVerbose("Running as pass-through");

		TArray<FString> Tokens;

		if (URL.Host.ParseIntoArray(Tokens, TEXT("."), false) == 4 || !URL.Host.Contains(EOS_CONNECTION_URL_PREFIX))
		{
			bool bSocketsEnabled = false;
			GConfig->GetBool(TEXT("/Script/OnlineSubsystemEOS.NetDriverEOS"), TEXT("bIsUsingP2PSockets"), bSocketsEnabled, GEngineIni);

			if (bSocketsEnabled)
			{
				LogError("You are connecting to a regular IPV4 Addr while using EOS Sockets. This is a unsupported configuration. Set bIsUsingP2PSockets=false in your DefaultEngine.ini");

				return false;
			}
		}

		return Super::InitBase(bInitAsClient, InNotify, URL, bReuseAddressAndPort, Error);
	}

	if (!UNetDriver::InitBase(bInitAsClient, InNotify, URL, bReuseAddressAndPort, Error))
	{
		LogWarning("Failed to init driver base");
		return false;
	}

	FSocketSubsystemEOS* const SocketSubsystem = static_cast<FSocketSubsystemEOS*>(GetSocketSubsystem());

	if (!SocketSubsystem)
	{
		LogWarning("Could not get socket subsystem");
		return false;
	}

	const UWorld* const MyWorld = FindWorld();

	TSharedRef<FInternetAddr> LocalAddress = SocketSubsystem->GetLocalBindAddr(MyWorld, *GLog);

	if (!LocalAddress->IsValid())
	{
		// Not logged in?
		Error = TEXT("Could not bind local address");
		LogWarning("Could not bind local address");
		return false;
	}

	SetSocketAndLocalAddress(SocketSubsystem->CreateSocket(NAME_DGram, TEXT("UE4"), NAME_None));

	if (GetSocket() == nullptr)
	{
		LogWarning("Could not create socket");
		return false;
	}

	// Store our local address and set our port
	TSharedRef<FInternetAddrEOS> EOSLocalAddress = StaticCastSharedRef<FInternetAddrEOS>(LocalAddress);
	EOSLocalAddress->SetChannel(bInitAsClient ? GetClientPort() : URL.Port);

	static_cast<FSocketEOS*>(GetSocket())->SetLocalAddress(*EOSLocalAddress);

	LocalAddr = LocalAddress;

	return true;
}

bool UNetDriverEOS::InitConnect(FNetworkNotify* InNotify, const FURL& ConnectURL, FString& Error)
{
/*
	if (!ConnectURL.Host.StartsWith(EOS_CONNECTION_URL_PREFIX))
	{
		LogError("Attempted to connect to an invalid EOS address (%s)", *ConnectURL.ToString(true));
		return false;
	}*/

	if (!bIsUsingP2PSockets || !IsAvailable() || !ConnectURL.Host.StartsWith(EOS_CONNECTION_URL_PREFIX, ESearchCase::IgnoreCase))
	{
		LogVerbose("Connecting using IPNetDriver passthrough. ConnectUrl = (%s)", *ConnectURL.ToString());

		bIsPassthrough = true;
		return Super::InitConnect(InNotify, ConnectURL, Error);
	}

	bool bIsValid = false;
	TSharedRef<FInternetAddrEOS> RemoteHost = MakeShared<FInternetAddrEOS>();
	RemoteHost->SetIp(*ConnectURL.Host, bIsValid);
	if (!bIsValid || ConnectURL.Port < 0)
	{
		Error = TEXT("Invalid remote address");
		LogWarning("Invalid Remote Address. ConnectUrl = (%s)", *ConnectURL.ToString());
		return false;
	}

	LogVerbose("Connecting using EOSNetDriver. ConnectUrl = (%s)", *ConnectURL.ToString());

	if (!InitBase(true, InNotify, ConnectURL, false, Error))
	{
		return false;
	}

	LocalAddr = RemoteHost;

	FSocket* CurSocket = GetSocket();

	FSocketSubsystemEOS* const SocketSubsystem = static_cast<FSocketSubsystemEOS*>(GetSocketSubsystem());
	check(SocketSubsystem);
	if (!SocketSubsystem->BindNextPort(CurSocket, *LocalAddr, MaxPortCountToTry + 1, 1))
	{
		// Failure
		Error = TEXT("Could not bind local port");
		LogWarning("Could not bind local port in %d attempts", MaxPortCountToTry);
		return false;
	}

	UNetConnectionEOS* Connection = NewObject<UNetConnectionEOS>(NetConnectionClass);
	check(Connection);

	ServerConnection = Connection;
	Connection->InitLocalConnection(this, CurSocket, ConnectURL, USOCK_Pending);

	CreateInitialClientChannels();

	return true;
}

bool UNetDriverEOS::InitListen(FNetworkNotify* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error)
{
	if (!bIsUsingP2PSockets || !IsAvailable() || LocalURL.HasOption(TEXT("bIsLanMatch")) || LocalURL.HasOption(TEXT("bUseIPSockets")))
	{
		LogVerbose("Init as IPNetDriver listen server. LocalURL = (%s)", *LocalURL.ToString());

		bIsPassthrough = true;
		return Super::InitListen(InNotify, LocalURL, bReuseAddressAndPort, Error);
	}

	LogVerbose("Init as EOSNetDriver listen server. LocalURL = (%s)", *LocalURL.ToString());

	if (!InitBase(false, InNotify, LocalURL, bReuseAddressAndPort, Error))
	{
		return false;
	}

	FSocket* CurSocket = GetSocket();

	if (!CurSocket->Listen(0))
	{
		Error = TEXT("Could not listen");
		LogWarning("Could not listen on socket");
		return false;
	}

	InitConnectionlessHandler();

	LogVerbose("Initialized as an EOSP2P listen server");
	return true;
}

ISocketSubsystem* UNetDriverEOS::GetSocketSubsystem()
{
#if UE_EDITOR
	if (FindWorld())
	{
		FOnlineSubsystemEOS* m_Subsystem = static_cast<FOnlineSubsystemEOS*>(Online::GetSubsystem(FindWorld(), EOS_SUBSYSTEM));

		if (m_Subsystem && m_Subsystem->m_SocketSubsystem)
			return m_Subsystem->m_SocketSubsystem.Get();
	}
#endif
	return ISocketSubsystem::Get(bIsPassthrough ? PLATFORM_SOCKETSUBSYSTEM : EOS_SUBSYSTEM);
}

void UNetDriverEOS::Shutdown()
{
	LogVerbose("Shutting down NetDriver");

	Super::Shutdown();

	if (!bIsPassthrough)
	{
		if (UNetConnectionEOS* const EOSServerConnection = Cast<UNetConnectionEOS>(ServerConnection))
		{
			EOSServerConnection->DestroyEOSConnection();
		}
		for (UNetConnection* Client : ClientConnections)
		{
			if (UNetConnectionEOS* const EOSClient = Cast<UNetConnectionEOS>(Client))
			{
				EOSClient->DestroyEOSConnection();
			}
		}
	}
}

int UNetDriverEOS::GetClientPort()
{
	if (bIsPassthrough)
	{
		return Super::GetClientPort();
	}

	return 49152;
}

UWorld* UNetDriverEOS::FindWorld() const
{
	UWorld* MyWorld = GetWorld();

	if (!MyWorld && GEngine)
	{
		if (FWorldContext* WorldContext = GEngine->GetWorldContextFromPendingNetGameNetDriver(this))
		{
			MyWorld = WorldContext->World();
		}
	}

	return MyWorld;
}

