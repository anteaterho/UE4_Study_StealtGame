// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSLunchPad.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UArrowComponent;

UCLASS()
class FPSGAME_API AFPSLunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSLunchPad();

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* OverlapComp;

	UFUNCTION()
	void OverlapLaunchPad(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditInstanceOnly, Category = "LunchPad")
	float LaunchStrength;

	UPROPERTY(EditInstanceOnly, Category = "LunchPad")
	float LaunchPitchAngle;
	
	UPROPERTY(EditDefaultsOnly, Category = "LunchPad")
	UParticleSystem* ActiveLaunchPadEffect;


public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
