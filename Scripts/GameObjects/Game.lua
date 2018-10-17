

function GameStart(isServer)
	print(isServer)
	print("I am in GameStart")
	local x, y, z = CameraGetPos(CAMERA)
	--print (x, y, z)
	--Send out a message that we have started the game if we are the server
	if isServer
	then
		local msg, size = GameMessage(NETWORK_MESSAGES["ID_GAME_START"])
		print (msg)
		
		NETWORK_HANDLER = Multiplayer()
		NETWORK_HANDLER.Send(msg, size, PACKET["IMMEDIATE_PRIORITY"])
		
	end

	CreateCubePrototype(x, y, z)
end