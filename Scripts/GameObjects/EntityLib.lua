

function CreateCubePrototype(x, y, z)
	local object = Prototype(x, y, z)
	print(object)
	AddPlayer(object)
	local mt = getmetatable(object)
	local nid = mt.GetNID(object)
	PLAYER_NID = nid
	print(x, y , z)
	local msg, size = CreateEntityMsg(NETWORK_MESSAGES["ID_CREATE_REMOTE_PLAYER"], nid, x, y, z)
	print(msg)
	NETWORK_HANDLER = Multiplayer()
	NETWORK_HANDLER.Send(msg, size, PACKET["LOW_PRIORITY"])
end

function CreateCubePrototypeRemote(data)
	local object = PrototypeRemote(data)
	AddPlayer(object)
end

function PlayerMoved()
--print("I am in PlayerMoved\n")
	local x, y, z = CameraGetPos(CAMERA)
	UpdateLocalPlayer(PLAYER_NID, x, y, z)
	local msg, size = MovePacket(NETWORK_MESSAGES["ID_UPDATE_SPHERE_LOCATION"], PLAYER_NID, x, y, z)
--print("Packet:", msg, " Length: ", size)
	NETWORK_HANDLER = Multiplayer()
	NETWORK_HANDLER.Send(msg, size, PACKET["HIGH_PRIORITY"])
end

function RemotePlayerMoved(data)
--print("I am in RemotePlayerMoved\n")
	UpdateRemotePlayer(data)
end