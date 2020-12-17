/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#pragma once

#include "CoreMinimal.h"
#include "UObject/CoreOnline.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineSubsystemTypesEOS.h"
#include "eos_auth_types.h"
#include "eos_connect_types.h"

class FOnlineSubsystemEOS;

typedef TSharedPtr<FOnlineUser> FOnlineUserPtr;
typedef TSharedRef<FOnlineUser> FOnlineUserRef;

struct FGetAccountMappingOptions :
	public EOS_Connect_GetExternalAccountMappingsOptions
{
	FGetAccountMappingOptions() :
		EOS_Connect_GetExternalAccountMappingsOptions()
	{
		ApiVersion = EOS_CONNECT_GETEXTERNALACCOUNTMAPPINGS_API_LATEST;
		AccountIdType = EOS_EExternalAccountType::EOS_EAT_EPIC;
		TargetExternalUserId = AccountId;
	}
	char AccountId[EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH + 1];
};

typedef TEOSCallback<EOS_Connect_OnQueryExternalAccountMappingsCallback, EOS_Connect_QueryExternalAccountMappingsCallbackInfo> FQueryByStringIdsCallback;

struct FQueryByStringIdsOptions :
	public EOS_Connect_QueryExternalAccountMappingsOptions
{
	FQueryByStringIdsOptions(const uint32 InNumStringIds, EOS_ProductUserId InLocalUserId) :
		EOS_Connect_QueryExternalAccountMappingsOptions()
	{
		PointerArray.AddZeroed(InNumStringIds);
		for (int32 Index = 0; Index < PointerArray.Num(); Index++)
		{
			PointerArray[Index] = new char[EOS_CONNECT_EXTERNAL_ACCOUNT_ID_MAX_LENGTH+1];
		}
		ApiVersion = EOS_CONNECT_QUERYEXTERNALACCOUNTMAPPINGS_API_LATEST;
		AccountIdType = EOS_EExternalAccountType::EOS_EAT_EPIC;
		ExternalAccountIds = (const char**)PointerArray.GetData();
		ExternalAccountIdCount = InNumStringIds;
		LocalUserId = InLocalUserId;
	}

	~FQueryByStringIdsOptions()
	{
		for (int32 Index = 0; Index < PointerArray.Num(); Index++)
		{
			delete [] PointerArray[Index];
		}
	}
	TArray<char*> PointerArray;
};

class FUserOnlineAccountEOS : public TUserOnlineAccountEOS<FUserOnlineAccount>
{
public:
	FUserOnlineAccountEOS(FUniqueNetIdEOSRef InUserId) : TUserOnlineAccountEOS<FUserOnlineAccount>(InUserId) {}
	virtual ~FUserOnlineAccountEOS() = default;
};

typedef TSharedPtr<FUserOnlineAccountEOS> FUserOnlineAccountEOSPtr;
typedef TSharedRef<FUserOnlineAccountEOS> FUserOnlineAccountEOSRef;

template<class ListClass, class ListClassReturnType>
class TOnlinePlayerList
{
private:
	int32 LocalUserNum;
	FUniqueNetIdEOSRef OwningNetId;
	TArray<ListClass> ListEntries;
	TMap<FString, ListClass> NetIdStringToListEntryMap;
public:
	TOnlinePlayerList(int32 InLocalUserNum, FUniqueNetIdEOSRef InOwningNetId)
		: LocalUserNum(InLocalUserNum)
		, OwningNetId(InOwningNetId) {}
public:
	const TArray<ListClass>& GetList() { return ListEntries; }

	void Add(const FString& InNetId, ListClass InListEntry)
	{
		ListEntries.Add(InListEntry);
		NetIdStringToListEntryMap.Add(InNetId, InListEntry);
	}

	void UpdateNetIdStr(const FString& PrevNetId, const FString& NewNetId)
	{
		ListClass ListEntry = NetIdStringToListEntryMap[PrevNetId];
		NetIdStringToListEntryMap.Remove(PrevNetId);
		NetIdStringToListEntryMap.Add(NewNetId, ListEntry);
	}

	ListClassReturnType GetByIndex(int32 Index)
	{
		if (ListEntries.IsValidIndex(Index))
		{
			return ListEntries[Index];
		}
		return ListClassReturnType();
	}

	ListClassReturnType GetByNetIdString(const FString& NetId)
	{
		const ListClass* Found = NetIdStringToListEntryMap.Find(NetId);
		if (Found != nullptr)
		{
			return *Found;
		}
		return ListClassReturnType();
	}
};

struct FNotificationIdCallbackPair
{
public:
	EOS_NotificationId NotificationId;
	FCallbackBase* Callback;
public:
	FNotificationIdCallbackPair()
		: NotificationId(EOS_INVALID_NOTIFICATIONID)
		, Callback(nullptr)
	{}

	virtual ~FNotificationIdCallbackPair()
	{
		delete Callback;
	}
};

class FOnlineIdentityEOS : public IOnlineIdentity
{
public:
	FOnlineIdentityEOS(FOnlineSubsystemEOS* InSubsystem);
	virtual ~FOnlineIdentityEOS();
	friend class FOnlineUserEOS;
// IOnlineIdentity Interface
public:
	virtual bool Login(int32 LocalUserNum, const FOnlineAccountCredentials& AccountCredentials) override;
	virtual bool Logout(int32 LocalUserNum) override;
	virtual bool AutoLogin(int32 LocalUserNum) override;
	virtual TSharedPtr<FUserOnlineAccount> GetUserAccount(const FUniqueNetId& UserId) const override;
	virtual TArray<TSharedPtr<FUserOnlineAccount>> GetAllUserAccounts() const override;
	virtual TSharedPtr<const FUniqueNetId> GetUniquePlayerId(int32 LocalUserNum) const override;
	virtual TSharedPtr<const FUniqueNetId> CreateUniquePlayerId(uint8* Bytes, int32 Size) override;
	virtual TSharedPtr<const FUniqueNetId> CreateUniquePlayerId(const FString& Str) override;
	virtual ELoginStatus::Type GetLoginStatus(int32 LocalUserNum) const override;
	virtual ELoginStatus::Type GetLoginStatus(const FUniqueNetId& UserId) const override;
	virtual FString GetPlayerNickname(int32 LocalUserNum) const override;
	virtual FString GetPlayerNickname(const FUniqueNetId& UserId) const override;
	virtual FString GetAuthToken(int32 LocalUserNum) const override;
	virtual void GetUserPrivilege(const FUniqueNetId& UserId, EUserPrivileges::Type Privilege, const FOnGetUserPrivilegeCompleteDelegate& Delegate) override;
	virtual FString GetAuthType() const override;
	virtual void RevokeAuthToken(const FUniqueNetId& LocalUserId, const FOnRevokeAuthTokenCompleteDelegate& Delegate) override;
	virtual FPlatformUserId GetPlatformUserIdFromUniqueNetId(const FUniqueNetId& UniqueNetId) const override;
// ~IOnlineIdentity Interface
public:
	ELoginStatus::Type GetLoginStatus(const FUniqueNetIdEOS& UserId) const;

PACKAGE_SCOPE:
	FOnlineIdentityEOS() = delete;
PACKAGE_SCOPE:
	EOS_ELoginStatus GetAuthLoggedInStatus(int32 userId) const;
	EOS_ELoginStatus GetConnectLoggedInStatus(int32 userId) const;
	EOS_EpicAccountId GetLocalEpicAccountId(int32 LocalUserNum) const;
	EOS_EpicAccountId GetLocalEpicAccountId() const;
	EOS_ProductUserId GetLocalProductUserId(int32 LocalUserNum) const;
	EOS_ProductUserId GetLocalProductUserId() const;
	EOS_EpicAccountId GetLocalEpicAccountId(EOS_ProductUserId UserId) const;
	EOS_ProductUserId GetLocalProductUserId(EOS_EpicAccountId AccountId) const;
	FUniqueNetIdEOSPtr GetLocalUniqueNetIdEOS(int32 LocalUserNum) const;
	FUniqueNetIdEOSPtr GetLocalUniqueNetIdEOS(EOS_ProductUserId UserId) const;
	FUniqueNetIdEOSPtr GetLocalUniqueNetIdEOS(EOS_EpicAccountId AccountId) const;
	FUniqueNetIdEOSPtr GetLocalUniqueNetIdEOS() const {	return GetLocalUniqueNetIdEOS(GetDefaultLocalUser()); }

	int32 GetLocalUserNumFromUniqueNetId(const FUniqueNetId& NetId) const;

	EOS_EpicAccountId GetEpicAccountId(const FUniqueNetId& NetId) const;
	EOS_ProductUserId GetProductUserId(const FUniqueNetId& NetId) const;

	FOnlineUserPtr GetLocalOnlineUser(int32 LocalUserNum) const;
	FOnlineUserPtr GetOnlineUser(EOS_ProductUserId UserId) const;
	FOnlineUserPtr GetOnlineUser(EOS_EpicAccountId AccountId) const;

PACKAGE_SCOPE:
	void Tick(float deltaTime);
	void ConnectLogin(int32 LocalUserNum);
	void CreateConnectedLogin(int32 LocalUserNum, EOS_EpicAccountId AccountId, EOS_ContinuanceToken Token);
	void CreateDeviceId(int32 localUserNum, const FOnlineAccountCredentials& credentials, bool bInvokeLoginOnSuccess = false);
	void RefreshConnectLogin(int32 LocalUserNum);
	void FullLoginCallback(int32 LocalUserNum);
	void LoginStatusChanged(const EOS_Auth_LoginStatusChangedCallbackInfo* Data);
	void ConnectLoginStatusChanged(const EOS_Connect_LoginStatusChangedCallbackInfo* data);
	int32 GetDefaultLocalUser() const { return m_DefaultLocalUser; }

private:
	void RemoveLocalUser(int32 LocalUserNum);
	void AddLocalUser(int32 LocalUserNum, EOS_EpicAccountId EpicAccountId, EOS_ProductUserId UserId);
	void ReadUserInfo(EOS_EpicAccountId EpicAccountId);
	void UpdateUserInfo(IAttributeAccessInterfaceRef AttriubteAccessRef, EOS_EpicAccountId LocalId, EOS_EpicAccountId TargetId);
private:
	FOnlineSubsystemEOS* m_Subsystem;
	int32 m_DefaultLocalUser;
	bool bInitialized;
protected:
	/** Notification state for SDK events */
	EOS_NotificationId LoginNotificationId;
	EOS_NotificationId ConnectLoginNotificationId;
	FCallbackBase* LoginNotificationCallback;
	TMap<int32, FNotificationIdCallbackPair*> LocalUserNumToConnectLoginNotifcationMap;
protected:
	// int maps
	TMap<EOS_EpicAccountId, int32> AccountIdToUserNumMap;
	TMap<int32, FUniqueNetIdEOSPtr> UserNumToNetIdMap;
	TMap<EOS_ProductUserId, int32> ProductUserIdToUserNumMap;
	TMap<FString, FUserOnlineAccountEOSRef> StringToUserAccountMap;
protected:
	// string maps
	TMap<EOS_EpicAccountId, FString> AccountIdToStringMap;
	TMap<EOS_ProductUserId, FString> ProductUserIdToStringMap;
	TMap<FString, EOS_EpicAccountId> StringToAccountIdMap;
	TMap<FString, EOS_ProductUserId> StringToProductUserIdMap;
protected:
	TMap<FString, FOnlineUserPtr> NetIdStringToOnlineUserMap;
	TMap<EOS_EpicAccountId, IAttributeAccessInterfaceRef> EpicAccountIdToAttributeAccessMap;
};