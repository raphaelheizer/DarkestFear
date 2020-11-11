// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/SceneCaptureComponent2D.h"
#include "DarkestFear/Item.h"
#include "Phone.generated.h"

UCLASS()
class DARKESTFEAR_API APhone : public AItem
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    APhone();

    // The phone must have a render target camera so we can film what happens
    UPROPERTY(VisibleAnywhere, Instanced, BlueprintReadWrite, Category="General")
    class USceneCaptureComponent2D* RealTimeCamera;

    // todo: must have a plane too so we can render the camera into this plane
    UPROPERTY(VisibleAnywhere, Instanced, BlueprintReadWrite, Category="General")
    class UStaticMeshComponent* PhoneScreen;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    virtual void Use(ADarkestFearCharacter* DarkestFearCharacter) override;
    virtual void AlternateUse(ADarkestFearCharacter* DarkestFearCharacter) override;
};
