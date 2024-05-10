// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "DrawDebugHelpers.h"
#include "MovieSceneTimeHelpers.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SGameModeBase.h"
#include "SWorldUserWidget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"

ASAICharacter::ASAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	CheckLowHealthInterval = 5;

	HitFlashParamName = "TimeToHit";
	TargetActorKey = "TargetActor";
}

void ASAICharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnSeePawn);

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::ASAICharacter::OnHealthValueChanged);
	
}

void ASAICharacter::OnSeePawn(APawn* Pawn)
{
	if(GetTargetActor() == Pawn)
	{
		return;
	}
	
	SetTargetActor(Cast<AActor>(Pawn));
	
	if(PlayerSpottedWidgetClass)
	{
		USWorldUserWidget* Widget = CreateWidget<USWorldUserWidget>(GetWorld(), PlayerSpottedWidgetClass);
		
		if(Widget)
		{
			Widget->AttachToActor = this;
			Widget->AddToViewport(10);
		}
	}

	//DrawDebugString(GetWorld(), GetActorLocation(), "PLAYR SPOTTED", nullptr, FColor::White, 4.0f, true, 1);
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
	if(Delta < 0.0f && InstigatorActor && InstigatorActor != this && InstigatorActor->GetClass() != ASAICharacter::GetClass())
	{
		SetTargetActor(InstigatorActor);
	}
	
	// AI Dead:
	if(NewHealth <= 0.0f)
	{
		ActorDead(InstigatorActor);
	}
}

void ASAICharacter::HealAISelf(float HealValue /*= 0.0f*/)
{
	if(AttributeComp->IsInLowHealth())
	{
		if(HealValue == 0.0f)
		{
			HealValue = AttributeComp->GetMaxHealth();
		}
		AttributeComp->ApplyHealthChange(nullptr, HealValue);
	}
}

void ASAICharacter::SetTargetActor(AActor* NewTargetActor)
{
	if(GetTargetActor() == NewTargetActor)
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

AActor* ASAICharacter::GetTargetActor() const
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if(AIC)
	{
		return Cast<AActor>(AIC->GetBlackboardComponent()->GetValueAsObject(TargetActorKey));
	}
	return nullptr;
}

void ASAICharacter::ActorDead(AActor* Killer)
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

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();

	// Set lifespan
	SetLifeSpan(10.0f);

	ASGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASGameModeBase>();
	if(GameMode)
	{
		GameMode->OnAIActorKilled(Killer);
	}
}
