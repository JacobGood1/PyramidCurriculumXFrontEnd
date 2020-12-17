/**
* Copyright (C) 2017-2020 | Dry Eel Development
*
* Official SteamCore Documentation: https://eeldev.com
*/

#include "EOSSettings.h"

#define LOCTEXT_NAMESPACE "FEOSCoreModule"

UEOSCoreSettings::UEOSCoreSettings()
	: DefaultConfigurationName("DefaultConfig")
	, TickBudgetInMilliseconds(0)
{}

bool UEOSCoreSettings::GetEOSConfig(const FString& ConfigName, FEOSConfig& OutSettings)
{
	TArray<FString> Configurations;
	GConfig->GetArray(INI_SECTION, TEXT("EOSConfigurations"), Configurations, GEngineIni);

	TArray<FEOSConfig> LoadedConfigurations;

	for (FString& Line : Configurations)
	{
		FEOSConfig Config;
		Config.ParseRawArrayEntry(Line);
		LoadedConfigurations.Add(Config);
	}

	FString ConfigOverrideName;
	FParse::Value(FCommandLine::Get(), TEXT("EOSConfig="), ConfigOverrideName);

	if (ConfigOverrideName.IsEmpty())
	{
		ConfigOverrideName = ConfigName;
	}
	
	UE_LOG_ONLINE(Log, TEXT("GetEOSConfig(): Finding configuration name: (%s)"), *ConfigName);
	
	for (FEOSConfig& Config : LoadedConfigurations)
	{
		if (Config.ConfigName == ConfigOverrideName)
		{
			OutSettings = Config;
			return true;
		}
	}

	FString DefaultConfigurationName;
	GConfig->GetString(INI_SECTION, TEXT("DefaultConfigurationName"), DefaultConfigurationName, GEngineIni);

	UE_LOG_ONLINE(Log, TEXT("GetEOSConfig(): No override configuration found, attempting to find a DefaultConfiguration: (%s)"), *DefaultConfigurationName);

	for (FEOSConfig& Config : LoadedConfigurations)
	{
		if (Config.ConfigName == DefaultConfigurationName)
		{
			OutSettings = Config;
			return true;
		}
	}

	UE_LOG_ONLINE(Warning, TEXT("GetEOSConfig(): Did not find any EOS Configuration. Check your Project Settings! Attempted Settings: Config: (%s), OverrideConfig: (%s), DefaultConfigurationName: (%s)"), *ConfigName, *ConfigOverrideName, *DefaultConfigurationName);

	return false;
}

#if WITH_EDITOR
void UEOSCoreSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property == nullptr)
	{
		Super::PostEditChangeProperty(PropertyChangedEvent);
		return;
	}

	if (PropertyChangedEvent.MemberProperty != nullptr &&
		PropertyChangedEvent.MemberProperty->GetFName() == FName(TEXT("EOSConfigurations")) &&
		(PropertyChangedEvent.ChangeType & EPropertyChangeType::ValueSet))
	{
		for (FEOSConfig& Artifact : EOSConfigurations)
		{
			if (!Artifact.ClientId.IsEmpty())
			{
				if (!Artifact.ClientId.StartsWith(TEXT("xyz")))
				{
					FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ClientIdInvalidMsg", "Client ids created after SDK version 1.5 start with xyz. Double check that you did not use your BPT Client Id instead."));
				}
				if (!IsAnsi(Artifact.ClientId) || ContainsWhitespace(Artifact.ClientId))
				{
					FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ClientIdNotAnsiMsg", "Client ids must contain ANSI printable characters only with no whitespace"));
					Artifact.ClientId.Empty();
				}
			}

			if (!Artifact.ClientSecret.IsEmpty())
			{
				if (!IsAnsi(Artifact.ClientSecret) || ContainsWhitespace(Artifact.ClientSecret))
				{
					FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ClientSecretNotAnsiMsg", "ClientSecret must contain ANSI printable characters only with no whitespace"));
					Artifact.ClientSecret.Empty();
				}
			}

			if (!Artifact.EncryptionKey.IsEmpty())
			{
				if (!IsHex(Artifact.EncryptionKey) || Artifact.EncryptionKey.Len() != 64)
				{
					FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("EncryptionKeyNotHexMsg", "EncryptionKey must contain 64 hex characters"));
					Artifact.EncryptionKey.Empty();
				}
			}
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

FName UEOSCoreSettings::GetCategoryName() const
{
	return NAME_Game;
}

#endif

#undef LOCTEXT_NAMESPACE

void FEOSConfig::ParseRawArrayEntry(FString& RawLine)
{
	const TCHAR* Delims[4] = { TEXT("("), TEXT(")"), TEXT("="), TEXT(",") };
	TArray<FString> Values;
	RawLine.ParseIntoArray(Values, Delims, 4, false);
	for (int32 ValueIndex = 0; ValueIndex < Values.Num(); ValueIndex++)
	{
		if (Values[ValueIndex].IsEmpty())
		{
			continue;
		}

		if (Values[ValueIndex] == TEXT("ConfigName"))
		{
			ConfigName = StripQuotes(Values[ValueIndex + 1]);
		}
		else if (Values[ValueIndex] == TEXT("ClientId"))
		{
			ClientId = StripQuotes(Values[ValueIndex + 1]);
		}
		else if (Values[ValueIndex] == TEXT("ClientSecret"))
		{
			ClientSecret = StripQuotes(Values[ValueIndex + 1]);
		}
		else if (Values[ValueIndex] == TEXT("ProductId"))
		{
			ProductId = StripQuotes(Values[ValueIndex + 1]);
		}
		else if (Values[ValueIndex] == TEXT("SandboxId"))
		{
			SandboxId = StripQuotes(Values[ValueIndex + 1]);
		}
		else if (Values[ValueIndex] == TEXT("DeploymentId"))
		{
			DeploymentId = StripQuotes(Values[ValueIndex + 1]);
		}
		else if (Values[ValueIndex] == TEXT("EncryptionKey"))
		{
			EncryptionKey = StripQuotes(Values[ValueIndex + 1]);
		}
		ValueIndex++;
	}
}
