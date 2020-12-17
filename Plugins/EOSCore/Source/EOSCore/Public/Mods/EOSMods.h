/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
 official EOSCore Documentation: https://eeldev.com
*/

#pragma once

#include "CoreMinimal.h"
#include "EOSCoreModule.h"
#include "eos_mods_types.h"
#include "Mods/EOSModsTypes.h"
#include "EOSMods.generated.h"

/**
 * The Mods Interface is used to manage a user's mods. Allowing a user to install/uninstall/enumerate mods.
 * All Mods Interface calls take a handle of type EOS_HMods as the first parameter.
 * This handle can be retrieved from a EOS_HPlatform handle by using the EOS_Platform_GetModsInterface function.
 *
 * NOTE: At this time, this feature is only available for desktop platforms and for products that are part of the Epic Games store.
 *
 * @see EOS_Platform_GetModsInterface
 */

UCLASS()
class EOSCORE_API UCoreMods : public UEOSCoreSubsystem
{
	GENERATED_BODY()
public:
    UCoreMods();
public:
	/**
	 * The Mods Interface is used to manage a user's mods. Allowing a user to install/uninstall/enumerate mods.
	 * All Mods Interface calls take a handle of type EOS_HMods as the first parameter.
	 * This handle can be retrieved from a EOS_HPlatform handle by using the EOS_Platform_GetModsInterface function.
	 *
	 * NOTE: At this time, this feature is only available for desktop platforms and for products that are part of the Epic Games store.
	 *
	 * @see EOS_Platform_GetModsInterface
	 */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "EOSCore|Interfaces")
		static UCoreMods* GetMods() { return s_ModsObject; }
public:
	/**
	 * Starts an asynchronous task that makes a request to install the specified mod.
	 *
	 * @param Options structure containing the game and mod identifiers
	 * @param ClientData arbitrary data that is passed back to you in the callback
	 * @param callback a callback that is fired when the async operation completes, either successfully or in error
	 *
	 * @see the section related to mods in eos_result.h for more details.
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Connect", meta = (WorldContext = "worldContextObject"))
		void EOSModsInstallMod(UObject* worldContextObject, FEOSModsInstallModOptions options, const FOnInstallModCallback& callback);

	/**
	 * Starts an asynchronous task that makes a request to uninstall the specified mod.
	 *
	 * @param Options structure containing the game and mod identifiers
	 * @param ClientData arbitrary data that is passed back to you in the callback
	 * @param callback a callback that is fired when the async operation completes, either successfully or in error
	 *
	 * @see the section related to mods in eos_result.h for more details.
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Connect", meta = (WorldContext = "worldContextObject"))
		void EOSModsUninstallMod(UObject* worldContextObject, FEOSModsUninstallModOptions options, const FOnUninstallModCallback& callback);

	/**
	 * Starts an asynchronous task that makes a request to enumerate mods for the specified game.
	 * Types of the mods to enumerate can be specified through EOSModsEnumerateModsOptions
	 *
	 * @param Options structure containing the game identifiers
	 * @param ClientData arbitrary data that is passed back to you in the callback
	 * @param callback a callback that is fired when the async operation completes, either successfully or in error
	 *
	 * @see the section related to mods in eos_result.h for more details.
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Connect", meta = (WorldContext = "worldContextObject"))
		void EOSModsEnumerateMods(UObject* worldContextObject, FEOSModsEnumerateModsOptions options, const FOnEnumerateModsCallback& callback);

	/**
	 * Get cached enumerated mods object. If successful, this data must be released by calling EOSModsModInfo_Release
	 * Types of the cached enumerated mods can be specified through EOSModsCopyModInfoOptions
	 *
	 * @param Options structure containing the game identifier for which requesting enumerated mods
	 * @param OutEnumeratedMods Enumerated mods Info. If the returned result is success, this will be set to data that must be later released, otherwise this will be set to NULL
	 * @return Success if we have cached data, or an error result if the request was invalid or we do not have cached data.
	 *
	 * @see EOSModsModInfo_Release
	 *
	 * This request may fail with an EOS_NotFound code if an enumeration of a certain type was not performed before this call.
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Connect", meta = (WorldContext = "worldContextObject"))
		EOSResult EOSModsCopyModInfo(UObject* worldContextObject, FEOSModsCopyModInfoOptions options, FEOSModsModInfo& OutEnumeratedMods);

	/**
	 * Starts an asynchronous task that makes a request to update the specified mod to the latest version.
	 *
	 * @param Options structure containing the game and mod identifiers
	 * @param ClientData arbitrary data that is passed back to you in the callback
	 * @param callback a callback that is fired when the async operation completes, either successfully or in error. If the mod is up to date then the operation will complete with success.
	 *
	 * @see the section related to mods in eos_result.h for more details.
	 */
	UFUNCTION(BlueprintCallable, Category = "EOSCore|Connect", meta = (WorldContext = "worldContextObject"))
		void EOSModsUpdateMod(UObject* worldContextObject, FEOSModsUpdateModOptions options, const FOnUpdateModCallback& callback);
private:
	static void Internal_OnInstallModCallback(const EOS_Mods_InstallModCallbackInfo* data);
	static void Internal_OnUninstallModCallback(const EOS_Mods_UninstallModCallbackInfo* data);
	static void Internal_OnEnumerateModsCallback(const EOS_Mods_EnumerateModsCallbackInfo* data);
	static void Internal_OnUpdateModCallback(const EOS_Mods_UpdateModCallbackInfo* data);
private:
    static UCoreMods* s_ModsObject;
};
