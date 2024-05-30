// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SInteractionComponent.generated.h"

class USWorldUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USInteractionComponent();

protected:

	UPROPERTY()
	AActor* FocusedActor;

	UPROPERTY(EditDefaultsOnly, Category="Interact")
	float TranceRadius;
	
	UPROPERTY(EditDefaultsOnly, Category="Interact")
	float TranceDistance;
	
	UPROPERTY(EditDefaultsOnly, Category="Interact")
	TEnumAsByte<ECollisionChannel> QueryChannel;
	
	UPROPERTY(EditDefaultsOnly, Category="Interact")
	TSubclassOf<USWorldUserWidget> DefaultWidgetClass;

	UPROPERTY()
	USWorldUserWidget* DefaultWidgetInstance;
	
public:
	
	void PrimaryInteract();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	
	void FindBestInteractable();

	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* InFocus);
};
