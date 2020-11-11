// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Components/SpotLightComponent.h"
#include "DarkestFear/Item.h"
#include "Flashlight.generated.h"

UCLASS()
class DARKESTFEAR_API AFlashlight : public AItem
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AFlashlight();

    UPROPERTY(VisibleAnywhere, Instanced, BlueprintReadWrite, Category="General")
    class USpotLightComponent* SpotLight;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Declaration of actor's functions
    virtual void Use(class ADarkestFearCharacter* DarkestFearCharacter) override;
    virtual void AlternateUse(ADarkestFearCharacter* DarkestFearCharacter) override;
};
