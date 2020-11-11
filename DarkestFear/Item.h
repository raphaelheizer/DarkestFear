// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ArrowComponent.h"
#include "DarkestFearCharacter.h"
#include "GameFramework/Actor.h"

#include "Item.generated.h"

UCLASS()
class DARKESTFEAR_API AItem : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AItem();

    UPROPERTY(VisibleAnywhere, Instanced, BlueprintReadWrite, Category="General")
    class UStaticMeshComponent* MeshComponent;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="State")
    bool bCanPickup;

    virtual void Use(class ADarkestFearCharacter* DarkestFearCharacter);
    virtual void AlternateUse(class ADarkestFearCharacter* DarkestFearCharacter);
    /*
     * todo 11/09/2020 change the below line AttachmentName to value taken from the player character
     * it absolutely makes no sense to define a default player value in the item class. 
     */
    virtual AItem* Pickup(class ADarkestFearCharacter* DarkestFearCharacter, FName AttachmentName = "hand_l_socket");

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

private:
    // Sets default properties hidden from everybody's eyes

    /*
     * We give the item an arrow so we know where the item forwards must be facing
     * Do remember: socket attaching follows this rule! Remember to align both parent and child's forwards!
     */
    UPROPERTY()
    class UArrowComponent* ArrowComponent;
};
