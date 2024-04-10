// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	if(ensureMsgf(BeHaviorTree, TEXT("BeHaviorTree is nullptr! Please Assignment BeHavior Asest!")))
	{
		RunBehaviorTree(BeHaviorTree);
	}
}

