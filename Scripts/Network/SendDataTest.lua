--[[package.path = "C:\\Users\\mtlat\\Desktop\\RipTag Repos\\RipTag\\Scripts\\Network\\?.lua;" .. package.path]]

package.path = ";../" .. ";../Scripts/Network/?.lua;" .. package.path


mymod = require "RequireTest"
mymod.foo()

print(package.path) 

