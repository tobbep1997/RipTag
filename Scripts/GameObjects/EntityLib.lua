

function CreateCubePrototype(x, y, z)
	local object = Prototype(x, y, z)
	AddPlayer(object)
	local mt = getmetatable(object)
	local nid = mt.GetNID(object)
	
	local msg = CreateEntityMsg(NETWORK_MESSAGES["ID_CREATE_REMOTE_PLAYER"], nid, x, y, z)
	
	NETWORK_HANDLER.Send(msg)
end

function CreateCubePrototypeRemote(data)
	local object = PrototypeRemote(data)
	AddPlayer(object)
end