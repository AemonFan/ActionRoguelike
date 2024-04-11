// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTask_RangedAttack.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USBTTask_RangedAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:

	USBTTask_RangedAttack();

	UPROPERTY(EditAnywhere, Category="Attack")
	float BulletRangePitch;

	UPROPERTY(EditAnywhere, Category="Attack")
	float BulletRangeYaw;
	
	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category="AI")
	FBlackboardKeySelector TargetActorKey;
};
