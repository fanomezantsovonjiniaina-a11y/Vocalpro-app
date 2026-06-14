#include "MenaranaAI.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/SurvivalCharacter.h"

AMenaranaAI::AMenaranaAI()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;
}

void AMenaranaAI::BeginPlay()
{
    Super::BeginPlay();

    // Langaha sy Ithycyphus miafina aloha
    CurrentState = (SnakeType == EMenaranaType::Boa)
        ? EMenaranaState::Resting
        : EMenaranaState::Hidden;
}

void AMenaranaAI::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!HasAuthority()) return;
    StrikeCooldown = FMath::Max(0.f, StrikeCooldown - DeltaTime);
    UpdateAI(DeltaTime);
}

void AMenaranaAI::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMenaranaAI, CurrentState);
}

void AMenaranaAI::UpdateAI(float DeltaTime)
{
    AActor* NearPlayer = GetPlayerInRange();

    switch (CurrentState)
    {
    case EMenaranaState::Hidden:
    case EMenaranaState::Resting:
        // Mikapoka raha manakaiky loatra
        if (NearPlayer)
            CurrentState = EMenaranaState::Striking;
        break;

    case EMenaranaState::Striking:
        if (!NearPlayer) { CurrentState = EMenaranaState::Retreating; break; }
        StrikePlayer(NearPlayer);
        break;

    case EMenaranaState::Retreating:
        // Manalavitra — miverina miafina
        {
            FVector Away = -GetActorForwardVector();
            AddMovementInput(Away * 0.5f);
            // Afaka miafina rehefa lavitra
            float Dist = NearPlayer ? FVector::Dist(GetActorLocation(), NearPlayer->GetActorLocation()) : 9999.f;
            if (Dist > 500.f)
                CurrentState = EMenaranaState::Hidden;
        }
        break;

    default: break;
    }

    // Ithycyphus: mety hianiana avy amin'ny hazo (random)
    if (SnakeType == EMenaranaType::Ithycyphus && NearPlayer && CurrentState == EMenaranaState::Hidden)
    {
        if (FMath::RandRange(0, 500) == 0) // Rare event
        {
            SetActorLocation(NearPlayer->GetActorLocation() + FVector(0, 0, 50));
            CurrentState = EMenaranaState::Striking;
        }
    }
}

AActor* AMenaranaAI::GetPlayerInRange() const
{
    TArray<AActor*> Players;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASurvivalCharacter::StaticClass(), Players);
    for (AActor* P : Players)
        if (FVector::Dist(GetActorLocation(), P->GetActorLocation()) < DetectionRadius * 100.f)
            return P;
    return nullptr;
}

void AMenaranaAI::StrikePlayer(AActor* Player)
{
    if (StrikeCooldown > 0.f) return;

    ASurvivalCharacter* PlayerChar = Cast<ASurvivalCharacter>(Player);
    if (!PlayerChar) return;

    USurvivalComponent* Survival = PlayerChar->GetSurvivalComponent();

    switch (SnakeType)
    {
    case EMenaranaType::Boa:
        Survival->Server_ApplyDamage(30.f);
        break;
    case EMenaranaType::Langaha:
        Survival->Server_ApplyDamage(5.f);
        Survival->Server_ApplyStatusEffect(EStatusEffect::SnakePoison);
        break;
    case EMenaranaType::Ithycyphus:
        Survival->Server_ApplyDamage(3.f);
        Survival->Server_ApplyStatusEffect(EStatusEffect::SnakePoison);
        break;
    }

    StrikeCooldown = 4.f;
    CurrentState = EMenaranaState::Retreating;
}

EStatusEffect AMenaranaAI::GetPoisonType() const
{
    if (SnakeType == EMenaranaType::Boa) return EStatusEffect::None;
    return EStatusEffect::SnakePoison;
}

void AMenaranaAI::TakeDamage_Snake(float Amount)
{
    HP = FMath::Max(0.f, HP - Amount);
    if (HP <= 0.f) SetLifeSpan(1.f);
    else CurrentState = EMenaranaState::Retreating;
}
