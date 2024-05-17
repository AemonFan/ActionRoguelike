// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

/**
 * 
 */

class ASPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnChanged, APawn*, NewPawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, APlayerState*, PlayerState);

UCLASS()
class ACTIONROGUELIKE_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	// Member variable declaration: 'PlayerState' cannot be defined in 'SPlayerController' as it is already defined in scope 'Class /Script/Engine.Controller' (shadowing is not allowed)
	// UPROPERTY(ReplicatedUsing="OnRep_PlayerState")
	// ASPlayerState* PlayerState;

	UPROPERTY(BlueprintAssignable)
	FOnPawnChanged OnPawnChanged;

	virtual void SetPawn(APawn* InPawn) override;
	
	// 监听到来的PlayerState (对于客户端来说，在刚加入游戏的时候PlayerState可能是空指针, 
	// 获得之后PlayerState不会再变化因为PlayerController在同一个关卡中保持PlayerState的状态)
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChanged OnPlayerStateReceived;
	
	/* 当PlayerController准备好开始游戏的时候调用, 初始化UI之类与游戏数据有关的对象的好时机
			(特别是在多人游戏中客户端可能没有足够多的信息，因为PlayerState还没有收到) */
	virtual void BeginPlayingState() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintBeginPlayingState();

	// defined in parent class : AController
	virtual void OnRep_PlayerState() override;
};
