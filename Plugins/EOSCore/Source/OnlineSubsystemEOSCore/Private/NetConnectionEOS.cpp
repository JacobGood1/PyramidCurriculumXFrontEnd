/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "NetConnectionEOS.h"
#include "NetDriverEOS.h"
#include "InternetAddrEOS.h"
#include "SocketEOS.h"
#include "OnlineSubsystemModuleEOS.h"
#include "OnlineSubsystemEOSLogging.h"
#include "OnlineSubsystemEOSPrivatePCH.h"

UNetConnectionEOS::UNetConnectionEOS(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bIsPassthrough(false)
	, bHasP2PSession(false)
{
}

void UNetConnectionEOS::InitLocalConnection(UNetDriver* InDriver, FSocket* InSocket, const FURL& InURL, EConnectionState InState, int32 InMaxPacket, int32 InPacketOverhead)
{
	bIsPassthrough = !static_cast<UNetDriverEOS*>(InDriver)->bIsUsingP2PSockets || !InURL.Host.StartsWith(EOS_CONNECTION_URL_PREFIX, ESearchCase::IgnoreCase);
	bHasP2PSession = !bIsPassthrough;
	  
	if (bHasP2PSession)
	{
		DisableAddressResolution();
	}

	Super::InitLocalConnection(InDriver, InSocket, InURL, InState, InMaxPacket, InPacketOverhead);
}

void UNetConnectionEOS::InitRemoteConnection(UNetDriver* InDriver, FSocket* InSocket, const FURL& InURL, const FInternetAddr& InRemoteAddr, EConnectionState InState, int32 InMaxPacket, int32 InPacketOverhead)
{
	bIsPassthrough = static_cast<UNetDriverEOS*>(InDriver)->bIsPassthrough;
	bHasP2PSession = !bIsPassthrough;

	if (bHasP2PSession)
	{
		DisableAddressResolution();
	}

	Super::InitRemoteConnection(InDriver, InSocket, InURL, InRemoteAddr, InState, InMaxPacket, InPacketOverhead);
}

void UNetConnectionEOS::CleanUp()
{
	Super::CleanUp();

	if (bHasP2PSession)
	{
		DestroyEOSConnection();
	}
}

void UNetConnectionEOS::DestroyEOSConnection()
{
	if (!Socket)
	{
		return;
	}

	if (!bHasP2PSession)
	{
		return;
	}

	bHasP2PSession = false;

	TSharedPtr<FInternetAddrEOS> RemoteAddrEOS = StaticCastSharedPtr<FInternetAddrEOS>(RemoteAddr);
	if (RemoteAddrEOS.IsValid())
	{
		static_cast<FSocketEOS*>(Socket)->Close(*RemoteAddrEOS);
	}
}
