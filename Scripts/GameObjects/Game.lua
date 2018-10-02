

function GameStart(isServer)
	local x, y, z = CameraGetPos(CAMERA)
	--Send out a message that we have started the game if we are the server
	if isServer
	then
		local msg = GameMessage(NETWORK_MESSAGES["ID_GAME_START"])
		NETWORK_HANDLER.Send(msg)
	end

	CreateCubePrototype(x, y, z)
end