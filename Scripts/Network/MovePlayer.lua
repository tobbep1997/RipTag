
function OnPlayerMove(objectPtr)
	local x, y ,z = CameraGetPos(CAMERA)
	local mId = NETWORK_MESSAGES["ID_UPDATE_SPHERE_LOCATION"]
	local nId = objectPtr.GetNID()
	
	NETWORK_MESSAGES.sendMovePacket(mId, nId, x, y , z)
end

function OnPlayerMoveNetwork(NID, x, y, z)
	local object = GetNIDObject(NID)
	object.Move()

end