// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


#include "GameFramework/Character.h"
#include "DarkestFearCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class ADarkestFearCharacter : public ACharacter
{
    GENERATED_BODY()

    /** Pawn mesh: 1st person view (arms; seen only by self) */
    UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
    class USkeletalMeshComponent* Mesh1P;

    /** First person camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FirstPersonCameraComponent;

    /** Motion controller (right hand) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UMotionControllerComponent* R_MotionController;

    /** Motion controller (left hand) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    class UMotionControllerComponent* L_MotionController;

public:
    ADarkestFearCharacter();

protected:
    virtual void BeginPlay() override;

public:
    /** Item Use line trace distance */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = General)
    float UseLineDistance;

    // Movement modifiers
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
    float MovementFactor;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
    float StrafeMovementFactor;

    /** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
    float BaseTurnRate;

    /** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
    float BaseLookUpRate;

    /** Gun muzzle's offset from the characters location */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
    FVector GunOffset;

    /** Projectile class to spawn */
    UPROPERTY(EditDefaultsOnly, Category=Projectile)
    TSubclassOf<class ADarkestFearProjectile> ProjectileClass;

    /** Sound to play each time we fire */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
    class USoundBase* FireSound;

    /** AnimMontage to play each time we fire */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    class UAnimMontage* FireAnimation;

    /** Whether to use motion controller location for aiming. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
    uint32 bUsingMotionControllers : 1;

    /** Player inventory */
    UPROPERTY(EditInstanceOnly, Category="General")
    TArray<class AItem*> Inventory;

    /** Player has a ghost mesh used as a pivot for item placement */
    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* ActiveItemGhost;

    // DEBUG TESTING
    void OnUseActiveItem();

    // todo 11/09/2020: Now we can find items in inventory array, is this still needed??
    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    class AItem* ActiveItem;

protected:

    /** Resets HMD orientation and position in VR. */
    void OnResetVR();

    /** Handles moving forward/backward */
    void MoveForward(float Val);

    /** Handles strafing movement, left and right */
    void MoveRight(float Val);

    /**
     * Called via input to turn at a given rate.
     * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
     */
    void TurnAtRate(float Rate);

    /**
     * Called via input to turn look up/down at a given rate.
     * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
     */
    void LookUpAtRate(float Rate);

    struct TouchData
    {
        TouchData()
        {
            bIsPressed = false;
            Location = FVector::ZeroVector;
        }

        bool bIsPressed;
        ETouchIndex::Type FingerIndex;
        FVector Location;
        bool bMoved;
    };

    void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
    void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
    void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
    TouchData TouchItem;

protected:
    // APawn interface
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

    // Primary Use Action
    void OnPrimaryUse();

    // Secondary Use Action
    void OnSecondaryUse();
    // End of APawn interface

    /* 
     * Configures input for touchscreen devices if there is a valid touch interface for doing so 
     *
     * @param	InputComponent	The input component pointer to bind controls to
     * @returns true if touch controls were enabled.
     */
    bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
    /** Returns Mesh1P subobject **/
    FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
    /** Returns FirstPersonCameraComponent subobject **/
    FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

private:
    // Tells us if the player is performing a place item action
    bool bIsPlacing;

    // Drop Item Pivot Rotation
    FRotator GhostMeshPivotRotation;

    // Item Cursor
    int8 InvCursor;

    virtual void Tick(float DeltaTime) override;

    FHitResult TraceLine();

    // Sets the currently equipped/active item based on a cursor/selector integer
    void SetActiveItem(int8 Slot);

    // Reserved action for picking up a pickupable item
    void PickUpItem();

    // Slots we can set and activate when selecting ActiveItem
    void OnUseSlot0();
    void OnUseSlot1();
    void OnUseSlot2();

    // Place item action
    void OnBeginPlace();
    void DisplayPlacementPivot();
    void OnFinishPlace();

    // Mouse Wheel Action
    void OnMouseWheelUp();
    void OnMouseWheelDown();
};
