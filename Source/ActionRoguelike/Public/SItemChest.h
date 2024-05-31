#pragma once

#include "CoreMinimal.h"
#include "SGamePlayInterface.h"
#include "GameFramework/Actor.h"
#include "SItemChest.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASItemChest : public AActor, public ISGamePlayInterface
{
	GENERATED_BODY()

public:
	
	ASItemChest();

protected:

	UPROPERTY(visibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LidMesh;
	
	UPROPERTY(EditAnywhere)
	float TargetPitchValue;

	UPROPERTY(ReplicatedUsing="OnRep_LidOpen", SaveGame, BlueprintReadOnly)
	bool bLidOpen;

public:

	bool GetLidOpenState() const
	{
		return bLidOpen;
	}
	
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	
	virtual void OnActorLoaded_Implementation() override;

protected:

	UFUNCTION()
	void OnRep_LidOpen(); // RepNotify
	
	virtual void BeginPlay() override;
};
