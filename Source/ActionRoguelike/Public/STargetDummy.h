// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STargetDummy.generated.h"

class USAttributeComponent;

UCLASS()
class ACTIONROGUELIKE_API ASTargetDummy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTargetDummy();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USAttributeComponent* AttributeComp;
	
public:
	
	virtual void PostInitializeComponents() override;
	
	UFUNCTION()
	void OnHealthValueChange(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta, float MaxHealth, float DangerousHealth);
};
