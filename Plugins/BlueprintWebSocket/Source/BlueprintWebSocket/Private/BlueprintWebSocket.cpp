// Copyright Charmillot Clement 2020. All Rights Reserved.

#include "BlueprintWebSocket.h"
#include "WebSocketsModule.h"
#include "IWebSocket.h"

DEFINE_LOG_CATEGORY(LogBlueprintWebSocket);

#define LOCTEXT_NAMESPACE "FBlueprintWebSocketModule"

void FBlueprintWebSocketModule::StartupModule()
{
	const FName WebSocketsModuleName = TEXT("WebSockets");

	if (!FModuleManager::Get().IsModuleLoaded(WebSocketsModuleName))
	{
		FWebSocketsModule& Module = FModuleManager::LoadModuleChecked<FWebSocketsModule>(WebSocketsModuleName);
	}
}

void FBlueprintWebSocketModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBlueprintWebSocketModule, BlueprintWebSocket)