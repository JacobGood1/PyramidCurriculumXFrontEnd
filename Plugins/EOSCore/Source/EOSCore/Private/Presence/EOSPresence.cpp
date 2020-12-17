/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Presence/EOSPresence.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLogging.h"

UCorePresence* UCorePresence::s_PresenceObject = nullptr;

UCorePresence::UCorePresence()
{
	s_PresenceObject = this;
}

void UCorePresence::EOSPresenceQueryPresence(UObject* worldContextObject, FEOSPresenceQueryPresenceOptions options, const FOnPresenceQueryPresenceCompleteCallback& callback)
{
	LogVerbose("");

	if (GetPresenceHandle(worldContextObject))
	{
		EOS_Presence_QueryPresenceOptions PresenceOptions = { options.ApiVersion, options.LocalUserId, options.TargetUserId };
		FQueryPresenceCompleteCallback* CallbackObj = new FQueryPresenceCompleteCallback({ this, callback });
		EOS_Presence_QueryPresence(GetPresenceHandle(worldContextObject), &PresenceOptions, CallbackObj, Internal_OnPresenceQueryPresenceCallback);
	}
}

bool UCorePresence::EOSPresenceHasPresence(UObject* worldContextObject, FEOSPresenceHasPresenceOptions options)
{
	LogVerbose("");

	if (GetPresenceHandle(worldContextObject))
	{
		EOS_Presence_HasPresenceOptions PresenceOptions = { options.ApiVersion, options.LocalUserId, options.TargetUserId };

		return (EOS_Presence_HasPresence(GetPresenceHandle(worldContextObject), &PresenceOptions) > 0);
	}

	return false;
}

EOSResult UCorePresence::EOSPresenceCopyPresence(UObject* worldContextObject, FEOSPresenceCopyPresenceOptions options, FEOSPresenceInfo& outPresence)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetPresenceHandle(worldContextObject))
	{
		EOS_Presence_CopyPresenceOptions PresenceOptions = { };
		PresenceOptions.ApiVersion = options.ApiVersion;
		PresenceOptions.LocalUserId = options.LocalUserId;
		PresenceOptions.TargetUserId = options.TargetUserId;

		EOS_Presence_Info* PresenceInfo = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Presence_CopyPresence(GetPresenceHandle(worldContextObject), &PresenceOptions, &PresenceInfo));

		if (Result == EOSResult::EOS_Success)
		{
			outPresence = *PresenceInfo;
			EOS_Presence_Info_Release(PresenceInfo);
		}
	}

	return Result;
}

EOSResult UCorePresence::EOSPresenceCreatePresenceModification(UObject* worldContextObject, FEOSPresenceCreatePresenceModificationOptions options, FEOSHPresenceModification& handle)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetPresenceHandle(worldContextObject))
	{
		EOS_Presence_CreatePresenceModificationOptions PresenceOptions = { };
		PresenceOptions.ApiVersion = options.ApiVersion;
		PresenceOptions.LocalUserId = options.LocalUserId;

		EOS_HPresenceModification PresenceModification;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Presence_CreatePresenceModification(GetPresenceHandle(worldContextObject), &PresenceOptions, &PresenceModification));

		if (Result == EOSResult::EOS_Success)
		{
			handle = PresenceModification;
		}
	}

	return Result;
}

void UCorePresence::EOSPresenceSetPresence(UObject* worldContextObject, FEOSPresenceSetPresenceOptions options, const FOnPresenceSetPresenceCompleteCallback& callback)
{
	LogVerbose("");

	if (GetPresenceHandle(worldContextObject))
	{
		EOS_Presence_SetPresenceOptions PresenceOptions = { };
		PresenceOptions.ApiVersion = options.ApiVersion;
		PresenceOptions.LocalUserId = options.LocalUserId;
		PresenceOptions.PresenceModificationHandle = options.PresenceModificationHandle;

		FSetPresenceCompleteCallback* CallbackObj = new FSetPresenceCompleteCallback({ this, callback });
		EOS_Presence_SetPresence(GetPresenceHandle(worldContextObject), &PresenceOptions, CallbackObj, Internal_OnPresenceSetPresenceCallback);
	}
}

struct FPresenceChangedCallback
{
public:
	FPresenceChangedCallback(UObject* worldContextObject, const FOnPresenceChangedCallback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{
	}
	~FPresenceChangedCallback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnPresenceChangedCallback Callback;
};

static TMap<FEOSNotificationId, FPresenceChangedCallback*> s_PresenceChangedCallbacks;

FEOSNotificationId UCorePresence::EOSPresenceAddNotifyOnPresenceChanged(UObject* worldContextObject, const FOnPresenceChangedCallback& callback)
{
	LogVerbose("");

	FEOSNotificationId NotificationId;

	if (GetPresenceHandle(worldContextObject))
	{
		EOS_Presence_AddNotifyOnPresenceChangedOptions PresenceOptions = { };
		PresenceOptions.ApiVersion = EOS_PRESENCE_ADDNOTIFYONPRESENCECHANGED_API_LATEST;

		FPresenceChangedCallback* CallbackObj = new FPresenceChangedCallback(worldContextObject, callback);
		CallbackObj->Callback = callback;

		NotificationId = EOS_Presence_AddNotifyOnPresenceChanged(GetPresenceHandle(worldContextObject), &PresenceOptions, CallbackObj, ([](const EOS_Presence_PresenceChangedCallbackInfo* data)
		{
			FPresenceChangedCallback* CallbackData = static_cast<FPresenceChangedCallback*>(data->ClientData);
			CallbackData->Callback.ExecuteIfBound(*data);
		}));

		s_PresenceChangedCallbacks.Add(NotificationId, CallbackObj);
	}

	return NotificationId;
}

void UCorePresence::EOSPresenceRemoveNotifyOnPresenceChanged(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetPresenceHandle(worldContextObject))
	{
		EOS_Presence_RemoveNotifyOnPresenceChanged(GetPresenceHandle(worldContextObject), id);

		if (FPresenceChangedCallback** CallbackObj = s_PresenceChangedCallbacks.Find(id))
		{
			delete CallbackObj;
		}
		s_PresenceChangedCallbacks.Remove(id);
	}
}

struct FPresenceOnJoinGameAcceptedCallback
{
public:
	FPresenceOnJoinGameAcceptedCallback(UObject* worldContextObject, const FOnPresenceOnJoinGameAcceptedCallback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{
	}
	~FPresenceOnJoinGameAcceptedCallback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnPresenceOnJoinGameAcceptedCallback Callback;
};

static TMap<FEOSNotificationId, FPresenceOnJoinGameAcceptedCallback*> s_PresenceOnJoinGameAcceptedCallbacks;

FEOSNotificationId UCorePresence::EOSPresenceAddNotifyJoinGameAccepted(UObject* worldContextObject, const FOnPresenceOnJoinGameAcceptedCallback& callback)
{
	LogVerbose("");

	FEOSNotificationId NotificationId;

	if (GetPresenceHandle(worldContextObject))
	{
		EOS_Presence_AddNotifyJoinGameAcceptedOptions PresenceOptions = { EOS_PRESENCE_ADDNOTIFYJOINGAMEACCEPTED_API_LATEST };

		FPresenceOnJoinGameAcceptedCallback* CallbackObj = new FPresenceOnJoinGameAcceptedCallback(worldContextObject, callback);
		CallbackObj->Callback = callback;

		NotificationId = EOS_Presence_AddNotifyJoinGameAccepted(GetPresenceHandle(worldContextObject), &PresenceOptions, CallbackObj, ([](const EOS_Presence_JoinGameAcceptedCallbackInfo* data)
		{
			FPresenceOnJoinGameAcceptedCallback* CallbackData = static_cast<FPresenceOnJoinGameAcceptedCallback*>(data->ClientData);
			CallbackData->Callback.ExecuteIfBound(*data);
		}));

		s_PresenceOnJoinGameAcceptedCallbacks.Add(NotificationId, CallbackObj);
	}

	return NotificationId;
}

void UCorePresence::EOSPresenceRemoveNotifyJoinGameAccepted(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	if (GetPresenceHandle(worldContextObject))
	{
		EOS_Presence_RemoveNotifyJoinGameAccepted(GetPresenceHandle(worldContextObject), id);

		if (FPresenceOnJoinGameAcceptedCallback** CallbackObj = s_PresenceOnJoinGameAcceptedCallbacks.Find(id))
		{
			delete CallbackObj;
		}
		s_PresenceOnJoinGameAcceptedCallbacks.Remove(id);
	}
}

EOSResult UCorePresence::EOSPresenceGetJoinInfo(UObject* worldContextObject, FEOSPresenceGetJoinInfoOptions options, FString& outInfo)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetPresenceHandle(worldContextObject))
	{
		EOS_Presence_GetJoinInfoOptions PresenceOptions = { };
		PresenceOptions.ApiVersion = options.ApiVersion;
		PresenceOptions.LocalUserId = options.LocalUserId;

		char Buffer[EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH];
		int32_t Size = EOS_PRESENCEMODIFICATION_JOININFO_MAX_LENGTH;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Presence_GetJoinInfo(GetPresenceHandle(worldContextObject), &PresenceOptions, Buffer, &Size));

		if (Result == EOSResult::EOS_Success)
		{
			outInfo = Buffer;
		}
	}
	return Result;
}

EOSResult UCorePresence::EOSPresenceModificationSetStatus(UObject* worldContextObject, const FEOSHPresenceModification& handle, FEOSPresenceModificationSetStatusOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetPresenceHandle(worldContextObject))
	{
		EOS_PresenceModification_SetStatusOptions PresenceOptions = { };
		PresenceOptions.ApiVersion = options.ApiVersion;
		PresenceOptions.Status = static_cast<EOS_Presence_EStatus>(options.Status);

		Result = EOSHelpers::ToEOSCoreResult(EOS_PresenceModification_SetStatus(handle, &PresenceOptions));
	}

	return Result;
}

EOSResult UCorePresence::EOSPresenceModificationSetRawRichText(UObject* worldContextObject, const FEOSHPresenceModification& handle, FEOSPresenceModificationSetRawRichTextOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetPresenceHandle(worldContextObject))
	{
		char RichTextAnsi[256];
		FCStringAnsi::Strncpy(RichTextAnsi, TCHAR_TO_UTF8(*options.RichText), 256);

		EOS_PresenceModification_SetRawRichTextOptions PresenceOptions = { };
		PresenceOptions.ApiVersion = options.ApiVersion;
		PresenceOptions.RichText = RichTextAnsi;

		Result = EOSHelpers::ToEOSCoreResult(EOS_PresenceModification_SetRawRichText(handle, &PresenceOptions));
	}

	return Result;
}

struct FSetDataOperation : public EOS_PresenceModification_SetDataOptions
{
public:
	FSetDataOperation(int32 numRecords)
		: EOS_PresenceModification_SetDataOptions()
	{
		PointerArray.AddZeroed(numRecords);

		for (int32 i = 0; i < numRecords; i++)
		{
			PointerArray[i].ApiVersion = EOS_PRESENCEMODIFICATION_SETDATA_API_LATEST;
			PointerArray[i].Key = new char[256];
			PointerArray[i].Value = new char[256];
		}

		ApiVersion = EOS_PRESENCEMODIFICATION_SETDATA_API_LATEST;
		RecordsCount = numRecords;
		Records = PointerArray.GetData();
	}
	~FSetDataOperation()
	{
		for (int32 i = 0; i < PointerArray.Num(); i++)
		{
			delete[] PointerArray[i].Key;
			delete[] PointerArray[i].Value;
		}
	}
public:
	TArray<EOS_Presence_DataRecord> PointerArray;
};

EOSResult UCorePresence::EOSPresenceModificationSetData(UObject* worldContextObject, const FEOSHPresenceModification& handle, FEOSPresenceModificationSetDataOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetPresenceHandle(worldContextObject))
	{
		FSetDataOperation SetDataOperation(options.Records.Num());

		for (int32 i = 0; i < options.Records.Num(); i++)
		{
			FCStringAnsi::Strncpy(const_cast<char*>(SetDataOperation.PointerArray[i].Key), TCHAR_TO_UTF8(*options.Records[i].Key), (options.Records[i].Key.Len() +1));
			FCStringAnsi::Strncpy(const_cast<char*>(SetDataOperation.PointerArray[i].Value), TCHAR_TO_UTF8(*options.Records[i].Value), (options.Records[i].Value.Len() +1));
		}

		Result = EOSHelpers::ToEOSCoreResult(EOS_PresenceModification_SetData(handle, &SetDataOperation));
	}

	return Result;
}

EOSResult UCorePresence::EOSPresenceModificationDeleteData(UObject* worldContextObject, const FEOSHPresenceModification& handle, FEOSPresenceModificationDeleteDataOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetPresenceHandle(worldContextObject))
	{
		TArray<EOS_PresenceModification_DataRecordId> Records;
		Records.AddZeroed(options.Records.Num());

		for (int32 i = 0; i < options.Records.Num(); i++)
		{
			Records[i].ApiVersion = EOS_PRESENCEMODIFICATION_DATARECORDID_API_LATEST;
			Records[i].Key = new char[256];
			FCStringAnsi::Strncpy(const_cast<char*>(Records[i].Key), TCHAR_TO_UTF8(*options.Records[i].Key), (options.Records[i].Key.Len() + 1));
		}

		EOS_PresenceModification_DeleteDataOptions PresenceOptions = { };
		PresenceOptions.ApiVersion = options.ApiVersion;
		PresenceOptions.RecordsCount = options.Records.Num();
		PresenceOptions.Records = Records.GetData();

		Result = EOSHelpers::ToEOSCoreResult(EOS_PresenceModification_DeleteData(handle, &PresenceOptions));

		for (int32 i = 0; i < Records.Num(); i++)
		{
			delete[] Records[i].Key;
		}
	}

	return Result;
}

EOSResult UCorePresence::EOSPresenceModificationSetJoinInfo(UObject* worldContextObject, const FEOSHPresenceModification& handle, FEOSPresenceModificationSetJoinInfoOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetPresenceHandle(worldContextObject))
	{
		char JoinInfoAnsi[256];
		FCStringAnsi::Strncpy(JoinInfoAnsi, TCHAR_TO_UTF8(*options.JoinInfo), 256);

		EOS_PresenceModification_SetJoinInfoOptions PresenceOptions = { };
		PresenceOptions.ApiVersion = options.ApiVersion;
		PresenceOptions.JoinInfo = JoinInfoAnsi;
		
		Result = EOSHelpers::ToEOSCoreResult(EOS_PresenceModification_SetJoinInfo(handle, &PresenceOptions));
	}

	return Result;
}

void UCorePresence::EOSPresenceModificationRelease(UObject* worldContextObject, const FEOSHPresenceModification& handle)
{
	LogVerbose("");

	if (GetPresenceHandle(worldContextObject))
	{
		EOS_PresenceModification_Release(handle);
	}
}

void UCorePresence::Internal_OnPresenceQueryPresenceCallback(const EOS_Presence_QueryPresenceCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FQueryPresenceCompleteCallback* CallbackObj = static_cast<FQueryPresenceCompleteCallback*>(data->ClientData);
	check(CallbackObj);
	UCorePresence* Presence = CallbackObj->PresenceObject;
	check(Presence);

	CallbackObj->Callback.ExecuteIfBound(*data);

	delete CallbackObj;
}

void UCorePresence::Internal_OnPresenceSetPresenceCallback(const EOS_Presence_SetPresenceCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FSetPresenceCompleteCallback* CallbackObj = static_cast<FSetPresenceCompleteCallback*>(data->ClientData);
	check(CallbackObj);
	UCorePresence* Presence = CallbackObj->PresenceObject;
	check(Presence);

	CallbackObj->Callback.ExecuteIfBound(*data);

	delete CallbackObj;
}