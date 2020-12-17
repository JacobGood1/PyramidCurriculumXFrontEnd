/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "InternetAddrEOS.h"
#include "OnlineSubsystemTypesEOS.h"
#include "OnlineSubsystemModuleEOS.h"
#include "OnlineSubsystemEOSLogging.h"
#include "OnlineSubsystemEOSPrivatePCH.h"

DEFINE_LOG_CATEGORY(LogSocketSubsystemEOS);

inline uint8 PortToChannel(int32 InPort)
{
	return InPort > 255 ? InPort % 256 : FMath::Clamp(InPort, 0, 255);
}

FInternetAddrEOS::FInternetAddrEOS()
	: LocalUserId(nullptr)
	, RemoteUserId(nullptr)
	, Channel(0)
{
	SocketName[0] = '\0';
}

FInternetAddrEOS::FInternetAddrEOS(const FString& InRemoteUserId, const FString& InSocketName, const int32 InChannel)
	: LocalUserId(nullptr)
	, RemoteUserId(nullptr)
{
	RemoteUserId = EOS_ProductUserId_FromString(TCHAR_TO_UTF8(*InRemoteUserId));
	FCStringAnsi::Strcpy(SocketName, TCHAR_TO_UTF8(*InSocketName));
	Channel = PortToChannel(InChannel);
}

FInternetAddrEOS::FInternetAddrEOS(const EOS_ProductUserId InRemoteUserId, const FString& InSocketName, const int32 InChannel)
	: LocalUserId(nullptr)
	, RemoteUserId(InRemoteUserId)
	, Channel(InChannel)
{
	FCStringAnsi::Strcpy(SocketName, TCHAR_TO_UTF8(*InSocketName));
	Channel = PortToChannel(InChannel);
}

void FInternetAddrEOS::SetIp(uint32)
{
	LogWarning("Calls to FInternetAddrEOS::SetIp are not valid");
}

void FInternetAddrEOS::SetIp(const TCHAR* InAddr, bool& bIsValid)
{
	bIsValid = false;

	if (InAddr == nullptr)
	{
		return;
	}

	Channel = 0;

	TArray<FString> UrlParts;
	FString FullAddress = InAddr;
	FullAddress.ParseIntoArray(UrlParts, EOS_URL_SEPARATOR, false);
	
	if (UrlParts.Num() < 3 || UrlParts.Num() > 4)
	{
		return;
	}
	if (UrlParts[0] != EOS_CONNECTION_URL_PREFIX)
	{
		return;
	}
	RemoteUserId = EOS_ProductUserId_FromString(TCHAR_TO_UTF8(*UrlParts[1]));
	if (EOS_ProductUserId_IsValid(RemoteUserId) == EOS_FALSE)
	{
		return;
	}
	if (UrlParts[2].Len() == 0)
	{
		return;
	}
	FCStringAnsi::Strcpy(SocketName, TCHAR_TO_UTF8(*UrlParts[2]));
	if (UrlParts.Num() == 4)
	{
		Channel = FCString::Atoi(*UrlParts[3]);
	}
	bIsValid = true;
}

void FInternetAddrEOS::GetIp(uint32& OutAddr) const
{
	OutAddr = 0u;

	LogWarning("Calls to FInternetAddrEOS::GetIp are not valid");
}

void FInternetAddrEOS::SetPort(int32 InPort)
{
	Channel = PortToChannel(InPort);
}

int32 FInternetAddrEOS::GetPort() const
{
	return Channel;
}

void FInternetAddrEOS::SetRawIp(const TArray<uint8>& RawAddr)
{
	auto ConvertedTCHARData = StringCast<TCHAR>(reinterpret_cast<const ANSICHAR*>(RawAddr.GetData()), RawAddr.Num());
	const FString IpAsString(ConvertedTCHARData.Length(), ConvertedTCHARData.Get());

	bool bUnused;
	SetIp(*IpAsString, bUnused);
}

TArray<uint8> FInternetAddrEOS::GetRawIp() const
{
	const FString StringVersion = ToString(true);
	auto ConvertedANSIData = StringCast<ANSICHAR>(*StringVersion, StringVersion.Len());

	TArray<uint8> OutData;
	for (int32 Index = 0; Index < ConvertedANSIData.Length(); ++Index)
	{
		OutData.Add(ConvertedANSIData.Get()[Index]);
	}

	return OutData;
}

void FInternetAddrEOS::SetAnyAddress()
{
}

void FInternetAddrEOS::SetBroadcastAddress()
{
}

void FInternetAddrEOS::SetLoopbackAddress()
{
}

FString FInternetAddrEOS::ToString(bool bAppendPort) const
{
	char PuidBuffer[64];
	int32 BufferLen = 64;
	if (EOS_ProductUserId_ToString(RemoteUserId, PuidBuffer, &BufferLen) != EOS_EResult::EOS_Success)
	{
		PuidBuffer[0] = '\0';
	}

	if (bAppendPort)
	{
		return FString::Printf(TEXT("%s%s%s%s%s%s%u"), EOS_CONNECTION_URL_PREFIX, EOS_URL_SEPARATOR, UTF8_TO_TCHAR(PuidBuffer), EOS_URL_SEPARATOR, UTF8_TO_TCHAR(SocketName), EOS_URL_SEPARATOR, Channel);
	}

	return FString::Printf(TEXT("%s%s%s%s%s"), EOS_CONNECTION_URL_PREFIX, EOS_URL_SEPARATOR, UTF8_TO_TCHAR(PuidBuffer), EOS_URL_SEPARATOR, UTF8_TO_TCHAR(SocketName));
}

uint32 FInternetAddrEOS::GetTypeHash() const
{
	return HashCombine(HashCombine(HashCombine(::GetTypeHash((void*)LocalUserId), ::GetTypeHash((void *)RemoteUserId)), ::GetTypeHash(SocketName)), Channel);
}

bool FInternetAddrEOS::IsValid() const
{
	return (EOS_ProductUserId_IsValid(LocalUserId) == EOS_TRUE || EOS_ProductUserId_IsValid(RemoteUserId) == EOS_TRUE) && FCStringAnsi::Strlen(SocketName) > 0;
}

TSharedRef<FInternetAddr> FInternetAddrEOS::Clone() const
{
	return MakeShared<FInternetAddrEOS>(*this);
}
