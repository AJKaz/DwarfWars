// Fill out your copyright notice in the Description page of Project Settings.


#include "DwarfGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Engine/World.h"
#include <Online/OnlineSessionNames.h>
#include "Kismet/GameplayStatics.h"

UDwarfGameInstance::UDwarfGameInstance() {

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

void UDwarfGameInstance::OnCreateSessionComplete(FName ServerName, bool Succeeded) {
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete | Succeeded: %d"), Succeeded);
	if (Succeeded) {
		GetWorld()->ServerTravel("/Game/Maps/TestLevel?listen");
	}
}

void UDwarfGameInstance::OnFindSessionComplete(bool Succeeded) {
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete | Succeeded: %d"), Succeeded);
	if (Succeeded) {
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		
		for (FOnlineSessionSearchResult Result : SearchResults) {
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
			
			ServerListDelegate.Broadcast(Info);
		}

		UE_LOG(LogTemp, Warning, TEXT("Search Results, Server Count: %d"), SearchResults.Num());
		
		if (SearchResults.Num()) {
			/*UE_LOG(LogTemp, Warning, TEXT("Joining Server"));
			SessionInterface->JoinSession(0, "My Session", SearchResults[0]);*/
		}
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

void UDwarfGameInstance::CreateServer(FString ServerName, FString HostName) {
	if (SessionInterface.IsValid()) {
		UE_LOG(LogTemp, Warning, TEXT("CreateServer"));
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bAllowJoinInProgress = true;
		SessionSettings.bAllowJoinViaPresence = true;
		SessionSettings.bUseLobbiesIfAvailable = true;
		SessionSettings.bIsDedicated = false;
		SessionSettings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.NumPublicConnections = 10;

		SessionSettings.Set(FName("SERVER_NAME_KEY"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionSettings.Set(FName("SERVER_HOSTNAME_KEY"), HostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		SessionInterface->CreateSession(0, FName("My Session"), SessionSettings);
	}
}

void UDwarfGameInstance::FindServers() {
	UE_LOG(LogTemp, Warning, TEXT("FindServers"));
	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	SessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (LocalPlayer) SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}


