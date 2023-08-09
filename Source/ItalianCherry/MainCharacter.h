// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Item.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EPlayerMovementStatus: uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Crouching UMETA(DisplayName = "Crouching"),

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EPlayerMovementDirection : uint8
{
	EMS_Forward UMETA(DisplayName = "Forward"),
	EMS_Left UMETA(DisplayName = "Left"),
	EMS_Right UMETA(DisplayName = "Right"),
	EMS_Backward UMETA(DisplayName = "Backward"),

	EMS_MAX UMETA(DisplayName = "idle")
};

UCLASS()
class ITALIANCHERRY_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow Camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	FVector CameraInitialPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EPlayerMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EPlayerMovementDirection MovementDirection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float WalkingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float CrouchingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meshes")
	TArray<USkeletalMesh*> AvaliableMeshes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meshes")
	TArray<UAnimBlueprint*>  AnimationsClasesOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Meshes")
	int IndexCurrentCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Ui")
	bool UserIventoryUIStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Ui")
	bool ItemTouch;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void StopMovement();
	void ChangetoRun();
	void ChangeToNormal();
	void ChangeToCrouch();
	void SetPlayerMovementStatus(EPlayerMovementStatus Status);
	void SetPlayerMovementDirection(EPlayerMovementDirection Direction);
	void ChangeCharacter();
	void ManageInventory();

};
