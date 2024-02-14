// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackStartNotifyState.h"
#include "Engine.h"
#include "../Character/DwarfCharacter.h"

void UAttackStartNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) {
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, __FUNCTION__);
	if (MeshComp && MeshComp->GetOwner()) {
		if (ADwarfCharacter* Player = Cast<ADwarfCharacter>(MeshComp->GetOwner())) {
			Player->PunchAttackStart();
		}
	}
}

void UAttackStartNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) {
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, __FUNCTION__);
	if (MeshComp && MeshComp->GetOwner()) {
		if (ADwarfCharacter* Player = Cast<ADwarfCharacter>(MeshComp->GetOwner())) {
			Player->PunchAttackEnd();
		}
	}
}
