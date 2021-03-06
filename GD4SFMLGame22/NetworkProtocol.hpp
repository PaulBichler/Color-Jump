#pragma once
#include <SFML/System/Vector2.hpp>

const unsigned short SERVER_PORT = 50000;

namespace server
{
	//These are packets that come from the Server
	enum class PacketType
	{
		kInitialState,
		kPlayerConnect,
		kPlayerDisconnect,
		kSpawnSelf,
		kUpdateClientState,
		kMissionSuccess,
		kUpdatePlatformColors,
		kUpdatePlayer,
		kRespawnTeam,
		kSetTeamCheckpoint,
		kTeamSelection,
		kStartGame,
		kStartGameCountdown,
		kLobbyUpdate
	};
}

namespace client
{
	//Messages sent from the Client
	enum class PacketType
	{
		kPositionUpdate,
		kQuit,
		kPlatformUpdate,
		kPlayerUpdate,
		kGoalReached,
		kTeamChange,
		kStartNetworkGame,
		kStartNetworkGameCountdown,
		kTeamDeath,
		kCheckpointReached
	};
}
