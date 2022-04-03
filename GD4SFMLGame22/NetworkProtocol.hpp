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
		kPlayerEvent,
		kPlayerRealtimeChange,
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
		kPlayerEvent,
		kPlayerRealtimeChange,
		kPositionUpdate,
		kQuit,
		kGameEvent,
		kPlatformUpdate,
		kPlayerUpdate
	};
}
