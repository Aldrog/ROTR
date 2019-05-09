// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ROTRCharacter.generated.h"

UCLASS(config=Game)
class AROTRCharacter : public ACharacter
{
    GENERATED_BODY()

    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    /** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;
public:
    AROTRCharacter();

    /** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
    float BaseTurnRate;

    /** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
    float BaseLookUpRate;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Stats)
    float Health = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Display)
    float HealthPercentage = 1.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Stats)
    float Stamina = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
    float MaxStamina = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Display)
    float StaminaPercentage = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
    float StaminaRegen = 2.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement)
    bool IsSprinting = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement)
    float SprintSpeed = 600.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement)
    float RunSpeed = 400.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement)
    float WalkSpeed = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement)
    float RunCost = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Movement)
    float SprintCost = 10.f;

    bool UpdateHealth(float Delta);
    bool UpdateStamina(float Delta);

protected:

    /** Resets HMD orientation in VR. */
    void OnResetVR();

    /** Called for forwards/backward input */
    void MoveForward(float Value);

    /** Called for side to side input */
    void MoveRight(float Value);

    /**
     * Called via input to turn at a given rate.
     * @param Rate    This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
     */
    void TurnAtRate(float Rate);

    /**
     * Called via input to turn look up/down at a given rate.
     * @param Rate    This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
     */
    void LookUpAtRate(float Rate);

    /** Handler for when a touch input begins. */
    void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

    /** Handler for when a touch input stops. */
    void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

    void Tick(float DeltaSeconds) override;

protected:
    // APawn interface
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    // End of APawn interface

public:
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
    void Sprint();
    void StopSprinting();
    void ToggleCrawling();
};

