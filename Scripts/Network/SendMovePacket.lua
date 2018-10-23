local A = {}

local function sendMovePacket(mId, networkId, x, y, z)
	local data = MovePacket(mId, networkId, x, y, z)

	NETWORK_HANDLER.Send(data)
end

A.sendMovePacket = sendMovePacket

return A