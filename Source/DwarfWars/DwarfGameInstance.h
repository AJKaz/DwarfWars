// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "DwarfGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FCreateServerInfo {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FString ServerName;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxPlayers;
};

USTRUCT(BlueprintType)
struct FServerInfo {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FString ServerName;

	UPROPERTY(BlueprintReadOnly)
	FString PlayerCountStr;
	
	UPROPERTY(BlueprintReadOnly)
	int32 ServerArrayIndex;

	UPROPERTY(BlueprintReadOnly)
	int32 Ping;

	int32 CurrentPlayers;
	int32 MaxPlayers;

	void SetPlayerCount() {
		PlayerCountStr = FString(FString::FromInt(CurrentPlayers) + "/" + FString::FromInt(MaxPlayers));
	};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerDelegate, FServerInfo, ServerListDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerSearchingDelegate, bool, SearchingForServer);

/**
 * 
 */
UCLASS()
class DWARFWARS_API UDwarfGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UDwarfGameInstance();

protected:
	virtual void Init() override;

	/* Delegates */
	virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	virtual void OnFindSessionComplete(bool Succeeded);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
	UFUNCTION(BlueprintCallable)
	void CreateServer(FCreateServerInfo ServerInfo);

	UFUNCTION(BlueprintCallable)
	void FindServers();

	UFUNCTION(BlueprintCallable)
	void JoinServer(int32 ArrayIndex);

	UFUNCTION(BlueprintCallable)
	FText ValidatePlayerCountInput(FText Input, FText CurrentValue);

protected:
	FName DefaultSessionName;

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	UPROPERTY(BlueprintAssignable)
	FServerDelegate ServerListDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FServerSearchingDelegate SearchingForServer;

private:
	const uint8 MAX_PLAYER_COUNT = 20;
	const uint8 MIN_PLAYER_COUNT = 1;
};
