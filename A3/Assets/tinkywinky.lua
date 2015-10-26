-- tinky-winky.lua

PI = 3.14159265359

rootnode = gr.node('root')
--rootnode:rotate('y', -20.0)
rootnode:scale(0.1, 0.1, 0.1)
rootnode:translate(0.0, 0.0, -1.0)

blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
lightwhite = gr.material({0.6, 0.6, 0.6}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

torso = gr.mesh('sphere', 'torso')
rootnode:add_child(torso)
torso:set_material(blue)
torso:scale(1.2, 2.5, 1.0);

screen = gr.mesh('cube', 'screen')
torso:add_child(screen)
screen:rotate('y', 45.0)
screen:set_material(lightwhite)
screen:scale(0.8, 0.4, 0.01)
screen:translate(0.0, -0.1, 1.0)

j_torso_head = gr.joint('j_torso_head', {-PI/4, 0, PI/4}, {-PI/2, 0, PI/2})
torso:add_child(j_torso_head)

head = gr.mesh('sphere', 'head')
j_torso_head:add_child(head)
head:scale(0.8, 0.4, 1.0)
head:translate(0.0, 1.2, 0.0)
head:set_material(blue)

neck = gr.mesh('sphere', 'neck')
torso:add_child(neck)
neck:scale(0.2, 0.2, 0.2)
neck:translate(0.0, 1.0, 0.0)
neck:set_material(blue)

leftEar = gr.mesh('sphere', 'leftEar')
head:add_child(leftEar)
--leftEar:rotate('z', 45)
leftEar:scale(0.2, 0.3, 0.3)
leftEar:translate(-1.0, -0.3, 0.0)
leftEar:set_material(white)

rightEar = gr.mesh('sphere', 'rightEar')
head:add_child(rightEar)
--rightEar:rotate('x', -90.0)
rightEar:scale(0.2, 0.3, 0.3)
rightEar:translate(1.0, -0.3, 0.0)
rightEar:set_material(white)

leftShoulder = gr.mesh('sphere', 'leftShoulder')
torso:add_child(leftShoulder)
leftShoulder:scale(0.2, 0.1, 0.2);
leftShoulder:translate(-0.5, 0.7, 0.0)
leftShoulder:set_material(blue)

j_leftShoulder_leftArm = gr.joint('j_leftShoulder_leftArm', {-PI/4, 0, PI/4}, {-PI/2, 0, PI/2})
leftShoulder:add_child(j_leftShoulder_leftArm)

leftArm = gr.mesh('sphere', 'leftArm')
j_leftShoulder_leftArm:add_child(leftArm)
leftArm:scale(6.0, 1.5, 1.0);
leftArm:rotate('z', 20)
leftArm:translate(-5.0, -1.4, 0.0)
leftArm:set_material(blue)

rightShoulder = gr.mesh('sphere', 'rightShoulder')
torso:add_child(rightShoulder)
rightShoulder:scale(0.2, 0.1, 0.2);
rightShoulder:translate(0.5, 0.7, 0.0)
rightShoulder:set_material(blue)

j_rightShoulder_rightArm = gr.joint('j_rightShoulder_rightArm', {-PI/4, 0, PI/4}, {-PI/2, 0, PI/2})
rightShoulder:add_child(j_rightShoulder_rightArm)

rightArm = gr.mesh('sphere', 'rightArm')
j_rightShoulder_rightArm:add_child(rightArm)
rightArm:scale(6.0, 1.5, 1.0);
rightArm:rotate('z', -20)
rightArm:translate(5.0, -1.4, 0.0)
rightArm:set_material(blue)

leftElbow = gr.mesh('sphere', 'elbow')
leftArm:add_child(leftElbow)
leftElbow:scale(0.1, 0.3, 0.3)
leftElbow:translate(-1.0, 0.0, 0.0)
leftElbow:set_material(blue)

j_LeftElbow_leftBotArm = gr.joint('j_leftElbow_leftBotArm', {-PI/4, 0, PI/4}, {-PI/2, 0, PI/2})
leftElbow:add_child(j_LeftElbow_leftBotArm)

leftBotArm = gr.mesh('sphere', 'leftBotArm')
j_LeftElbow_leftBotArm:add_child(leftBotArm)
leftBotArm:scale(10.0, 3.0, 1.5)
leftBotArm:translate(-4.0, 0.0, 0.0)
leftBotArm:set_material(blue)

rightElbow = gr.mesh('sphere', 'elbow')
rightArm:add_child(rightElbow)
rightElbow:scale(0.1, 0.3, 0.3)
rightElbow:translate(1.0, 0.0, 0.0)
rightElbow:set_material(blue)

j_RightElbow_rightBotArm = gr.joint('j_RightElbow_rightBotArm', {-PI/4, 0, PI/4}, {-PI/2, 0, PI/2})
rightElbow:add_child(j_RightElbow_rightBotArm)

rightBotArm = gr.mesh('sphere', 'rightBotArm')
j_RightElbow_rightBotArm:add_child(rightBotArm)
rightBotArm:scale(10.0, 3.0, 1.5)
rightBotArm:translate(4.0, 0.0, 0.0)
rightBotArm:set_material(blue)

return rootnode
