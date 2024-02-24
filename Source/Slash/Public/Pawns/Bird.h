// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Bird.generated.h"

struct FInputActionValue;

UCLASS()
class SLASH_API ABird : public APawn
{
	GENERATED_BODY()

public:
	ABird();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void MoveForward(const FInputActionValue& Value);
	UFUNCTION()
	void Turn(const FInputActionValue& Value);
	UFUNCTION()
	void LookUp(const FInputActionValue& Value);

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCapsuleComponent> Capsule;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USkeletalMeshComponent> BirdMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCameraComponent> ViewCamera;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> IMC_Player;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputAction> IA_MoveForward;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputAction> IA_Turn;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputAction> IA_LookUp;
};
