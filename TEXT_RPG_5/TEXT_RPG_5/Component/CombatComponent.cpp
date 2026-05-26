#include "CombatComponent.h"
#include "MoveComponent.h"
#include "../Manager/InputManager.h"
#include "../Manager/RenderManager.h"
#include "../Player.h"
#include "../Projectile.h"
#include "../Struct/ProjectileInfo.h"
#include "../Manager/ObjectPoolManager.h"

UCombatComponent::UCombatComponent(AObject* InOwner)
	: UComponent(InOwner)
{
	PlayerPtr = dynamic_cast<Player*>(InOwner);
	MoveComponentPtr = PlayerPtr->GetComponent<UMoveComponent>();
	this->AttackTotalTime = 0.0f;
	this->AttackDelayTime = 0.0f;
	this->ProjectileTotalTime = 0.0f;
	this->ProjectileDelayTime = 0.0f;
}

UCombatComponent::~UCombatComponent()
{

}

vector<Vector> UCombatComponent::GetAttackValue()
{
	return AttackValue;
}

void UCombatComponent::Tick(float DeltaTime)
{
	if (nullptr == MoveComponentPtr)
		return;
	
	if (PlayerPtr != nullptr)
	{
		AttackTotalTime += DeltaTime;
		ProjectileTotalTime += DeltaTime;
		
		if (InputManager::GetInstance()->IsKeyDown(eKeyCode::Z) && (AttackTotalTime >= AttackDelayTime))
		{
			if (!(PlayerPtr->GetIsAttack()))
			{
				AttackValue.clear();
				PlayerPtr->SetIsAttack(true);

				if (MoveComponentPtr->GetFacingDirection() == EDirection::UP)
				{
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y - 1 });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X - 1, PlayerPtr->GetPosition().Y - 1 });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y - 1 });
				}
				if (MoveComponentPtr->GetFacingDirection() == EDirection::DOWN)
				{
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y + 1 });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X - 1, PlayerPtr->GetPosition().Y + 1 });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y + 1 });
				}
				if (MoveComponentPtr->GetFacingDirection() == EDirection::RIGHT)
				{
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y - 1 });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y + 1 });
				}
				if (MoveComponentPtr->GetFacingDirection() == EDirection::LEFT)
				{
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X - 1, PlayerPtr->GetPosition().Y - 1 });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X - 1, PlayerPtr->GetPosition().Y });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X - 1, PlayerPtr->GetPosition().Y + 1 });
				}
				if (MoveComponentPtr->GetFacingDirection() == EDirection::NONE)
				{
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y - 1 });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X - 1, PlayerPtr->GetPosition().Y });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X + 1, PlayerPtr->GetPosition().Y });
					this->AttackValue.push_back({ PlayerPtr->GetPosition().X, PlayerPtr->GetPosition().Y + 1 });
				}
			}
			AttackTotalTime = 0.0f;
			AttackDelayTime = 2.0f;
		}
		if (InputManager::GetInstance()->IsKeyDown(eKeyCode::X) && (ProjectileTotalTime >= ProjectileDelayTime))
		{
			ProjectileInfo Info;
			Info.Range = 10;
			Info.Damage = 40;
			Info.Speed = 0.5f;

			EDirection Direction = MoveComponentPtr->GetFacingDirection();

			Projectile* ProjectileAttack = ObjectPoolManager::GetInstance()->Get<Projectile>();

			if (ProjectileAttack != nullptr)
			{
				ProjectileAttack->BeginPlay(PlayerPtr, Direction, Info);
				ProjectileAttack->Fire();
				ObjectPoolManager::GetInstance()->Return(ProjectileAttack);
			}
			
			ProjectileTotalTime = 0.0f;
			ProjectileDelayTime = 0.5f;
		}

		//RenderManager::GetInstance()->AddRender(2, 2, "타이머 : " + to_string(DeltaTime));
		//RenderManager::GetInstance()->AddRender(0, 0, "타이머 : " + to_string(ProjectileTotalTime));
	}
}
