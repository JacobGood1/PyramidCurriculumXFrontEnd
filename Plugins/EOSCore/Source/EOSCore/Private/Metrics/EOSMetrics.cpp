/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official EOSCore Documentation: https://eeldev.com
*/

#include "Metrics/EOSMetrics.h"
#include "Core/EOSCorePluginPrivatePCH.h"

UCoreMetrics* UCoreMetrics::s_MetricsObject = nullptr;

UCoreMetrics::UCoreMetrics()
{
	s_MetricsObject = this;
}

EOSResult UCoreMetrics::EOSMetricsBeginPlayerSession(UObject* worldContextObject, FEOSMetricsBeginPlayerSessionOptions options)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (EOS_HMetrics Metrics = GetMetricsHandle(worldContextObject))
	{
		EOS_Metrics_BeginPlayerSessionOptions MetricsOptions = { };
		MetricsOptions.ApiVersion = options.ApiVersion;
		MetricsOptions.AccountIdType = static_cast<EOS_EMetricsAccountIdType>(options.AccountIdType);
		MetricsOptions.AccountId.External = nullptr;
		MetricsOptions.GameSessionId = nullptr;
		MetricsOptions.ServerIp = nullptr;

		char ExternalAnsi[128];
		char DisplayNameAnsi[128];
		char GameSessionIdAnsi[128];
		char ServerIpAnsi[128];
		
		FCStringAnsi::Strncpy(ExternalAnsi, TCHAR_TO_UTF8(*options.External), 128);
		FCStringAnsi::Strncpy(DisplayNameAnsi, TCHAR_TO_UTF8(*options.DisplayName), 128);
		FCStringAnsi::Strncpy(GameSessionIdAnsi, TCHAR_TO_UTF8(*options.GameSessionId), 128);
		FCStringAnsi::Strncpy(ServerIpAnsi, TCHAR_TO_UTF8(*options.ServerIp), 128);
		
		switch (options.AccountIdType)
		{
		case EEOSEMetricsAccountIdType::EOS_MAIT_Epic:
			MetricsOptions.AccountId.Epic = options.Epic;
			break;
		case EEOSEMetricsAccountIdType::EOS_MAIT_External:
			MetricsOptions.AccountId.External = options.External.Len() > 0 ? ExternalAnsi : nullptr;
			break;
		}

		MetricsOptions.ControllerType = static_cast<EOS_EUserControllerType>(options.ControllerType);
		MetricsOptions.DisplayName = DisplayNameAnsi;
		MetricsOptions.GameSessionId = options.GameSessionId.Len() > 0 ? GameSessionIdAnsi : nullptr;
		MetricsOptions.ServerIp = options.ServerIp.Len() > 0 ? ServerIpAnsi : nullptr;
		
		Result = EOSHelpers::ToEOSCoreResult(EOS_Metrics_BeginPlayerSession(Metrics, &MetricsOptions));
	}

	return Result;
}

EOSResult UCoreMetrics::EOSMetricsEndPlayerSession(UObject* worldContextObject, FEOSMetricsEndPlayerSessionOptions options)
{
	EOSResult Result = EOSResult::EOS_ServiceFailure;

	if (EOS_HMetrics Metrics = GetMetricsHandle(worldContextObject))
	{
		char ExternalAnsi[128];
		FCStringAnsi::Strncpy(ExternalAnsi, TCHAR_TO_UTF8(*options.External), 128);

		EOS_Metrics_EndPlayerSessionOptions MetricsOptions = { };
		MetricsOptions.ApiVersion = options.ApiVersion;
		MetricsOptions.AccountIdType = static_cast<EOS_EMetricsAccountIdType>(options.AccountIdType);
		MetricsOptions.AccountId.External = nullptr;

		switch (options.AccountIdType)
		{
		case EEOSEMetricsAccountIdType::EOS_MAIT_Epic:
			MetricsOptions.AccountId.Epic = options.Epic;
			break;
		case EEOSEMetricsAccountIdType::EOS_MAIT_External:
			MetricsOptions.AccountId.External = options.External.Len() > 0 ? ExternalAnsi : nullptr;
			break;
		}

		Result = EOSHelpers::ToEOSCoreResult(EOS_Metrics_EndPlayerSession(Metrics, &MetricsOptions));
	}

	return Result;
}
