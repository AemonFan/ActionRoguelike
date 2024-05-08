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

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
	
protected:

	UPROPERTY(visibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LidMesh;
	
	UPROPERTY(EditAnywhere)
	float TargetPitchValue;

	UPROPERTY(ReplicatedUsing="OnRep_LidOpen", BlueprintReadOnly)
	bool bLidOpen;
	
protected:

	UFUNCTION()
	void OnRep_LidOpen(); // RepNotify
};
