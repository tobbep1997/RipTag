frameH:addFrame(0.5,0.5,0.5,0.5)
frameH:addFrame(0.5,0.5,0.5,0.5)

print(frameH:getFrameAt(0):tester())

frameH:getFrameAt(0):setPosition(0.0,0.0,0,1)
frameH:getFrameAt(0):setScale(1,1,0,1)

frameH:getFrameAt(1):setPosition(0.5,0.5,0,1)
frameH:getFrameAt(1):setScale(0.5,0.5,0,1)