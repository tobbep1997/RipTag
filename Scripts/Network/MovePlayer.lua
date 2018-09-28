function sendMovePacket
	local data = Move(x, y, z, w, networkId)

	run SendData.lua
	network = Multiplayer()

	network.Send(data)
end