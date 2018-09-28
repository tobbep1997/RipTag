local A = {}

local function camHehe()
	local x, y, z = CameraGetPos(CAMERA)
	print(x, y, z)
end

A.camHehe = camHehe

return A