/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "UI/EOSUI.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLogging.h"

UCoreUI* UCoreUI::s_UIObject = nullptr;

UCoreUI::UCoreUI()
{
	s_UIObject = this;
}

void UCoreUI::EOSUIShowFriends(UObject* worldContextObject, FEOSUIShowFriendsOptions options, const FOnShowFriendsCallback& callback)
{
	LogVerbose("");

	if (GetUiHandle(worldContextObject))
	{
		EOS_UI_ShowFriendsOptions UIOptions = { options.ApiVersion, options.LocalUserId };
		FShowFriendsCallback* CallbackObj = new FShowFriendsCallback({ this, callback });
		EOS_UI_ShowFriends(GetUiHandle(worldContextObject), &UIOptions, CallbackObj, Internal_OnUIShowFriendsCallback);
	}
}

void UCoreUI::EOSUIHideFriends(UObject* worldContextObject, FEOSUIHideFriendsOptions options, const FOnHideFriendsCallback& callback)
{
	LogVerbose("");

	if (GetUiHandle(worldContextObject))
	{
		EOS_UI_HideFriendsOptions UIOptions = { options.ApiVersion, options.LocalUserId };
		FHideFriendsCallback* CallbackObj = new FHideFriendsCallback({ this, callback });
		EOS_UI_HideFriends(GetUiHandle(worldContextObject), &UIOptions, CallbackObj, Internal_OnUIHideFriendsCallback);
	}
}

bool UCoreUI::EOSUIGetFriendsVisible(UObject* worldContextObject, FEOSUIGetFriendsVisibleOptions options)
{
	LogVerbose("");

	bool bResult = false;

	if (GetUiHandle(worldContextObject))
	{
		EOS_UI_GetFriendsVisibleOptions UIOptions = { };
		UIOptions.ApiVersion = options.ApiVersion;
		UIOptions.LocalUserId = options.LocalUserId;

		bResult = (EOS_UI_GetFriendsVisible(GetUiHandle(worldContextObject), &UIOptions) > 0);
	}

	return bResult;
}

EOSResult UCoreUI::EOSUISetToggleFriendsKey(UObject* worldContextObject, FEOSUISetToggleFriendsKeyOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetUiHandle(worldContextObject))
	{
		EOS_UI_SetToggleFriendsKeyOptions UIOptions = { };
		UIOptions.ApiVersion = options.ApiVersion;

		EOS_UI_EKeyCombination Combination = EOS_UI_EKeyCombination::EOS_UIK_None;
		int32 KeyCombination = options.keyCombination;

		if ((KeyCombination & (int32)EOS_UI_EKeyCombination::EOS_UIK_Shift) != 0)
		{
			Combination = Combination | EOS_UI_EKeyCombination::EOS_UIK_Shift;
			KeyCombination -= (int32)EOS_UI_EKeyCombination::EOS_UIK_Shift;
		}

		if ((KeyCombination & (int32)EOS_UI_EKeyCombination::EOS_UIK_Control) != 0)
		{
			Combination = Combination | EOS_UI_EKeyCombination::EOS_UIK_Control;
			KeyCombination -= (int32)EOS_UI_EKeyCombination::EOS_UIK_Control;
		}

		if ((KeyCombination & (int32)EOS_UI_EKeyCombination::EOS_UIK_Alt) != 0)
		{
			Combination = Combination | EOS_UI_EKeyCombination::EOS_UIK_Alt;
			KeyCombination -= (int32)EOS_UI_EKeyCombination::EOS_UIK_Alt;
		}

		if ((KeyCombination & (int32)EOS_UI_EKeyCombination::EOS_UIK_Meta) != 0)
		{
			Combination = Combination | EOS_UI_EKeyCombination::EOS_UIK_Meta;
			KeyCombination -= (int32)EOS_UI_EKeyCombination::EOS_UIK_Meta;
		}

		if (KeyCombination > 0)
		{
			Combination |= (EOS_UI_EKeyCombination)KeyCombination;
			KeyCombination -= (int32)(EOS_UI_EKeyCombination)KeyCombination;
		}

		UIOptions.KeyCombination = Combination;

		Result = EOSHelpers::ToEOSCoreResult(EOS_UI_SetToggleFriendsKey(GetUiHandle(worldContextObject), &UIOptions));
	}

	return Result;
}

int32 UCoreUI::EOSUIGetToggleFriendsKey(UObject* worldContextObject, FEOSUIGetToggleFriendsKeyOptions options)
{
	LogVerbose("");

	int32 Result = 0;

	if (GetUiHandle(worldContextObject))
	{
		EOS_UI_GetToggleFriendsKeyOptions UIOptions = { };
		UIOptions.ApiVersion = options.ApiVersion;

		Result = (int32)EOS_UI_GetToggleFriendsKey(GetUiHandle(worldContextObject), &UIOptions);
	}

	return Result;
}

bool UCoreUI::EOSUIIsValidKeyCombination(UObject* worldContextObject, int32 keyCombination)
{
	LogVerbose("");

	bool bResult = false;

	if (GetUiHandle(worldContextObject))
	{
		EOS_UI_EKeyCombination Combination = EOS_UI_EKeyCombination::EOS_UIK_None;

		if ((keyCombination & (int32)EOS_UI_EKeyCombination::EOS_UIK_Shift) != 0)
		{
			Combination = Combination | EOS_UI_EKeyCombination::EOS_UIK_Shift;
			keyCombination -= (int32)EOS_UI_EKeyCombination::EOS_UIK_Shift;
		}

		if ((keyCombination & (int32)EOS_UI_EKeyCombination::EOS_UIK_Control) != 0)
		{
			Combination = Combination | EOS_UI_EKeyCombination::EOS_UIK_Control;
			keyCombination -= (int32)EOS_UI_EKeyCombination::EOS_UIK_Control;
		}

		if ((keyCombination & (int32)EOS_UI_EKeyCombination::EOS_UIK_Alt) != 0)
		{
			Combination = Combination | EOS_UI_EKeyCombination::EOS_UIK_Alt;
			keyCombination -= (int32)EOS_UI_EKeyCombination::EOS_UIK_Alt;
		}

		if ((keyCombination & (int32)EOS_UI_EKeyCombination::EOS_UIK_Meta) != 0)
		{
			Combination = Combination | EOS_UI_EKeyCombination::EOS_UIK_Meta;
			keyCombination -= (int32)EOS_UI_EKeyCombination::EOS_UIK_Meta;
		}

		if (keyCombination > 0)
		{
			Combination |= (EOS_UI_EKeyCombination)keyCombination;
			keyCombination -= (int32)(EOS_UI_EKeyCombination)keyCombination;
		}

		bResult = (EOS_UI_IsValidKeyCombination(GetUiHandle(worldContextObject), Combination) > 0);
	}

	return bResult;
}

EOSResult UCoreUI::EOSUISetDisplayPreference(UObject* worldContextObject, FEOSUISetDisplayPreferenceOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetUiHandle(worldContextObject))
	{
		EOS_UI_SetDisplayPreferenceOptions UIOptions = { };
		UIOptions.ApiVersion = options.ApiVersion;
		UIOptions.NotificationLocation = static_cast<EOS_UI_ENotificationLocation>(options.NotificationLocation);

		Result = EOSHelpers::ToEOSCoreResult(EOS_UI_SetDisplayPreference(GetUiHandle(worldContextObject), &UIOptions));
	}

	return Result;
}

EEOSUIENotificationLocation UCoreUI::EOSUIGetNotificationLocationPreference(UObject* worldContextObject)
{
	LogVerbose("");

	EEOSUIENotificationLocation Result = EEOSUIENotificationLocation::EOS_UNL_BottomLeft;

	if (GetUiHandle(worldContextObject))
	{
		Result = static_cast<EEOSUIENotificationLocation>(EOS_UI_GetNotificationLocationPreference(GetUiHandle(worldContextObject)));
	}

	return Result;
}

EOSResult UCoreUI::EOSUIAcknowledgeEventId(UObject* worldContextObject, FEOSUIAcknowledgeEventIdOptions options)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetUiHandle(worldContextObject))
	{
		EOS_UI_AcknowledgeEventIdOptions UIOptions = { };
		UIOptions.ApiVersion = options.ApiVersion;
		UIOptions.UiEventId = options.UiEventId;
		UIOptions.Result = static_cast<EOS_EResult>(options.Result);


		Result = EOSHelpers::ToEOSCoreResult(EOS_UI_AcknowledgeEventId(GetUiHandle(worldContextObject), &UIOptions));
	}

	return Result;
}

struct FDisplaySettingsUpdatedCallback
{
public:
	FDisplaySettingsUpdatedCallback(UObject* worldContextObject, const FOnDisplaySettingsUpdatedCallback& callback)
		: WorldContextObject(worldContextObject), Callback(callback)
	{
	}
	~FDisplaySettingsUpdatedCallback()
	{
		Callback.Unbind();
	}
public:
	UObject* WorldContextObject;
	FOnDisplaySettingsUpdatedCallback Callback;
};

static TMap<FEOSNotificationId, FDisplaySettingsUpdatedCallback*> s_DisplaySettingsUpdatedCallbacks;

FEOSNotificationId UCoreUI::EOSUIAddNotifyDisplaySettingsUpdated(UObject* worldContextObject, const FEOSUIAddNotifyDisplaySettingsUpdatedOptions options, const FOnDisplaySettingsUpdatedCallback& callback)
{
	LogVerbose("");

	EOS_UI_AddNotifyDisplaySettingsUpdatedOptions UIOptions = { };
	UIOptions.ApiVersion = options.ApiVersion;

	FDisplaySettingsUpdatedCallback* CallbackObj = new FDisplaySettingsUpdatedCallback(worldContextObject, callback);
	CallbackObj->Callback = callback;

	EOS_NotificationId Id = EOS_UI_AddNotifyDisplaySettingsUpdated(GetUiHandle(worldContextObject), &UIOptions, CallbackObj, ([](const EOS_UI_OnDisplaySettingsUpdatedCallbackInfo* data)
	{
		FDisplaySettingsUpdatedCallback* m_CallbackData = static_cast<FDisplaySettingsUpdatedCallback*>(data->ClientData);
		m_CallbackData->Callback.ExecuteIfBound(*data);
	}));

	s_DisplaySettingsUpdatedCallbacks.Add(Id, CallbackObj);

	return Id;
}


void UCoreUI::EOSUIRemoveNotifyDisplaySettingsUpdated(UObject* worldContextObject, FEOSNotificationId id)
{
	LogVerbose("");

	EOS_UI_RemoveNotifyDisplaySettingsUpdated(GetUiHandle(worldContextObject), id);
	if (FDisplaySettingsUpdatedCallback** CallbackObj = s_DisplaySettingsUpdatedCallbacks.Find(id))
	{
		delete CallbackObj;
	}
	s_DisplaySettingsUpdatedCallbacks.Remove(id);
}

void UCoreUI::Internal_OnUIShowFriendsCallback(const EOS_UI_ShowFriendsCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FShowFriendsCallback* CallbackObj = static_cast<FShowFriendsCallback*>(data->ClientData);
	check(CallbackObj);
	UCoreUI* UI = CallbackObj->UIObject;
	check(UI);

	CallbackObj->Callback.ExecuteIfBound(*data);

	delete CallbackObj;
}

void UCoreUI::Internal_OnUIHideFriendsCallback(const EOS_UI_HideFriendsCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FHideFriendsCallback* CallbackObj = static_cast<FHideFriendsCallback*>(data->ClientData);
	check(CallbackObj);
	UCoreUI* UI = CallbackObj->UIObject;
	check(UI);

	CallbackObj->Callback.ExecuteIfBound(*data);

	delete CallbackObj;
}