// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	if(ensure(BeHaviorTree))
	{
		RunBehaviorTree(BeHaviorTree);
	}
	
	APawn* MyPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if(MyPawn)
	{
		if(GetBlackboardComponent())
		{
			GetBlackboardComponent()->SetValueAsObject("TargetActor", MyPawn);
		}
	}

}

