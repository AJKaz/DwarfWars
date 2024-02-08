// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "DwarfGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FServerInfo {
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FString ServerName;
	UPROPERTY(BlueprintReadOnly)
	FString PlayerCountStr;
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentPlayers;
	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers;
	

	void SetPlayerCount() {
		PlayerCountStr = FString(FString::FromInt(CurrentPlayers) + "/" + FString::FromInt(MaxPlayers));
	};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerDelegate, FServerInfo, ServerListDelegate);

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
	virtual void OnCreateSessionComplete(FName ServerName, bool Succeeded);
	virtual void OnFindSessionComplete(bool Succeeded);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	
	UFUNCTION(BlueprintCallable)
	void CreateServer(FString ServerName, FString HostName);

	UFUNCTION(BlueprintCallable)
	void FindServers();


protected:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	UPROPERTY(BlueprintAssignable)
	FServerDelegate ServerListDelegate;
};
