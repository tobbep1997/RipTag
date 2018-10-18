
function OnSend(object)

local x, y, z = CameraGetPos(CAMERA)
local id = NETWORK_MESSAGES["ID_CREATE_REMOTE_PLAYER"]
local mt = getmetatable(object)
local nid = mt.GetNID(object)

local packet = CreateEntityMsg(id, nid, x, y, z)

NETWORK_HANDLER.Send(packet)

end

function OnReceive()

end
