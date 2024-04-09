// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RangedAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/ControlChannel.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type USBTTask_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if(ensure(AIController))
	{
		ACharacter* AICharacter = Cast<ACharacter>(AIController->GetPawn());
		if(AICharacter == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		AActor* TargetActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetActorKey.SelectedKeyName));
		if(TargetActor == nullptr)
		{
			return  EBTNodeResult::Failed;
		}

		FVector HandLocation = AICharacter->GetMesh()->GetSocketLocation("Muzzle_01");
		FRotator HandRotator = (TargetActor->GetActorLocation() - HandLocation).Rotation();

		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, HandLocation, HandRotator, Params);

		return Projectile ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::Failed;
}
