// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

USBTService_CheckAttackRange::USBTService_CheckAttackRange()
{
	InnerRadius = 100.0f; 

	OuterRadius = 2000.0f;
}

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Check Target Actor is In Attack Range ?

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if(ensure(BlackboardComp))
	{
		AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("MoveToActor"));
		if(TargetActor)
		{
			AAIController* AIController = OwnerComp.GetAIOwner();
			if(ensure(AIController))
			{
				APawn* AIPawn = AIController->GetPawn();
				if(ensure(AIPawn))
				{
					float DistanceToTarget = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());

					// check if within attack range.
					bool bWithinRange = (DistanceToTarget <= OuterRadius && DistanceToTarget >= InnerRadius);

					bool bHasLOS = false;
					if(bWithinRange)
					{
						// check if AIController's pawn can see TargetActor. true means can see.
						bHasLOS = AIController->LineOfSightTo(TargetActor);
					}
					
					BlackboardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, bWithinRange && bHasLOS);
				}
			}
		}
	}
}
