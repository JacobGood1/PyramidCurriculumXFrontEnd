/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Mods/EOSMods.h"
#include "Core/EOSCorePluginPrivatePCH.h"
#include "Core/EOSCoreLogging.h"

UCoreMods* UCoreMods::s_ModsObject = nullptr;

UCoreMods::UCoreMods()
{
	s_ModsObject = this;
}

struct FInstallModCallback
{
public:
	UCoreMods* ModsObject;
	FOnInstallModCallback Callback;
public:
	FInstallModCallback(UCoreMods* obj, const FOnInstallModCallback& callback)
		: ModsObject(obj)
		, Callback(callback)
	{}
};

void UCoreMods::EOSModsInstallMod(UObject* worldContextObject, FEOSModsInstallModOptions options, const FOnInstallModCallback& callback)
{
	LogVerbose("");

	if (GetModsHandle(worldContextObject))
	{
		EOS_Mods_InstallModOptions ModsOptions = { };
		ModsOptions.ApiVersion = EOS_MODS_INSTALLMOD_API_LATEST;
		ModsOptions.LocalUserId = options.LocalUserId;

		char NamespaceIdAnsi[128];
		char ItemIdAnsi[128];
		char ArtifactIdAnsi[128];
		char TitleAnsi[128];
		char VersionAnsi[128];

		FCStringAnsi::Strncpy(NamespaceIdAnsi, TCHAR_TO_UTF8(*options.Mod.NamespaceId), 128);
		FCStringAnsi::Strncpy(ItemIdAnsi, TCHAR_TO_UTF8(*options.Mod.ItemId), 128);
		FCStringAnsi::Strncpy(ArtifactIdAnsi, TCHAR_TO_UTF8(*options.Mod.ArtifactId), 128);
		FCStringAnsi::Strncpy(TitleAnsi, TCHAR_TO_UTF8(*options.Mod.Title), 128);
		FCStringAnsi::Strncpy(VersionAnsi, TCHAR_TO_UTF8(*options.Mod.Version), 128);

		EOS_Mod_Identifier Mod;
		Mod.ApiVersion = { EOS_MOD_IDENTIFIER_API_LATEST };
		Mod.NamespaceId = NamespaceIdAnsi;
		Mod.ItemId = ItemIdAnsi;
		Mod.ArtifactId = ArtifactIdAnsi;
		Mod.Title = TitleAnsi;
		Mod.Version = VersionAnsi;

		ModsOptions.Mod = &Mod;
		ModsOptions.bRemoveAfterExit = options.bRemoveAfterExit;

		FInstallModCallback* CallbackObj = new FInstallModCallback({ this, callback });
		EOS_Mods_InstallMod(GetModsHandle(worldContextObject), &ModsOptions, CallbackObj, Internal_OnInstallModCallback);
	}
}

struct FUninstallModCallback
{
public:
	UCoreMods* ModsObject;
	FOnUninstallModCallback Callback;
public:
	FUninstallModCallback(UCoreMods* obj, const FOnUninstallModCallback& callback)
		: ModsObject(obj)
		, Callback(callback)
	{
	}
};

void UCoreMods::EOSModsUninstallMod(UObject* worldContextObject, FEOSModsUninstallModOptions options, const FOnUninstallModCallback& callback)
{
	LogVerbose("");

	if (GetModsHandle(worldContextObject))
	{
		EOS_Mods_UninstallModOptions ModsOptions = { };
		ModsOptions.ApiVersion = EOS_MODS_UNINSTALLMOD_API_LATEST;
		ModsOptions.LocalUserId = options.LocalUserId;

		EOS_Mod_Identifier Mod = { };
		Mod.ApiVersion = EOS_MOD_IDENTIFIER_API_LATEST;

		char NamespaceIdAnsi[128];
		char ItemIdAnsi[128];
		char ArtifactIdAnsi[128];
		char TitleAnsi[128];
		char VersionAnsi[128];

		FCStringAnsi::Strncpy(NamespaceIdAnsi, TCHAR_TO_UTF8(*options.Mod.NamespaceId), 128);
		FCStringAnsi::Strncpy(ItemIdAnsi, TCHAR_TO_UTF8(*options.Mod.ItemId), 128);
		FCStringAnsi::Strncpy(ArtifactIdAnsi, TCHAR_TO_UTF8(*options.Mod.ArtifactId), 128);
		FCStringAnsi::Strncpy(TitleAnsi, TCHAR_TO_UTF8(*options.Mod.Title), 128);
		FCStringAnsi::Strncpy(VersionAnsi, TCHAR_TO_UTF8(*options.Mod.Version), 128);

		Mod.NamespaceId = NamespaceIdAnsi;
		Mod.ItemId = ItemIdAnsi;
		Mod.ArtifactId = ArtifactIdAnsi;
		Mod.Title = TitleAnsi;
		Mod.Version = VersionAnsi;

		ModsOptions.Mod = &Mod;

		FUninstallModCallback* CallbackObj = new FUninstallModCallback({ this, callback });
		EOS_Mods_UninstallMod(GetModsHandle(worldContextObject), &ModsOptions, CallbackObj, Internal_OnUninstallModCallback);
	}
}

struct FEnumerateModsCallback
{
public:
	UCoreMods* ModsObject;
	FOnEnumerateModsCallback Callback;
public:
	FEnumerateModsCallback(UCoreMods* obj, const FOnEnumerateModsCallback& callback)
		: ModsObject(obj)
		, Callback(callback)
	{
	}
};


void UCoreMods::EOSModsEnumerateMods(UObject* worldContextObject, FEOSModsEnumerateModsOptions options, const FOnEnumerateModsCallback& callback)
{
	LogVerbose("");

	if (GetModsHandle(worldContextObject))
	{
		EOS_Mods_EnumerateModsOptions ModsOptions = { };
		ModsOptions.ApiVersion = EOS_MODS_ENUMERATEMODS_API_LATEST;
		ModsOptions.LocalUserId = options.LocalUserId;
		ModsOptions.Type = static_cast<EOS_EModEnumerationType>(options.Type);

		FEnumerateModsCallback* CallbackObj = new FEnumerateModsCallback({ this, callback });
		EOS_Mods_EnumerateMods(GetModsHandle(worldContextObject), &ModsOptions, CallbackObj, Internal_OnEnumerateModsCallback);
	}
}

EOSResult UCoreMods::EOSModsCopyModInfo(UObject* worldContextObject, FEOSModsCopyModInfoOptions options, FEOSModsModInfo& OutEnumeratedMods)
{
	LogVerbose("");

	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (GetModsHandle(worldContextObject))
	{
		EOS_Mods_CopyModInfoOptions ModsOptions = { };
		ModsOptions.ApiVersion = EOS_MODS_COPYMODINFO_API_LATEST;
		ModsOptions.LocalUserId = options.LocalUserId;
		ModsOptions.Type = static_cast<EOS_EModEnumerationType>(options.Type);

		EOS_Mods_ModInfo* ModInfo = NULL;

		Result = EOSHelpers::ToEOSCoreResult(EOS_Mods_CopyModInfo(GetModsHandle(worldContextObject), &ModsOptions, &ModInfo));

		if (Result == EOSResult::EOS_Success)
		{
			OutEnumeratedMods.ApiVersion = { EOS_MODS_COPYMODINFO_API_LATEST };

			for (int32 i = 0; i < ModInfo->ModsCount; i++)
			{
				OutEnumeratedMods.Mods.Add(ModInfo->Mods[i]);
			}

			OutEnumeratedMods.Type = static_cast<EEOSEModEnumerationType>(ModInfo->Type);

			EOS_Mods_ModInfo_Release(ModInfo);
		}
	}

	return Result;
}

struct FUpdateModCallback
{
public:
	UCoreMods* ModsObject;
	FOnUpdateModCallback Callback;
public:
	FUpdateModCallback(UCoreMods* obj, const FOnUpdateModCallback& callback)
		: ModsObject(obj)
		, Callback(callback)
	{
	}
};

void UCoreMods::EOSModsUpdateMod(UObject* worldContextObject, FEOSModsUpdateModOptions options, const FOnUpdateModCallback& callback)
{
	LogVerbose("");

	if (GetModsHandle(worldContextObject))
	{
		EOS_Mods_UpdateModOptions ModsOptions = { };
		ModsOptions.ApiVersion = EOS_MODS_UPDATEMOD_API_LATEST;
		ModsOptions.LocalUserId = options.LocalUserId;

		EOS_Mod_Identifier Mod = { };
		Mod.ApiVersion = EOS_MOD_IDENTIFIER_API_LATEST;

		char NamespaceIdAnsi[128];
		char ItemIdAnsi[128];
		char ArtifactIdAnsi[128];
		char TitleAnsi[128];
		char VersionAnsi[128];

		FCStringAnsi::Strncpy(NamespaceIdAnsi, TCHAR_TO_UTF8(*options.Mod.NamespaceId), 128);
		FCStringAnsi::Strncpy(ItemIdAnsi, TCHAR_TO_UTF8(*options.Mod.ItemId), 128);
		FCStringAnsi::Strncpy(ArtifactIdAnsi, TCHAR_TO_UTF8(*options.Mod.ArtifactId), 128);
		FCStringAnsi::Strncpy(TitleAnsi, TCHAR_TO_UTF8(*options.Mod.Title), 128);
		FCStringAnsi::Strncpy(VersionAnsi, TCHAR_TO_UTF8(*options.Mod.Version), 128);

		Mod.NamespaceId = NamespaceIdAnsi;
		Mod.ItemId = ItemIdAnsi;
		Mod.ArtifactId = ArtifactIdAnsi;
		Mod.Title = TitleAnsi;
		Mod.Version = VersionAnsi;

		ModsOptions.Mod = &Mod;

		FUpdateModCallback* CallbackObj = new FUpdateModCallback({ this, callback });
		EOS_Mods_UpdateMod(GetModsHandle(worldContextObject), &ModsOptions, CallbackObj, Internal_OnUpdateModCallback);
	}
}

void UCoreMods::Internal_OnInstallModCallback(const EOS_Mods_InstallModCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FInstallModCallback* CallbackObj = static_cast<FInstallModCallback*>(data->ClientData);
	check(CallbackObj);

	if (CallbackObj)
	{
		CallbackObj->Callback.ExecuteIfBound(*data);
		delete CallbackObj;
	}
}

void UCoreMods::Internal_OnUninstallModCallback(const EOS_Mods_UninstallModCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FUninstallModCallback* CallbackObj = static_cast<FUninstallModCallback*>(data->ClientData);
	check(CallbackObj);

	if (CallbackObj)
	{
		CallbackObj->Callback.ExecuteIfBound(*data);
		delete CallbackObj;
	}
}

void UCoreMods::Internal_OnEnumerateModsCallback(const EOS_Mods_EnumerateModsCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FEnumerateModsCallback* CallbackObj = static_cast<FEnumerateModsCallback*>(data->ClientData);
	check(CallbackObj);

	if (CallbackObj)
	{
		CallbackObj->Callback.ExecuteIfBound(*data);
		delete CallbackObj;
	}
}

void UCoreMods::Internal_OnUpdateModCallback(const EOS_Mods_UpdateModCallbackInfo* data)
{
	LogVerbose("%s", *FString(EOS_EResult_ToString(data->ResultCode)));

	FUpdateModCallback* CallbackObj = static_cast<FUpdateModCallback*>(data->ClientData);
	check(CallbackObj);

	if (CallbackObj)
	{
		CallbackObj->Callback.ExecuteIfBound(*data);
		delete CallbackObj;
	}
}
