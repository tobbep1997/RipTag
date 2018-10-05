

function CreateCubePrototype(x, y, z)
	local object = Prototype(x, y, z)
	--print(object)
	AddPlayer(object)
	local mt = getmetatable(object)
	local nid = mt.GetNID(object)
	PLAYER_NID = nid
	--print(x, y , z)
	--print("NID: ", nid, " PLAYER NID: ", PLAYER_NID)
	NETWORK_HANDLER = Multiplayer()
	if NETWORK_HANDLER.IsConnected() and PLAYER_NID then 
		print("Game started message")
		local msg, size = CreateEntityMsg(NETWORK_MESSAGES["ID_CREATE_REMOTE_PLAYER"], nid, x, y, z)
		--print(msg)
		NETWORK_HANDLER.Send(msg, size, PACKET["LOW_PRIORITY"])
	end
end

function CreateCubePrototypeRemote(data)
	local object = PrototypeRemote(data)
	AddPlayer(object)
end

function PlayerMoved()
--print("I am in PlayerMoved\n")
	local x, y, z = CameraGetPos(CAMERA)
	print(PLAYER_NID)
	if PLAYER_NID then
		UpdateLocalPlayer(PLAYER_NID, x, y, z)
	end
--print("Packet:", msg, " Length: ", size)
	NETWORK_HANDLER = Multiplayer()
	if NETWORK_HANDLER.IsConnected() then
		if NETWORK_HANDLER.IsGameRunning() then
			print("\nIs Game Running: ", NETWORK_HANDLER.IsGameRunning())
			local msg, size = MovePacket(NETWORK_MESSAGES["ID_UPDATE_SPHERE_LOCATION"], PLAYER_NID, x, y, z)
			NETWORK_HANDLER.Send(msg, size, PACKET["HIGH_PRIORITY"])
		end
	end
end

function RemotePlayerMoved(data)
--print("I am in RemotePlayerMoved\n")
	UpdateRemotePlayer(data)
end