// Fill out your copyright notice in the Description page of Project Settings.


#include "STargetDummy.h"

#include "SAttributeComponent.h"

// Sets default values
ASTargetDummy::ASTargetDummy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	RootComponent = StaticMesh;

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
}

void ASTargetDummy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASTargetDummy::OnHealthValueChange);
}

void ASTargetDummy::OnHealthValueChange(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta, float MaxHealth, float DangerousHealth)
{
	if(Delta < 0.0f)
	{
		StaticMesh->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->GetTimeSeconds());		
	}
}


