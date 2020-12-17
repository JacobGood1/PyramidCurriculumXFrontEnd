/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "CreateEOSSessionCallbackProxy.generated.h"

class APlayerController;

UCLASS(MinimalAPI)
class UCreateEOSSessionCallbackProxy : public UOnlineBlueprintCallProxyBase
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FEmptyOnlineDelegate OnSuccess;
	UPROPERTY(BlueprintAssignable)
		FEmptyOnlineDelegate OnFailure;
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "worldContextObject"), Category = "Online|Session")
		static UCreateEOSSessionCallbackProxy* Create_EOS_Session(UObject* worldContextObject, FString sessionName, int32 publicConnections);
public:
	// UOnlineBlueprintCallProxyBase interface
	virtual void Activate() override;
	// End of UOnlineBlueprintCallProxyBase interface
private:
	void OnCreateCompleted(FName SessionName, bool bWasSuccessful);
	void OnStartCompleted(FName SessionName, bool bWasSuccessful);
private:
	FOnCreateSessionCompleteDelegate m_CreateCompleteDelegate;
	FOnStartSessionCompleteDelegate m_StartCompleteDelegate;
	FDelegateHandle m_CreateCompleteDelegateHandle;
	FDelegateHandle m_StartCompleteDelegateHandle;
private:
	int m_NumPublicConnections;
	UObject* m_WorldContextObject;
	FString m_SessionName;
};
