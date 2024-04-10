// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "MovieSceneTimeHelpers.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"

ASAICharacter::ASAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	CheckLowHealthInterval = 5;
}

void ASAICharacter::BeginPlay()
{
	Super::BeginPlay();

	// GetWorldTimerManager().SetTimer(TimerHandle_CheckLowHealth, this, &ASAICharacter::CheckLowHealth, CheckLowHealthInterval);
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnSeePawn);

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::ASAICharacter::OnHealthValueChanged);
	
}

void ASAICharacter::OnSeePawn(APawn* Pawn)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if(ensure(AIController))
	{
		UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
		if(BBComp)
		{
			BBComp->SetValueAsObject("TargetActor", Pawn);
		}

		DrawDebugString(GetWorld(), GetActorLocation(), "PLAYR SPOTTED", nullptr, FColor::White, 4.0f, true, 1);
	}
}

void ASAICharacter::OnHealthValueChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta, float MaxHealth, float DangerousHealth)
{
	if(Delta < 0.0f)
	{
		// Show Hit Flash Material
		//GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->GetTimeSeconds());		
	}

	if(NewHealth <= 0.0f)
	{
		// AI Dead:

		// Stop Run BehaviorTree
		AAIController* AIController = Cast<AAIController>(GetController());
		if(AIController)
		{
			// Reason : Just used to debug.
			AIController->GetBrainComponent()->StopLogic("Killed");
		}

		// Ragdoll
		GetMesh()->SetAllBodiesSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName("Ragdoll");

		// Set lifespan
		SetLifeSpan(10.0f);
	}
}

void ASAICharacter::CheckLowHealth()
{
	if(AttributeComp->IsInLowHealth())
	{
		// Minion is dangerous, find a safe location to hide. And cure self.

		AttributeComp->ApplyHealthChange(AttributeComp->GetMaxHealth());
	}
}
