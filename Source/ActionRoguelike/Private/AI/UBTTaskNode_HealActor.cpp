// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/UBTTaskNode_HealActor.h"

#include "AIController.h"
#include "AI/SAICharacter.h"

UUBTTaskNode_HealActor::UUBTTaskNode_HealActor()
{
	HealValue = 0.0f;
}

EBTNodeResult::Type UUBTTaskNode_HealActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if(ensure(AIController))
	{
		ASAICharacter* AICharacter = Cast<ASAICharacter>(AIController->GetPawn());
		if(ensure(AICharacter))
		{
			AICharacter->HealAISelf(HealValue);
		}
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
