#include "RipTagPCH.h"
#include "PlayerManager.h"

PlayerManager::PlayerManager(b3World * physWorld)
{
	this->mWorld = physWorld;
}


PlayerManager::~PlayerManager()
{
	if (mRemotePlayer)
	{
		Network::Multiplayer::RemotePlayerOnReceiveMap.clear();
		delete mRemotePlayer;
	}
	if (mLocalPlayer)
	{
		Network::Multiplayer::LocalPlayerOnSendMap.clear();
		delete mLocalPlayer;
	}
	DX::g_player = nullptr;
	DX::g_remotePlayer = nullptr;
}

void PlayerManager::RegisterThisInstanceToNetwork()
{
	using namespace Network;
	using namespace std::placeholders;

	//Receive handling
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_UPDATE, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_ABILITY, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_ANIMATION, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_THROW_BEGIN, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_THROW_END, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_ABILITY_CANCEL, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_POSESS_BEGIN, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_POSESS_END, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_CROUCH_BEGIN, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_CROUCH_END, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_WON, std::bind(&PlayerManager::_onRemotePlayerWonPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_SMOKE_DETONATE, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_BLINK, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
	Multiplayer::addToOnReceiveFuncMap(NETWORKMESSAGES::ID_PLAYER_TELEPORT, std::bind(&PlayerManager::_onRemotePlayerPacket, this, _1, _2));
}

void PlayerManager::_onRemotePlayerPacket(unsigned char id, unsigned char * data)
{
	if (mRemotePlayer && hasRemotePlayer)
	{
		mRemotePlayer->HandlePacket(id, data);
	}
}

void PlayerManager::_onRemotePlayerWonPacket(unsigned char id, unsigned char * data)
{
	Network::ENTITYSTATEPACKET * pData = (Network::ENTITYSTATEPACKET*)data;
	mRemotePlayer->hasWon = pData->condition;
}

void PlayerManager::_onRemotePlayerDisconnect(unsigned char id, unsigned char * data)
{
	if (mRemotePlayer && hasRemotePlayer)
	{
		delete mRemotePlayer;
		mRemotePlayer = nullptr;
		hasRemotePlayer = false;
	}
}

void PlayerManager::Init(b3World * physWorld)
{
	this->mWorld = physWorld;
}

void PlayerManager::Update(float dt)
{
	static float accumulatedDT = 0;
	static const float frequency = 1.0f / 100.f; //one update each 10 ms

	accumulatedDT += dt;

	if (mRemotePlayer && hasRemotePlayer)
		mRemotePlayer->Update(dt);
	if (mLocalPlayer && hasLocalPlayer)
	{
		mLocalPlayer->Update(dt);
		const AudioEngine::Listener & pl = mLocalPlayer->getFMODListener();
		AudioEngine::UpdateListenerAttributes(pl);
	}
	if (hasRemotePlayer && hasLocalPlayer)
	{
		if (accumulatedDT >= frequency)
		{
			accumulatedDT -= frequency;
			mLocalPlayer->SendOnUpdateMessage();
			mLocalPlayer->SendAbilityUpdates();
		}
		mLocalPlayer->SendOnAnimationUpdate(dt);
	}

}

void PlayerManager::PhysicsUpdate()
{
	if (mLocalPlayer && hasLocalPlayer)
		mLocalPlayer->PhysicsUpdate();
}

void PlayerManager::Draw()
{
	if (mRemotePlayer && hasRemotePlayer)
	{
		mRemotePlayer->Draw();
		mRemotePlayer->getPosition();
	}
	if (mLocalPlayer && hasLocalPlayer)
		mLocalPlayer->Draw();
}

void PlayerManager::isCoop(bool coop)
{
	if (mLocalPlayer && coop)
		mLocalPlayer->RegisterThisInstanceToNetwork();
	if (mRemotePlayer && coop)
		this->RegisterThisInstanceToNetwork();
}

void PlayerManager::CreateLocalPlayer(DirectX::XMFLOAT4A pos)
{

	if (!mLocalPlayer)
	{
		hasLocalPlayer = true;
		mLocalPlayer = new Player();
		mLocalPlayer->Init(*this->mWorld, e_dynamicBody, 1.0f, 1.25f, 1.0f);
		mLocalPlayer->setEntityType(EntityType::PlayerType);
		mLocalPlayer->setColor(1.f, 1.f, 1.f, 1.f);
		//this->setModel(Manager::g_meshManager.getSkinnedMesh("PLAYER1"));
		//mLocalPlayer->setModel(Manager::g_meshManager.getStaticMesh("SPHERE"));
		mLocalPlayer->setModel(Manager::g_meshManager.getSkinnedMesh("PLAYER1"));
		mLocalPlayer->setTexture(Manager::g_textureManager.getTexture("PLAYER1"));
		mLocalPlayer->setModelTransform(DirectX::XMMatrixTranslation(0.0, -1.7f, 0.0f));
		mLocalPlayer->CastShadows(true);
		{
			std::vector<SharedAnimation> sharedAnimations;
			const char * collection = "PLAYER1";
			int nrOfStates = 5;

			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "IDLE_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_F_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_B_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_FL_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_FR_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_BL_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "W_BR_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "R_F_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "R_FL_ANIMATION"));
			sharedAnimations.push_back(Manager::g_animationManager.getAnimation(collection, "R_FR_ANIMATION"));

			mLocalPlayer->getAnimationPlayer()->Play();
			mLocalPlayer->getAnimationPlayer()->SetSkeleton(Manager::g_animationManager.getSkeleton(collection));

			std::unique_ptr<SM::AnimationStateMachine>& stateMachine = mLocalPlayer->getAnimationPlayer()->InitStateMachine(nrOfStates);

			{
				//Blend spaces - forward&backward
				SM::BlendSpace2D * blend_fwd = stateMachine->AddBlendSpace2DState(
					"walk_forward", //state name
					&mLocalPlayer->m_currentDirection, //x-axis driver
					&mLocalPlayer->m_currentSpeed, //y-axis driver
					-115.f, 115.f, //x-axis bounds
					0.0f, 6.0f //y-axis bounds
				);
				SM::BlendSpace2D * blend_bwd = stateMachine->AddBlendSpace2DState(
					"walk_backward", //state name
					&mLocalPlayer->m_currentDirection, //x-axis driver
					&mLocalPlayer->m_currentSpeed, //y-axis driver
					-180.f, 180.f, //x-axis bounds
					0.0f, 3.001f //y-axis bounds
				);

				//Add blendspace rows 
				//forward
				blend_fwd->AddRow(
					0.0f, //y placement
					{	//uses a vector initializer list for "convinience"
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), -115.f }, //the clip to use and x-placement
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), 0.f },
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), 115.f }
					}
				);
				blend_fwd->AddRow(
					3.1f, //y placement
					{	//uses a vector initializer list for "convinience"
						{ sharedAnimations[RemotePlayer::AnimState::RIGHT].get(), -115.f }, //the clip to use and x-placement
						{ sharedAnimations[RemotePlayer::AnimState::FORWARD].get(), 0.f },
						{ sharedAnimations[RemotePlayer::AnimState::LEFT].get(), 115.f }
					}
				);
				blend_fwd->AddRow(
					6.0f, //y placement
					{	//uses a vector initializer list for "convinience"
						{ sharedAnimations[RemotePlayer::AnimState::RUN_FORWARD_RIGHT].get(), -115.f }, //the clip to use and x-placement
						{ sharedAnimations[RemotePlayer::AnimState::RUN_FORWARD].get(), 0.f },
						{ sharedAnimations[RemotePlayer::AnimState::RUN_FORWARD_LEFT].get(), 115.f }
					}
				);
				//
				blend_bwd->AddRow(
					0.0f, //y placement
					{	//uses a vector initializer list for "convinience"
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), -180.f }, //the clip to use and x-placement
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), -90.f },
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), 0.f },
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), 90.f },
						{ sharedAnimations[RemotePlayer::AnimState::IDLE].get(), 180.f }
					}
				);
				blend_bwd->AddRow(
					3.1f, //y placement
					{	//uses a vector initializer list for "convinience"
						{ sharedAnimations[RemotePlayer::AnimState::BACKWARD].get(), -180.f }, //the clip to use and x-placement
						{ sharedAnimations[RemotePlayer::AnimState::BACK_RIGHT].get(), -90.f },
						{ sharedAnimations[RemotePlayer::AnimState::FORWARD].get(), 0.f },
						{ sharedAnimations[RemotePlayer::AnimState::BACK_LEFT].get(), 90.f },
						{ sharedAnimations[RemotePlayer::AnimState::BACKWARD].get(), 180.f }
					}
				);
				blend_bwd->AddRow(
					6.0f, //y placement
					{	//uses a vector initializer list for "convinience"
						{ sharedAnimations[RemotePlayer::AnimState::BACKWARD].get(), -180.f }, //the clip to use and x-placement
						{ sharedAnimations[RemotePlayer::AnimState::BACK_RIGHT].get(), -90.f },
						{ sharedAnimations[RemotePlayer::AnimState::FORWARD].get(), 0.f },
						{ sharedAnimations[RemotePlayer::AnimState::BACK_LEFT].get(), 90.f },
						{ sharedAnimations[RemotePlayer::AnimState::BACKWARD].get(), 180.f }
					}
				);

				//Adding out state / transitions
				SM::OutState & fwd_bwd_outstate = blend_fwd->AddOutState(blend_bwd);
				//Add transition condition
				fwd_bwd_outstate.AddTransition(
					&mLocalPlayer->m_currentDirection, //referenced variable for comparision
					-115.f, 115.f, //bound range for comparision
					SM::COMPARISON_OUTSIDE_RANGE //comparision condition
				);

				SM::OutState & bwd_fwd_outstate = blend_bwd->AddOutState(blend_fwd);
				//Add transition condition
				bwd_fwd_outstate.AddTransition(
					&mLocalPlayer->m_currentDirection, //referenced variable for comparision
					-90.f, 90.f, //bound range for comparision
					SM::COMPARISON_INSIDE_RANGE //comparision condition
				);

				auto throwBeginClip = Manager::g_animationManager.getAnimation(collection, "THROW_BEGIN_ANIMATION").get();
				auto throwHoldClip = Manager::g_animationManager.getAnimation(collection, "THROW_HOLD_ANIMATION").get();
				auto throwEndClip = Manager::g_animationManager.getAnimation(collection, "THROW_END_ANIMATION").get();
				auto posessClip = Manager::g_animationManager.getAnimation(collection, "POSESSING_ANIMATION").get();
				auto crouchClip = Manager::g_animationManager.getAnimation(collection, "CROUCH_POSE_ANIMATION").get();

				auto leanLeftPose = &Manager::g_animationManager.getAnimation(collection, "LEAN_LEFT_ANIMATION").get()->m_SkeletonPoses[0];
				auto leanRightPose = &Manager::g_animationManager.getAnimation(collection, "LEAN_RIGHT_ANIMATION").get()->m_SkeletonPoses[0];

				auto holdState = stateMachine->AddLoopState("throw_hold", throwHoldClip);
				stateMachine->AddAutoTransitionState("throw_begin", throwBeginClip, holdState);
				auto throwEndState = stateMachine->AddAutoTransitionState("throw_end", throwEndClip, blend_fwd);
				throwEndState->SetDefaultBlendTime(0.05f);
				auto posessState = stateMachine->AddLoopState("posessing", posessClip);
				posessState->SetDefaultBlendTime(0.3f);
				//set initial state
				stateMachine->SetState("walk_forward");

				auto& layerMachine = mLocalPlayer->getAnimationPlayer()->InitLayerMachine(Manager::g_animationManager.getSkeleton(collection).get());
				auto crouchState = layerMachine->AddBasicLayer("crouch", crouchClip, 0.0, 0.0);
				crouchState->UseFirstPoseOnly(true);

				auto leanState = layerMachine->Add1DPoseLayer("peek", &mLocalPlayer->m_currentPeek, -1.0f, 1.0f, { {leanRightPose, -1.0f}, {leanLeftPose, 1.0f} });
				leanState->UseSmoothDriver(false);
				layerMachine->ActivateLayer("peek");
				
			}

			/*auto animationPlayer = mLocalPlayer->getAnimationPlayer();

			animationPlayer->SetSkeleton(Manager::g_animationManager.getSkeleton("PLAYER1"));
			auto& stateMachine = animationPlayer->InitStateMachine(1);
			stateMachine->AddLoopState("temp", Manager::g_animationManager.getAnimation("PLAYER1", "POSESSING_ANIMATION").get());
			stateMachine->SetState("temp");
			animationPlayer->Play();*/
		}
		mLocalPlayer->setScale(.45, .45, .45);
		mLocalPlayer->setModelTransform(DirectX::XMMatrixTranslation(0.0, -1.0, 0.0));
		mLocalPlayer->setPosition(0.0f, 0.0f, 0.0f);
		mLocalPlayer->setTextureTileMult(2, 2);
	}
}

void PlayerManager::CreateRemotePlayer(DirectX::XMFLOAT4A pos, RakNet::NetworkID nid)
{
	if (!hasRemotePlayer)
	{
		DirectX::XMFLOAT4A scale = DirectX::XMFLOAT4A(0.015f, 0.015f, 0.015f, 1.0f);
		DirectX::XMFLOAT4A rot = { 0.0, 0.0, 0.0, 0.0 };
		this->mRemotePlayer = new RemotePlayer(nid, pos, scale, rot);
		this->mRemotePlayer->setEntityType(EntityType::RemotePlayerType);
		hasRemotePlayer = true;
		DX::g_remotePlayer = mRemotePlayer;
	}
}

void PlayerManager::DestroyRemotePlayer()
{
	if (mRemotePlayer)
		delete mRemotePlayer;
	mRemotePlayer = nullptr;
	hasRemotePlayer = false;
	DX::g_remotePlayer = nullptr;
}


Player * PlayerManager::getLocalPlayer()
{
	return mLocalPlayer;
}

RemotePlayer * PlayerManager::getRemotePlayer()
{
	return this->mRemotePlayer;
}

bool PlayerManager::isGameWon()
{
	if (hasLocalPlayer && hasRemotePlayer)
		return (mLocalPlayer->getWinState() && mRemotePlayer->hasWon);
	else
		return mLocalPlayer->getWinState();
}
