// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS_AbilitySystemComponent.h"

UGAS_AbilitySystemComponent::UGAS_AbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
}

void UGAS_AbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();
}
