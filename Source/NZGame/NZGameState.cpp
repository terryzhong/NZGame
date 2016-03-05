// Fill out your copyright notice in the Description page of Project Settings.

#include "NZGame.h"
#include "NZGameState.h"
#include "NZWeapon.h"
#include "NZTeamInterface.h"


void ANZGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
}

bool ANZGameState::OnSameTeam(const AActor* Actor1, const AActor* Actor2)
{
	const INZTeamInterface* TeamInterface1 = Cast<INZTeamInterface>(Actor1);
	const INZTeamInterface* TeamInterface2 = Cast<INZTeamInterface>(Actor2);
	if (TeamInterface1 == NULL || TeamInterface2 == NULL)
	{
		return false;
	}
	else if (TeamInterface1->IsFriendlyToAll() || TeamInterface2->IsFriendlyToAll())
	{
		return true;
	}
	else
	{
		uint8 TeamNum1 = TeamInterface1->GetTeamNum();
		uint8 TeamNum2 = TeamInterface2->GetTeamNum();

		if (TeamNum1 == 255 || TeamNum2 == 255)
		{
			return false;
		}
		else
		{
			return TeamNum1 == TeamNum2;
		}
	}
}

/** Returns true if P1 should be sorted before P2 */
bool ANZGameState::InOrder(class ANZPlayerState* P1, class ANZPlayerState* P2)
{
	// Spectators are sorted last
	if (P1->bOnlySpectator)
	{
		return P2->bOnlySpectator;
	}
	else if (P2->bOnlySpectator)
	{
		return true;
	}

	if (P1->Score < P2->Score)
	{
		return false;
	}

	if (P1->Score == P2->Score)
	{
		// If score tied, use deaths to sort
		if (P1->Deaths > P2->Deaths)
		{
			return false;
		}

		// Keep local player highest on list
		if ((P1->Deaths == P2->Deaths) && (Cast<APlayerController>(P2->GetOwner()) != NULL))
		{
			ULocalPlayer* LP2 = Cast<ULocalPlayer>(Cast<APlayerController>(P2->GetOwner())->Player);
			if (LP2 != NULL)
			{
				// Make sure ordering is consistent for splitscreen players
				ULocalPlayer* LP1 = Cast<ULocalPlayer>(Cast<APlayerController>(P1->GetOwner())->Player);
				return (LP1 != NULL);
			}
		}
	}

	return true;
}

void ANZGameState::SortPSArray()
{
	for (int32 i = 0; i < PlayerArray.Num() - 1; i++)
	{
		ANZPlayerState* P1 = Cast<ANZPlayerState>(PlayerArray[i]);
		for (int32 j = i + 1; j < PlayerArray.Num(); j++)
		{
			ANZPlayerState* P2 = Cast<ANZPlayerState>(PlayerArray[j]);
			if (!InOrder(P1, P2))
			{
				PlayerArray[i] = P2;
				PlayerArray[j] = P1;
				P1 = P2;
			}
		}
	}
}

bool ANZGameState::HasMatchStarted() const
{
	return Super::HasMatchStarted() && GetMatchState() != MatchState::CountdownToBegin && GetMatchState() != MatchState::PlayerIntro;
}

bool ANZGameState::IsMatchInProgress() const
{
	FName MatchState = GetMatchState();
	return (MatchState == MatchState::InProgress || MatchState == MatchState::MatchIsInOvertime);
}

bool ANZGameState::IsMatchInOvertime() const
{
	FName MatchState = GetMatchState();
	return (MatchState == MatchState::MatchEnteringOvertime || MatchState == MatchState::MatchIsInOvertime);
}

bool ANZGameState::IsMatchInCountdown() const
{
	return GetMatchState() == MatchState::CountdownToBegin;
}

bool ANZGameState::IsMatchIntermission() const
{
	return GetMatchState() == MatchState::MatchIntermission;
}

int32 ANZGameState::GetMaxSpectatingId()
{
    int32 MaxSpectatingID = 0;
    for (int32 i = 0; i < PlayerArray.Num(); i++)
    {
        ANZPlayerState* PS = Cast<ANZPlayerState>(PlayerArray[i]);
        if (PS && (PS->SpectatingID > MaxSpectatingID))
        {
            MaxSpectatingID = PS->SpectatingID;
        }
    }
    return MaxSpectatingID;
}

int32 ANZGameState::GetMaxTeamSpectatingId(int32 TeamNum)
{
    int32 MaxSpectatingID = 0;
    for (int32 i = 0; i < PlayerArray.Num(); i++)
    {
        ANZPlayerState* PS = Cast<ANZPlayerState>(PlayerArray[i]);
        if (PS && (PS->GetTeamNum() == TeamNum) && (PS->SpectatingIDTeam > MaxSpectatingID))
        {
            MaxSpectatingID = PS->SpectatingIDTeam;
        }
    }
    return MaxSpectatingID;
}

FOverlayEffect ANZGameState::GetFirstOverlay(uint16 Flags, bool bFirstPerson)
{
    if (Flags == 0)
    {
        return FOverlayEffect();
    }
    else
    {
        for (int32 i = 0; i < ARRAY_COUNT(OverlayEffects); i++)
        {
            if (Flags & (1 << i))
            {
                return (bFirstPerson && OverlayEffects1P[i].IsValid()) ? OverlayEffects1P[i] : OverlayEffects[i];
            }
        }
        return FOverlayEffect();
    }
}

FName ANZGameState::OverrideCameraStyle(APlayerController* PCOwner, FName CurrentCameraStyle)
{
    if (HasMatchEnded())
    {
        return FName(TEXT("FreeCam"));
    }
    // todo:
    //// FIXME: Shouldn't this come from the Pawn?
    //else if (Cast<ANZRemoteRedeemer>(PCOwner->GetPawn()) != NULL)
    //{
    //    return FName(TEXT("FirstPerson"));
    //}
    else
    {
        return CurrentCameraStyle;
    }
}

void ANZGameState::OnRep_OverlayEffects()
{
    
}


