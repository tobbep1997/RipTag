

function CreateCubePrototype(x, y, z)
	local object = Prototype(x, y, z)
	print(object)
	AddPlayer(object)
	local mt = getmetatable(object)
	local nid = mt.GetNID(object)
	print(x, y , z)
	local msg = CreateEntityMsg(NETWORK_MESSAGES["ID_CREATE_REMOTE_PLAYER"], nid, x, y, z)
	print(msg)
	NETWORK_HANDLER = Multiplayer()
	NETWORK_HANDLER.Send(msg, PACKET["LOW_PRIORITY"])
end

function CreateCubePrototypeRemote(data)
	local object = PrototypeRemote(data)
	AddPlayer(object)
end