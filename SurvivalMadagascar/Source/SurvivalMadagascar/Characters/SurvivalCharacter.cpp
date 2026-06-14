#include "SurvivalCharacter.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

ASurvivalCharacter::ASurvivalCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    SurvivalComponent = CreateDefaultSubobject<USurvivalComponent>(TEXT("SurvivalComponent"));
}

void ASurvivalCharacter::BeginPlay()
{
    Super::BeginPlay();
    SurvivalComponent->OnDeath.AddDynamic(this, &ASurvivalCharacter::OnPlayerDeath);
}

void ASurvivalCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!HasAuthority()) return;

    // Apply exhaustion movement penalty
    GetCharacterMovement()->MaxWalkSpeed = 600.f * GetMoveSpeedModifier();

    if (bIsDown)
    {
        DownedTimer += DeltaTime;
        if (DownedTimer >= DownedTimeout)
        {
            // Respawn at camp after timeout
            bIsDown = false;
            SurvivalComponent->Stats.HP = 30.f;
            // Respawn logic handled by GameMode
        }
    }

    UpdateRevive(DeltaTime);
}

void ASurvivalCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ASurvivalCharacter, Role);
    DOREPLIFETIME(ASurvivalCharacter, bIsDown);
    DOREPLIFETIME(ASurvivalCharacter, ReviveProgress);
}

void ASurvivalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    // Input bindings — set up in Blueprint or Enhanced Input
}

void ASurvivalCharacter::OnPlayerDeath()
{
    if (bIsDown) return;

    bIsDown = true;
    DownedTimer = 0.f;
    SurvivalComponent->Stats.HP = 1.f; // Keep at 1 while downed
    GetCharacterMovement()->MaxWalkSpeed = 0.f;
}

void ASurvivalCharacter::UpdateRevive(float DeltaTime)
{
    if (!ReviveTarget) return;

    float Distance = FVector::Dist(GetActorLocation(), ReviveTarget->GetActorLocation());
    if (Distance > 200.f)
    {
        // Player moved away — cancel revive
        ReviveTimer = 0.f;
        ReviveTarget->ReviveProgress = 0.f;
        ReviveTarget = nullptr;
        return;
    }

    ReviveTimer += DeltaTime;
    ReviveTarget->ReviveProgress = ReviveTimer / ReviveDuration;

    if (ReviveTimer >= ReviveDuration)
    {
        // Revive complete
        ReviveTarget->bIsDown = false;
        ReviveTarget->DownedTimer = 0.f;
        ReviveTarget->SurvivalComponent->Stats.HP = 30.f;
        ReviveTarget->ReviveProgress = 0.f;
        ReviveTarget->GetCharacterMovement()->MaxWalkSpeed = 600.f;
        ReviveTarget = nullptr;
        ReviveTimer = 0.f;
    }
}

void ASurvivalCharacter::Server_StartRevive_Implementation(ASurvivalCharacter* Target)
{
    if (!Target || !Target->bIsDown) return;
    ReviveTarget = Target;
    ReviveTimer = 0.f;
}

void ASurvivalCharacter::Server_StopRevive_Implementation()
{
    if (ReviveTarget)
    {
        ReviveTarget->ReviveProgress = 0.f;
        ReviveTarget = nullptr;
    }
    ReviveTimer = 0.f;
}

void ASurvivalCharacter::Server_SetRole_Implementation(EPlayerRole NewRole)
{
    Role = NewRole;
}

void ASurvivalCharacter::Server_Interact_Implementation(AActor* Target)
{
    if (!Target) return;
    // Interaction handled by the target actor's interface
    // e.g., IInteractable::Execute_Interact(Target, this)
}

float ASurvivalCharacter::GetMoveSpeedModifier() const
{
    float Modifier = 1.f;
    if (SurvivalComponent->IsExhausted()) Modifier *= 0.5f;
    if (SurvivalComponent->IsHungry()) Modifier *= 0.8f;
    if (SurvivalComponent->CurrentEffect == EStatusEffect::SpiderPoison) Modifier *= 0.3f;
    if (bIsDown) Modifier = 0.f;
    return Modifier;
}
