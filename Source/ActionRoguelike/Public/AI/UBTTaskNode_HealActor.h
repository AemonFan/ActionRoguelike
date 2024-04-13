// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UBTTaskNode_HealActor.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API UUBTTaskNode_HealActor : public UBTTaskNode
{
	GENERATED_BODY()

	UUBTTaskNode_HealActor();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:

	UPROPERTY(EditAnywhere, Category="AI")
	float HealValue;

};
