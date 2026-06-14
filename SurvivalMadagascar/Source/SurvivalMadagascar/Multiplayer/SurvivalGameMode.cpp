#include "SurvivalGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "../Animals/FosaAI.h"
#include "../Characters/SurvivalCharacter.h"

ASurvivalGameMode::ASurvivalGameMode()
{
    // Set default classes in BP subclass
}

void ASurvivalGameMode::BeginPlay()
{
    Super::BeginPlay();
    PlayersAlive = 0;
}

void ASurvivalGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    ConnectedPlayers.Add(NewPlayer);
    PlayersAlive++;
}

void ASurvivalGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);
    if (APlayerController* PC = Cast<APlayerController>(Exiting))
        ConnectedPlayers.Remove(PC);
}

void ASurvivalGameMode::OnNightBegins()
{
    CurrentNight++;

    // Spawn Fosa(s) — more as nights progress
    int32 FosaCount = FMath::Min(1 + CurrentNight / 5, 3);
    for (int i = 0; i < FosaCount; i++)
        SpawnFosa();
}

void ASurvivalGameMode::OnDayBegins()
{
    // Despawn all Fosas
    TArray<AActor*> Fosas;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AFosaAI::StaticClass(), Fosas);
    for (AActor* Fosa : Fosas)
        Fosa->Destroy();

    CheckVictoryCondition();
}

void ASurvivalGameMode::OnPlayerDied(APlayerController* PC)
{
    PlayersAlive--;

    if (PlayersAlive <= 0)
    {
        // Game Over — all players dead
        // Trigger GameOver UI via GameState
    }
}

void ASurvivalGameMode::OnPlayerRevived(APlayerController* PC)
{
    PlayersAlive++;
}

void ASurvivalGameMode::CheckVictoryCondition()
{
    if (CurrentNight >= NightsToWin)
    {
        // Victory! Trigger ending sequence
        UGameplayStatics::OpenLevel(GetWorld(), FName("VictoryLevel"));
    }
}

void ASurvivalGameMode::SpawnFosa()
{
    // Spawn outside player view, random location
    TArray<AActor*> Players;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASurvivalCharacter::StaticClass(), Players);

    if (Players.IsEmpty()) return;

    AActor* RandomPlayer = Players[FMath::RandRange(0, Players.Num() - 1)];
    FVector SpawnOffset = FMath::VRand() * 8000.f; // 80m away
    SpawnOffset.Z = 0.f;
    FVector SpawnLocation = RandomPlayer->GetActorLocation() + SpawnOffset;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AFosaAI* Fosa = GetWorld()->SpawnActor<AFosaAI>(AFosaAI::StaticClass(), SpawnLocation, FRotator::ZeroRotator, Params);
    if (Fosa)
        Fosa->OnNightBegins();
}

void ASurvivalGameMode::RespawnPlayer(APlayerController* PC)
{
    // Respawn at camp with reduced HP
    if (APawn* OldPawn = PC->GetPawn())
        OldPawn->Destroy();

    AActor* Camp = UGameplayStatics::GetActorOfClass(GetWorld(), AActor::StaticClass()); // Find camp actor
    FVector SpawnLocation = Camp ? Camp->GetActorLocation() : FVector::ZeroVector;

    RestartPlayerAtTransform(PC, FTransform(SpawnLocation));
}
