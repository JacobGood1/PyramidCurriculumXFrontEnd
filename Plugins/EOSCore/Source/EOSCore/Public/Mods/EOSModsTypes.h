/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#pragma once

#include "CoreMinimal.h"
#include "eos_mods_types.h"
#include "Core/EOSHelpers.h"
#include "EOSModsTypes.generated.h"

class UCoreMods;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		Enums
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
/** The type of mod enumeration. */
UENUM(BlueprintType)
enum class EEOSEModEnumerationType : uint8
{
	/** Installed mods */
	EOS_MET_INSTALLED = 0,
	/** All available mods*/
	EOS_MET_ALL_AVAILABLE
};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		Structs
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
/**
 * EOS_Mod_Identifier is used to identify a mod.
 */
USTRUCT(BlueprintType)
struct FEOSModIdentifier
{
	GENERATED_BODY()
public:
	/** API Version. */
	int32 ApiVersion;
public:
	/** Product namespace id in which this mod item exists */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FString NamespaceId;
	/* Item id of the Mod */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FString ItemId;
	/* Artifact id of the Mod */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FString ArtifactId;
	/** Represent mod item title. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FString Title;
	/** Represent mod item version. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FString Version;
public:
	explicit FEOSModIdentifier()
		: ApiVersion(EOS_MOD_IDENTIFIER_API_LATEST) { }
	FEOSModIdentifier(const EOS_Mod_Identifier& data) 
		: NamespaceId(UTF8_TO_TCHAR(data.NamespaceId))
		, ItemId(UTF8_TO_TCHAR(data.ItemId))
		, ArtifactId(UTF8_TO_TCHAR(data.ArtifactId))
		, Title(UTF8_TO_TCHAR(data.Title))
		, Version(UTF8_TO_TCHAR(data.Version))
	{}
};

/**
 * Input parameters for the EOS_Mods_InstallMod Function.
 */
USTRUCT(BlueprintType)
struct FEOSModsInstallModOptions
{
	GENERATED_BODY()
public:
	/** API Version. */
	int32 ApiVersion;
public:
	/** The Epic Online Services Account ID of the user for which the mod should be installed */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FEOSEpicAccountId LocalUserId;
	/** The mod to install */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FEOSModIdentifier Mod;
	/** Indicates whether the mod should be uninstalled after exiting the game or not. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		bool bRemoveAfterExit;
public:
	explicit FEOSModsInstallModOptions()
		: ApiVersion(EOS_MODS_INSTALLMOD_API_LATEST)
		, bRemoveAfterExit(false)
	{}
};

/**
 * Output parameters for the EOS_Mods_InstallMod Function. These parameters are received through the callback provided to EOS_Mods_InstallMod
 */
USTRUCT(BlueprintType)
struct FEOSModsInstallModCallbackInfo
{
	GENERATED_BODY()
public:
	/** Result code for the operation. EOS_Success is returned for a successful query, otherwise one of the error codes is returned. See eos_result.h */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		EOSResult ResultCode;
	/** The Epic Online Services Account ID of the user for which mod installation was requested */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FEOSEpicAccountId LocalUserId;
	/** Context that was passed into to EOS_Mods_InstallMod */
		void* ClientData;
	/** Mod for which installation was requested */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FEOSModIdentifier Mod;
public:
	explicit FEOSModsInstallModCallbackInfo()
		: ResultCode(EOSResult::EOS_ServiceFailure)
		, ClientData(nullptr) {}
	FEOSModsInstallModCallbackInfo(const EOS_Mods_InstallModCallbackInfo& data)
		: ResultCode(EOSHelpers::ToEOSCoreResult(data.ResultCode))
		, LocalUserId(data.LocalUserId)
		, ClientData(data.ClientData)
		, Mod(*data.Mod)
	{}
};

/**
 * Input parameters for the EOS_Mods_UninstallMod Function.
 */
USTRUCT(BlueprintType)
struct FEOSModsUninstallModOptions
{
	GENERATED_BODY()
public:
	/** API Version. */
	int32 ApiVersion;
public:
	/** The Epic Online Services Account ID of the user for which the mod should be uninstalled */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FEOSEpicAccountId LocalUserId;
	/** The mod to uninstall */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FEOSModIdentifier Mod;
public:
	explicit FEOSModsUninstallModOptions()
		: ApiVersion(EOS_MODS_UNINSTALLMOD_API_LATEST)
	{}
};

/**
 * Output parameters for the EOS_Mods_UninstallMod Function. These parameters are received through the callback provided to EOS_Mods_UninstallMod
 */
USTRUCT(BlueprintType)
struct FEOSModsUninstallModCallbackInfo
{
	GENERATED_BODY()
public:
	/** Result code for the operation. EOS_Success is returned for a successful query, otherwise one of the error codes is returned. See eos_result.h */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		EOSResult ResultCode;
	/** The Epic Online Services Account ID of the user for which mod uninstallation was requested */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FEOSEpicAccountId LocalUserId;
	/** Context that was passed into to EOS_Mods_UninstallMod */
		void* ClientData;
	/** Mod for which uninstallation was requested */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FEOSModIdentifier Mod;
public:
	explicit FEOSModsUninstallModCallbackInfo()
		: ResultCode(EOSResult::EOS_ServiceFailure)
		, ClientData(nullptr) {}
	FEOSModsUninstallModCallbackInfo(const EOS_Mods_UninstallModCallbackInfo& data)
		: ResultCode(EOSHelpers::ToEOSCoreResult(data.ResultCode))
		, LocalUserId(data.LocalUserId)
		, ClientData(data.ClientData)
		, Mod(*data.Mod)
	{}
};

/**
 * Input parameters for the EOS_Mods_EnumerateMods Function.
 */
USTRUCT(BlueprintType)
struct FEOSModsEnumerateModsOptions
{
	GENERATED_BODY()
public:
	/** API Version. */
	int32 ApiVersion;
public:
	/** The Epic Online Services Account ID of the user for which the mod should be enumerated */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FEOSEpicAccountId LocalUserId;
	/** Type of the mods to enumerate */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		EEOSEModEnumerationType Type;
public:
	explicit FEOSModsEnumerateModsOptions()
		: ApiVersion(EOS_MODS_ENUMERATEMODS_API_LATEST)
		, Type(EEOSEModEnumerationType::EOS_MET_ALL_AVAILABLE)
	{}
};

/**
 * Output parameters for the EOS_Mods_EnumerateMods Function. These parameters are received through the callback provided to EOS_Mods_EnumerateMods
 */
USTRUCT(BlueprintType)
struct FEOSModsEnumerateModsCallbackInfo
{
	GENERATED_BODY()
public:
	/** Result code for the operation. EOS_Success is returned for a successful query, otherwise one of the error codes is returned. See eos_result.h */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		EOSResult ResultCode;
	/** The Epic Online Services Account ID of the user for which mod enumeration was requested */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FEOSEpicAccountId LocalUserId;
	/** Context that was passed into to EOS_Mods_EnumerateMods */
		void* ClientData;
	/** Type of the enumerated mods */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		EEOSEModEnumerationType Type;
public:
	explicit FEOSModsEnumerateModsCallbackInfo()
		: ResultCode(EOSResult::EOS_ServiceFailure)
		, ClientData(nullptr) 
		, Type(EEOSEModEnumerationType::EOS_MET_ALL_AVAILABLE)
	{}
	FEOSModsEnumerateModsCallbackInfo(const EOS_Mods_EnumerateModsCallbackInfo& data)
		: ResultCode(EOSHelpers::ToEOSCoreResult(data.ResultCode))
		, LocalUserId(data.LocalUserId)
		, ClientData(data.ClientData)
		, Type(static_cast<EEOSEModEnumerationType>(data.Type))
	{}
};

/**
 * Data for the EOS_Mods_CopyModInfo function.
 */
USTRUCT(BlueprintType)
struct FEOSModsCopyModInfoOptions
{
	GENERATED_BODY()
public:
	/** API Version. */
	int32 ApiVersion;
public:
	/** The Epic Online Services Account ID of the user for which mods should be copied */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FEOSEpicAccountId LocalUserId;
	/** Type of the enumerated mod to copy */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		EEOSEModEnumerationType Type;
public:
	explicit FEOSModsCopyModInfoOptions()
		: ApiVersion(EOS_MODS_COPYMODINFO_API_LATEST)
		, Type(EEOSEModEnumerationType::EOS_MET_ALL_AVAILABLE)
	{}
};

/**
 * Data for the EOS_Mods_CopyModInfo function.
 *
 * @see EOS_Mods_CopyModInfo
 * @see EOS_Mods_ModInfo_Release
 */
USTRUCT(BlueprintType)
struct FEOSModsModInfo
{
	GENERATED_BODY()
public:
	/** API Version. */
	int32 ApiVersion;
public:
	/** The array of enumerated mods or NULL if no such type of mods were enumerated */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		TArray<FEOSModIdentifier> Mods;
	/** Type of the mods */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		EEOSEModEnumerationType Type;
public:
	explicit FEOSModsModInfo()
		: ApiVersion(EOS_MODS_COPYMODINFO_API_LATEST)
		, Type(EEOSEModEnumerationType::EOS_MET_ALL_AVAILABLE)
	{}
};

/**
 * Input parameters for the EOS_Mods_UpdateMod Function.
 */
USTRUCT(BlueprintType)
struct FEOSModsUpdateModOptions
{
	GENERATED_BODY()
public:
	/** API Version. */
	int32 ApiVersion;
public:
	/** The Epic Online Services Account ID of the user for which the mod should be updated */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FEOSEpicAccountId LocalUserId;
	/** The mod to update */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FEOSModIdentifier Mod;
public:
	explicit FEOSModsUpdateModOptions()
		: ApiVersion(EOS_MODS_UPDATEMOD_API_LATEST)
	{}
};

/**
 * Output parameters for the EOS_Mods_UpdateMod Function. These parameters are received through the callback provided to EOS_Mods_UpdateMod
 */
USTRUCT(BlueprintType)
struct FEOSModsUpdateModCallbackInfo
{
	GENERATED_BODY()
public:
	/** Result code for the operation. EOS_Success is returned for a successful query, otherwise one of the error codes is returned. See eos_result.h */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		EOSResult ResultCode;
	/** The Epic Online Services Account ID of the user for which mod update was requested */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FEOSEpicAccountId LocalUserId;
	/** Context that was passed into to EOS_Mods_UpdateMod */
		void* ClientData;
	/** Mod for which update was requested */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mods")
		FEOSModIdentifier Mod;
public:
	explicit FEOSModsUpdateModCallbackInfo()
		: ResultCode(EOSResult::EOS_ServiceFailure)
		, ClientData(nullptr) {}
	FEOSModsUpdateModCallbackInfo(const EOS_Mods_UpdateModCallbackInfo& data)
		: ResultCode(EOSHelpers::ToEOSCoreResult(data.ResultCode))
		, LocalUserId(data.LocalUserId)
		, ClientData(data.ClientData)
		, Mod(*data.Mod)
	{}
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnInstallModCallback, const FEOSModsInstallModCallbackInfo&, data);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUninstallModCallback, const FEOSModsUninstallModCallbackInfo&, data);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnEnumerateModsCallback, const FEOSModsEnumerateModsCallbackInfo&, data);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUpdateModCallback, const FEOSModsUpdateModCallbackInfo&, data);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInstallModCallbackDelegate, const FEOSModsInstallModCallbackInfo&, data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUninstallModCallbackDelegate, const FEOSModsUninstallModCallbackInfo&, data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnumerateModsCallbackDelegate, const FEOSModsEnumerateModsCallbackInfo&, data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateModCallbackDelegate, const FEOSModsUpdateModCallbackInfo&, data);