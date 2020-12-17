/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#pragma once

#include "CoreMinimal.h"
#include "SocketSubsystem.h"
#include "eos_p2p_types.h"

class UNetConnectionEOS;
class FSocket;
class FResolveInfoCached;
class FResolveInfo;
class FInternetAddr;
class FInternetAddrEOS;
class FSocketEOS;
class FOnlineSubsystemEOS;

typedef TSet<uint8> FChannelSet;

class FSocketSubsystemEOS : public ISocketSubsystem
{
public:
	FSocketSubsystemEOS(FOnlineSubsystemEOS* InSubsystemEOS);
	virtual ~FSocketSubsystemEOS();

public:
//~ Begin ISocketSubsystem Interface
	virtual bool Init(FString& Error) override;
	virtual void Shutdown() override;
	virtual FSocket* CreateSocket(const FName& SocketType, const FString& SocketDescription, const FName& ProtocolType) override;
	virtual FResolveInfoCached* CreateResolveInfoCached(TSharedPtr<FInternetAddr> Addr) const override;
	virtual void DestroySocket(FSocket* Socket) override;
	virtual FAddressInfoResult GetAddressInfo(const TCHAR* HostName, const TCHAR* ServiceName = nullptr, EAddressInfoFlags QueryFlags = EAddressInfoFlags::Default, const FName ProtocolTypeName = NAME_None, ESocketType SocketType = ESocketType::SOCKTYPE_Unknown) override;
	virtual bool RequiresChatDataBeSeparate() override;
	virtual bool RequiresEncryptedPackets() override;
	virtual bool GetHostName(FString& HostName) override;
	virtual TSharedRef<FInternetAddr> CreateInternetAddr() override;
	virtual TSharedPtr<FInternetAddr> GetAddressFromString(const FString& InString) override;
	virtual bool HasNetworkDevice() override;
	virtual const TCHAR* GetSocketAPIName() const override;
	virtual ESocketErrors GetLastErrorCode() override;
	virtual ESocketErrors TranslateErrorCode(int32 Code) override;
	virtual bool GetLocalAdapterAddresses(TArray<TSharedPtr<FInternetAddr>>& OutAdresses) override;
	virtual TSharedRef<FInternetAddr> GetLocalBindAddr(FOutputDevice& Out) override;
	virtual TArray<TSharedRef<FInternetAddr>> GetLocalBindAddresses() override;
	TSharedRef<FInternetAddr> GetLocalBindAddr(const UWorld* const OwningWorld, FOutputDevice& Out);
	virtual bool IsSocketWaitSupported() const override;
//~ End ISocketSubsystem Interface

public:
	void SetLastSocketError(const ESocketErrors NewSocketError);
	EOS_HP2P GetP2PHandle();
	EOS_ProductUserId GetLocalUserId();
	bool BindChannel(const FInternetAddrEOS& Address);
	bool UnbindChannel(const FInternetAddrEOS& Address);
private:
	FOnlineSubsystemEOS* m_Subsystem;
	TArray<TUniquePtr<FSocketEOS>> m_TrackedSockets;
	TMap<FString, FChannelSet> m_BoundAddresses;
	ESocketErrors m_LastSocketError;
};

void DestroyEOSSocketSubsystem();