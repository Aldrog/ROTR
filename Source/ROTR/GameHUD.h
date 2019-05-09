// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

/**
 * 
 */
UCLASS()
class ROTR_API AGameHUD : public AHUD
{
    GENERATED_BODY()

public:
    AGameHUD();

    virtual void DrawHUD() override;
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> HUDWidgetClass;

    UPROPERTY(EditAnywhere)
    UUserWidget *CurrentWidget;
};
