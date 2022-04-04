#pragma once
#include <SFML/System/Vector2.hpp>

const unsigned short SERVER_PORT = 50000;

namespace server
{
	//These are packets that come from the Server
	enum class PacketType
	{
		kBroadcastMessage,
		kInitialState,
		kPlayerConnect,
		kPlayerDisconnect,
		kSpawnSelf,
		kUpdateClientState,
		kMissionSuccess,
		kUpdatePlatformColors,
		kUpdatePlayer
	};
}

namespace client
{
	//Messages sent from the Client
	enum class PacketType
	{
		kPositionUpdate,
		kQuit,
		kGameEvent,
		kPlatformUpdate,
		kPlayerUpdate,
		kMission
	};
}
