// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "MovieSceneTimeHelpers.h"
#include "SAttributeComponent.h"
#include "SWorldUserWidget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Perception/PawnSensingComponent.h"

ASAICharacter::ASAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	CheckLowHealthInterval = 5;

	HitFlashParamName = "TimeToHit";
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
	SetTargetActor(Cast<AActor>(Pawn));
	
	DrawDebugString(GetWorld(), GetActorLocation(), "PLAYR SPOTTED", nullptr, FColor::White, 4.0f, true, 1);
}

void ASAICharacter::OnHealthValueChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if(Delta < 0.0f)
	{
		// Show Hit Flash Material
		GetMesh()->SetScalarParameterValueOnMaterials(HitFlashParamName, GetWorld()->GetTimeSeconds());		
	}

	// show health bar
	if(!UserWidgetUI)
	{
		UserWidgetUI = CreateWidget<USWorldUserWidget>(GetWorld(), UserWidgetClass);
		if(UserWidgetUI)
		{
			UserWidgetUI->AttachToActor = this;
			UserWidgetUI->AddToViewport();
		}
	}

	// avoid AI attack eachother ???
	if(Delta < 0.0f && InstigatorActor != this && InstigatorActor->GetClass() != ASAICharacter::GetClass())
	{
		SetTargetActor(InstigatorActor);
	}
	
	// AI Dead:
	if(NewHealth <= 0.0f)
	{

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

		AttributeComp->ApplyHealthChange(nullptr, AttributeComp->GetMaxHealth());
	}
}

void ASAICharacter::SetTargetActor(AActor* NewTargetActor)
{
	if(NewTargetActor == nullptr)
	{
		return;
	}
	
	AAIController* AIController = Cast<AAIController>(GetController());
	if(ensure(AIController))
	{
		UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
		if(BBComp)
		{
			BBComp->SetValueAsObject("TargetActor", NewTargetActor);
		}
	}
}
