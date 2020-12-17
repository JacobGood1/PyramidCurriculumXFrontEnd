/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#pragma once

#include "Mods/EOSModsTypes.h"
#include "Core/EOSCoreAsync.h"
//#include "EOSModsAsyncActions.generated.h"

/*
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
//		UEOSCoreModsQueryMods
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //
UCLASS()
class EOSCORE_API UEOSCoreModsQueryMods : public UEOSCoreAsyncAction
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
        FOnQueryModsCallbackDelegate OnCallback;
private:
    FOnQueryModsCallback Callback;
public:
    UEOSCoreModsQueryMods() { Callback.BindUFunction(this, "HandleCallback"); }
    ~UEOSCoreModsQueryMods() { Callback.Unbind(); }
public:
    / **
    * EOS_Mods_QueryMods is used to start an asynchronous query to retrieve information, such as display name, about another account.
    * Once the callback has been fired with a successful ResultCode, it is possible to call EOS_Mods_CopyMods to receive an EOS_Mods containing the available information.
    *
    * @param Options structure containing the input parameters
    * @param ClientData arbitrary data that is passed back to you in the callback
    * @param callback a callback that is fired when the async operation completes, either successfully or in error
    *
    * @see EOS_Mods
    * @see EOS_Mods_CopyMods
    * @see EOS_Mods_QueryModsOptions
    * @see EOS_Mods_OnQueryModsCallback
    * /
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", Category = "EOSCore|Mods|Async"))
		static UEOSCoreModsQueryMods* EOSModsQueryModsAsync(UObject* WorldContextObject, FEOSModsQueryModsOptions options);
protected:
    FEOSModsQueryModsOptions Options;
public:
	virtual void Activate() override;
private:
	UFUNCTION()
        void HandleCallback(const FEOSModsQueryModsCallbackInfo& data, bool bWasSuccessful)
    {
		OnCallback.Broadcast(data);
		Callback.Unbind();
		SetReadyToDestroy();
    }
};*/