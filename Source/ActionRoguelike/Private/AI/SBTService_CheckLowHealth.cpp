// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckLowHealth.h"

#include "AIController.h"
#include "SAttributeComponent.h"
#include "AI/SAICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckLowHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Check Is In Low Health State ?

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if(ensure(BlackboardComp))
	{
		AAIController* AIController = OwnerComp.GetAIOwner();
		if(ensure(AIController))
		{
			USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(AIController->GetPawn());
			if(AttributeComp)
			{
				BlackboardComp->SetValueAsBool(IsInLowHealth.SelectedKeyName, AttributeComp->IsInLowHealth());
			}
		}
	}	
}
