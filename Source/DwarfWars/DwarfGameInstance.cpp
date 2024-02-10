// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/World.h"
#include <Online/OnlineSessionNames.h>
#include "Kismet/GameplayStatics.h"

UDwarfGameInstance::UDwarfGameInstance() {
	DefaultSessionName = FName("My Session");
}

void UDwarfGameInstance::Init() {
	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get()) {
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid()) {
			// Bind Delegates:
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UDwarfGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UDwarfGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UDwarfGameInstance::OnJoinSessionComplete);
		}
	}
}

void UDwarfGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded) {
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete | Succeeded: %d"), Succeeded);
	if (Succeeded) {
		GetWorld()->ServerTravel("/Game/Maps/LobbyLevel?listen");
	}
}

void UDwarfGameInstance::OnFindSessionComplete(bool Succeeded) {
	SearchingForServer.Broadcast(false);

	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete | Succeeded: %d"), Succeeded);
	if (Succeeded) {
		int32 ArrayIndex = -1;
		for (FOnlineSessionSearchResult Result : SessionSearch->SearchResults) {
			++ArrayIndex;

			if (!Result.IsValid()) continue;

			FServerInfo Info;
			FString ServerName = "";
			FString HostName = "";

			Result.Session.SessionSettings.Get(FName("SERVER_NAME_KEY"), ServerName);
			Result.Session.SessionSettings.Get(FName("SERVER_HOSTNAME_KEY"), HostName);
			if (ServerName == "") ServerName = "Empty Server Name";

			Info.ServerName = ServerName;
			Info.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
			Info.CurrentPlayers = Info.MaxPlayers - Result.Session.NumOpenPublicConnections;
			Info.SetPlayerCount();
			Info.ServerArrayIndex = ArrayIndex;
			Info.Ping = Result.PingInMs;
			
			ServerListDelegate.Broadcast(Info);
		}

		UE_LOG(LogTemp, Warning, TEXT("Search Results, Server Count: %d"), SessionSearch->SearchResults.Num());
	}
}

void UDwarfGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result) {
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete"));
	if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0)) {
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
		if (JoinAddress != "") PController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
	}
}

void UDwarfGameInstance::CreateServer(FCreateServerInfo ServerInfo) {
	if (SessionInterface.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("CreateServer | MaxPlayers: %d"), ServerInfo.MaxPlayers);
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bAllowJoinViaPresence = true;
		SessionSettings.bUseLobbiesIfAvailable = true;
		SessionSettings.bIsDedicated = false;
		SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;


		SessionSettings.NumPublicConnections = ServerInfo.MaxPlayers;

		SessionSettings.Set(FName("SERVER_NAME_KEY"), ServerInfo.ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, DefaultSessionName, SessionSettings);
	}
}

void UDwarfGameInstance::FindServers() {
	SearchingForServer.Broadcast(true);

	UE_LOG(LogTemp, Warning, TEXT("FindServers"));
	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// Can prob use line below for packaged proj, but not for testing locally, need 2 following lines
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	/*const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (LocalPlayer) SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());*/

	
}

void UDwarfGameInstance::JoinServer(int32 ArrayIndex) {
	FOnlineSessionSearchResult Result = SessionSearch->SearchResults[ArrayIndex];
	if (Result.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("JOINING SERVER AT INDEX: %d"), ArrayIndex);
		SessionInterface->JoinSession(0, DefaultSessionName, Result);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("FAILED TO JOIN SERVER AT INDEX: %d"), ArrayIndex);
	}
}

FText UDwarfGameInstance::ValidatePlayerCountInput(FText Input, FText CurrentValue) {
	FString InputString = Input.ToString();
	if (InputString == "") {
		return FText::FromString("");
	}

	FRegexPattern Pattern(TEXT("-?\\d+$"));

	FRegexMatcher Matcher(Pattern, InputString);

	if (!Matcher.FindNext() || Input.IsEmpty()) {
		return CurrentValue;
	}

	int32 InputInt = FCString::Atoi(*InputString);
	InputInt = FMath::Clamp(InputInt, MIN_PLAYER_COUNT, MAX_PLAYER_COUNT);

	return FText::FromString(FString::FromInt(InputInt));
}


