/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Friends/EOSFriends.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLogging.h"

UCoreFriends* UCoreFriends::s_FriendsObject = nullptr;

UCoreFriends::UCoreFriends()
{
	s_FriendsObject = this;
}

void UCoreFriends::EOSFriendsQueryFriends(UObject* worldContextObject, FEOSFriendsQueryFriendsOptions options, const FOnFriendsQueryFriendsCallback& callback)
{
	LogVerbose("");

	if (GetFriendsHandle(worldContextObject))
	{
		EOS_Friends_QueryFriendsOptions FriendsOptions = { };
		FriendsOptions.ApiVersion = options.ApiVersion;
		FriendsOptions.LocalUserId = options.LocalUserId;

		FFriendsQueryFriendsCallback* CallbackObj = new FFriendsQueryFriendsCallback({ this, callback });
		EOS_Friends_QueryFriends(GetFriendsHandle(worldContextObject), &FriendsOptions, CallbackObj, Internal_OnFriendsQueryFriendsCallback);
	}
}

void UCoreFriends::EOSFriendsSendInvite(UObject* worldContextObject, FEOSFriendsSendInviteOptions options, const FOnFriendsSendInviteCallback& callback)
{
	LogVerbose("");

	if (GetFriendsHandle(worldContextObject))
	{
		EOS_Friends_SendInviteOptions FriendsOptions = { };
		FriendsOptions.ApiVersion = options.ApiVersion;
		FriendsOptions.LocalUserId = options.LocalUserId;
		FriendsOptions.TargetUserId = options.TargetUserId;

		FFriendsSendInviteCallback* CallbackObj = new FFriendsSendInviteCallback({ this, callback });
		EOS_Friends_SendInvite(GetFriendsHandle(worldContextObject), &FriendsOptions, CallbackObj, Internal_OnFriendsSendInviteCallback);
	}
}

void UCoreFriends::EOSFriendsAcceptInvite(UObject* worldContextObject, FEOSFriendsAcceptInviteOptions options, const FOnFriendsAcceptInviteCallback& callback)
{
	LogVerbose("");

	if (GetFriendsHandle(worldContextObject))
	{
		EOS_Friends_AcceptInviteOptions FriendsOptions = { };
		FriendsOptions.ApiVersion = options.ApiVersion;
		FriendsOptions.LocalUserId = options.LocalUserId;
		FriendsOptions.TargetUserId = options.TargetUserId;

		FFriendsAcceptInviteCallback* CallbackObj = new FFriendsAcceptInviteCallback({ this, callback });
		EOS_Friends_AcceptInvite(GetFriendsHandle(worldContextObject), &FriendsOptions, CallbackObj, Internal_OnFriendsAcceptInviteCallback);
	}
}

void UCoreFriends::EOSFriendsRejectInvite(UObject* worldContextObject, FEOSFriendsRejectInviteOptions options, const FOnFriendsRejectInviteCallback& callback)
{
	LogVerbose("");

	if (GetFriendsHandle(worldContextObject))
	{
		EOS_Friends_RejectInviteOptions FriendsOptions = { };
		FriendsOptions.ApiVersion = options.ApiVersion;
		FriendsOptions.LocalUserId = options.LocalUserId;
		FriendsOptions.TargetUserId = options.TargetUserId;

		FFriendsRejectInviteCallback* CallbackObj = new FFriendsRejectInviteCallback({ this, callback });
		EOS_Friends_RejectInvite(GetFriendsHandle(worldContextObject), &FriendsOptions, CallbackObj, Internal_OnFriendsRejectInviteCallback);
	}
}

int32 UCoreFriends::EOSFriendsGetFriendsCount(UObject* worldContextObject, FEOSFriendsGetFriendsCountOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetFriendsHandle(worldContextObject))
	{
		EOS_Friends_GetFriendsCountOptions FriendsOptions = { };
		FriendsOptions.ApiVersion = options.ApiVersion;
		FriendsOptions.LocalUserId = options.LocalUserId;

		Result = EOS_Friends_GetFriendsCount(GetFriendsHandle(worldContextObject), &FriendsOptions);
	}

	return Result;
}

FEOSEpicAccountId UCoreFriends::EOSFriendsGetFriendAtIndex(UObject* worldContextObject, FEOSFriendsGetFriendAtIndexOptions options)
{
	LogVerbose("");

	FEOSEpicAccountId Id;

	if (GetFriendsHandle(worldContextObject))
	{
		EOS_Friends_GetFriendAtIndexOptions FriendsOptions = { };
		FriendsOptions.ApiVersion = options.ApiVersion;
		FriendsOptions.LocalUserId = options.LocalUserId;
		FriendsOptions.Index = options.Index;

		Id = EOS_Friends_GetFriendAtIndex(GetFriendsHandle(worldContextObject), &FriendsOptions);
	}

	return Id;
}

EEOSEFriendsStatus UCoreFriends::EOSFriendsGetStatus(UObject* worldContextObject, FEOSFriendsGetStatusOptions options)
{
	LogVerbose("");

	EEOSEFriendsStatus Status = EEOSEFriendsStatus::EOS_FS_NotFriends;

	if (GetFriendsHandle(worldContextObject))
	{
		EOS_Friends_GetStatusOptions FriendsOptions = { };
		FriendsOptions.ApiVersion = options.ApiVersion;
		FriendsOptions.LocalUserId = options.LocalUserId;
		FriendsOptions.TargetUserId = options.TargetUserId;

		Status = static_cast<EEOSEFriendsStatus>(EOS_Friends_GetStatus(GetFriendsHandle(worldContextObject), &FriendsOptions));
	}

	return Status;
}

struct FFriendsUpdateCallback
{
public:
	FFriendsUpdateCallback(UObject* worldContextObject, const FOnFriendsUpdateCallback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{
	}
	~FFriendsUpdateCallback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnFriendsUpdateCallback Callback;
};

static TMap<FEOSNotificationId, FFriendsUpdateCallback*> s_FriendsUpdateCallbacks;

FEOSNotificationId UCoreFriends::EOSFriendsAddNotifyFriendsUpdate(UObject* worldContextObject, const FOnFriendsUpdateCallback& callback)
{
	LogVerbose("");

	FEOSNotificationId Id;

	if (GetFriendsHandle(worldContextObject))
	{
		EOS_Friends_AddNotifyFriendsUpdateOptions FriendsOptions = { EOS_FRIENDS_ADDNOTIFYFRIENDSUPDATE_API_LATEST };

		FFriendsUpdateCallback* CallbackObj = new FFriendsUpdateCallback(worldContextObject, callback);
		CallbackObj->Callback = callback;

		Id = EOS_Friends_AddNotifyFriendsUpdate(GetFriendsHandle(worldContextObject), &FriendsOptions, CallbackObj, ([](const EOS_Friends_OnFriendsUpdateInfo* data)
		{
			FFriendsUpdateCallback* CallbackObj = static_cast<FFriendsUpdateCallback*>(data->ClientData);
			CallbackObj->Callback.ExecuteIfBound(*data);
		}));

		s_FriendsUpdateCallbacks.Add(Id, CallbackObj);
	}

	return Id;
}

void UCoreFriends::EOSFriendsRemoveNotifyFriendsUpdate(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetFriendsHandle(worldContextObject))
	{
		EOS_Friends_RemoveNotifyFriendsUpdate(GetFriendsHandle(worldContextObject), id);
		if (FFriendsUpdateCallback** CallbackObj = s_FriendsUpdateCallbacks.Find(id))
		{
			delete CallbackObj;
		}
		s_FriendsUpdateCallbacks.Remove(id);
	}
}

void UCoreFriends::Internal_OnFriendsQueryFriendsCallback(const EOS_Friends_QueryFriendsCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FFriendsQueryFriendsCallback* CallbackObj = static_cast<FFriendsQueryFriendsCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreFriends* Friends = CallbackObj->FriendsObject;
		check(Friends);

		if (Friends)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreFriends::Internal_OnFriendsSendInviteCallback(const EOS_Friends_SendInviteCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FFriendsSendInviteCallback* CallbackObj = static_cast<FFriendsSendInviteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreFriends* Friends = CallbackObj->FriendsObject;
		check(Friends);

		if (Friends)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreFriends::Internal_OnFriendsAcceptInviteCallback(const EOS_Friends_AcceptInviteCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));
	
	FFriendsAcceptInviteCallback* CallbackObj = static_cast<FFriendsAcceptInviteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreFriends* Friends = CallbackObj->FriendsObject;
		check(Friends);

		if (Friends)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}

void UCoreFriends::Internal_OnFriendsRejectInviteCallback(const EOS_Friends_RejectInviteCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FFriendsRejectInviteCallback* CallbackObj = static_cast<FFriendsRejectInviteCallback*>(data->ClientData);
	check(CallbackObj);
	if (CallbackObj)
	{
		UCoreFriends* Friends = CallbackObj->FriendsObject;
		check(Friends);

		if (Friends)
		{
			CallbackObj->Callback.ExecuteIfBound(*data);
		}

		delete CallbackObj;
	}
}