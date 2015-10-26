-- tinky-winky.lua

PI = 3.14159265359

rootnode = gr.node('root')
--rootnode:rotate('y', -20.0)
rootnode:scale(0.1, 0.1, 0.1)
rootnode:translate(0.0, 0.0, -1.0)

blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
lightwhite = gr.material({0.6, 0.6, 0.6}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
brown = gr.material({0.64, 0.16, 0.16}, {0.1, 0.1, 0.1}, 10)

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

j_torso_head = gr.joint('j_torso_head', {-PI/4, 0, PI/4}, {-PI/4, 0, PI/4})
torso:add_child(j_torso_head)

head = gr.mesh('sphere', 'head')
j_torso_head:add_child(head)
head:scale(0.8, 0.4, 1.0)
head:translate(0.0, 1.2, 0.0)
head:set_material(blue)

face = gr.mesh('sphere', 'face')
head:add_child(face)
face:scale(0.7, 0.7, 0.3)
face:translate(0.0, 0.0, 0.7)
face:set_material(white)

leftEye = gr.mesh('sphere', 'leftEye')
face:add_child(leftEye)
leftEye:scale(0.2, 0.2, 0.2)
leftEye:translate(-0.4, 0.0, 0.9)
leftEye:set_material(brown)

rightEye = gr.mesh('sphere', 'rightEye')
face:add_child(rightEye)
rightEye:scale(0.2, 0.2, 0.2)
rightEye:translate(0.4, 0.0, 0.9)
rightEye:set_material(brown)

leftPupil = gr.mesh('sphere', 'leftPupil')
leftEye:add_child(leftPupil)
leftPupil:scale(0.6, 0.6, 0.6)
leftPupil:translate(0.0, -0.4, 1.0)
leftPupil:set_material(black)

rightPupil = gr.mesh('sphere', 'rightPupil')
rightEye:add_child(rightPupil)
rightPupil:scale(0.6, 0.6, 0.6)
rightPupil:translate(0.0, -0.4, 1.0)
rightPupil:set_material(black)

nose = gr.mesh('sphere', 'nose')
face:add_child(nose)
nose:scale(0.4, 0.2, 0.3)
nose:translate(0.0, -0.3, 0.7)
nose:set_material(white)

noseTip = gr.mesh('sphere', 'noseTip')
nose:add_child(noseTip)
noseTip:scale(0.5, 0.8, 0.8)
noseTip:translate(0.0, -0.2, 0.6)
noseTip:set_material(white)

mouth = gr.mesh('sphere', 'mouth')
face:add_child(mouth)
mouth:scale(0.4, 0.05, 0.2)
mouth:translate(0.0, -0.6, 0.7)
mouth:set_material(black)

lip = gr.mesh('sphere', 'lip')
mouth:add_child(lip)
lip:scale(0.9, 1.0, 0.4)
lip:translate(0.0, -0.8, 0.4)
lip:set_material(black)

antennaBot = gr.mesh('cube', 'antennaBot')
head:add_child(antennaBot)
antennaBot:scale(0.1, 0.6, 0.1)
antennaBot:translate(0.0, 1.0, 0.0)
antennaBot:set_material(blue)

antennaLeft = gr.mesh('cube', 'antennaLeft')
head:add_child(antennaLeft)
antennaLeft:scale(0.1, 0.8, 0.1)
antennaLeft:rotate('z', 30)
antennaLeft:translate(-0.20, 1.51, 0.0)
antennaLeft:set_material(blue)

antennaRight = gr.mesh('cube', 'antennaRight')
head:add_child(antennaRight)
antennaRight:scale(0.1, 0.8, 0.1)
antennaRight:rotate('z', -30)
antennaRight:translate(0.20, 1.51, 0.0)
antennaRight:set_material(blue)

antennaTop = gr.mesh('cube', 'antennaTop')
head:add_child(antennaTop)
antennaTop:scale(0.8, 0.1, 0.1)
antennaTop:translate(0.0, 1.82, 0.0)
antennaTop:set_material(blue)

neck = gr.mesh('sphere', 'neck')
torso:add_child(neck)
neck:scale(0.2, 0.2, 0.2)
neck:translate(0.0, 1.0, 0.0)
neck:set_material(blue)

leftEar = gr.mesh('sphere', 'leftEar')
head:add_child(leftEar)
leftEar:rotate('z', 45)
leftEar:scale(0.2, 0.3, 0.3)
leftEar:translate(-1.0, -0.1, 0.0)
leftEar:set_material(white)

rightEar = gr.mesh('sphere', 'rightEar')
head:add_child(rightEar)
rightEar:rotate('x', -90.0)
rightEar:scale(0.2, 0.3, 0.3)
rightEar:translate(1.0, -0.1, 0.0)
rightEar:set_material(white)

leftShoulder = gr.mesh('sphere', 'leftShoulder')
torso:add_child(leftShoulder)
leftShoulder:scale(0.4, 0.2, 0.4);
leftShoulder:translate(-0.5, 0.7, 0.0)
leftShoulder:set_material(blue)

j_leftShoulder_leftArm = gr.joint('j_leftShoulder_leftArm', {-PI/6, 0, PI/8}, {-PI/4, 0, PI/4})
leftShoulder:add_child(j_leftShoulder_leftArm)

leftArm = gr.mesh('sphere', 'leftArm')
j_leftShoulder_leftArm:add_child(leftArm)
leftArm:scale(2.0, 0.7, 1.0);
leftArm:rotate('z', 40)
leftArm:translate(-2.0, -0.6, 0.0)
leftArm:set_material(blue)

rightShoulder = gr.mesh('sphere', 'rightShoulder')
torso:add_child(rightShoulder)
rightShoulder:scale(0.4, 0.2, 0.4);
rightShoulder:translate(0.5, 0.7, 0.0)
rightShoulder:set_material(blue)

j_rightShoulder_rightArm = gr.joint('j_rightShoulder_rightArm', {-PI/6, 0, PI/8}, {-PI/4, 0, PI/4})
rightShoulder:add_child(j_rightShoulder_rightArm)

rightArm = gr.mesh('sphere', 'rightArm')
j_rightShoulder_rightArm:add_child(rightArm)
rightArm:scale(2.0, 0.7, 1.0);
rightArm:rotate('z', -40)
rightArm:translate(2.0, -0.6, 0.0)
rightArm:set_material(blue)

leftElbow = gr.mesh('sphere', 'elbow')
leftArm:add_child(leftElbow)
leftElbow:scale(0.4, 1.0, 0.8)
leftElbow:translate(-1.0, 0.0, 0.0)
leftElbow:set_material(blue)

j_LeftElbow_leftBotArm = gr.joint('j_leftElbow_leftBotArm', {-PI/4, 0, PI/4}, {-PI/2, 0, PI/2})
leftElbow:add_child(j_LeftElbow_leftBotArm)

leftBotArm = gr.mesh('sphere', 'leftBotArm')
j_LeftElbow_leftBotArm:add_child(leftBotArm)
leftBotArm:scale(2.0, 1.0, 1.0)
leftBotArm:translate(-2.0, 0.0, 0.0)
leftBotArm:set_material(blue)

rightElbow = gr.mesh('sphere', 'elbow')
rightArm:add_child(rightElbow)
rightElbow:scale(0.4, 1.0, 0.8)
rightElbow:translate(1.0, 0.0, 0.0)
rightElbow:set_material(blue)

j_RightElbow_rightBotArm = gr.joint('j_RightElbow_rightBotArm', {-PI/6, 0, PI/6}, {-PI/2, 0, PI/2})
rightElbow:add_child(j_RightElbow_rightBotArm)

rightBotArm = gr.mesh('sphere', 'rightBotArm')
j_RightElbow_rightBotArm:add_child(rightBotArm)
rightBotArm:scale(2.0, 1.0, 1.0)
rightBotArm:translate(2.0, 0.0, 0.0)
rightBotArm:set_material(blue)

waist = gr.mesh('sphere', 'waist')
torso:add_child(waist)
waist:scale(1.0, 0.5, 1.0)
waist:translate(0.0, -0.5, 0.0)
waist:set_material(blue)

leftHip = gr.mesh('sphere', 'leftHip')
waist:add_child(leftHip)
leftHip:scale(0.3, 0.1, 0.3);
leftHip:translate(-0.4, -0.9, 0.0)
leftHip:set_material(blue)

j_LeftHip_LeftLeg = gr.joint('j_LeftHip_LeftLeg', {-PI/6, 0, PI/6}, {-PI/2, 0, PI/2})
leftHip:add_child(j_LeftHip_LeftLeg)

leftLeg = gr.mesh('sphere', 'leftLeg')
j_LeftHip_LeftLeg:add_child(leftLeg)
leftLeg:scale(1.2, 10.0, 2.0)
leftLeg:translate(-0.7, -1.8, 0)
leftLeg:set_material(blue)

rightHip = gr.mesh('sphere', 'rightHip')
waist:add_child(rightHip)
rightHip:scale(0.3, 0.1, 0.3);
rightHip:translate(0.4, -0.9, 0.0)
rightHip:set_material(blue)

j_RightHip_RightLeg = gr.joint('j_RightHip_RightLeg', {-PI/6, 0, PI/6}, {-PI/2, 0, PI/2})
rightHip:add_child(j_RightHip_RightLeg)

rightLeg = gr.mesh('sphere', 'rightLeg')
j_RightHip_RightLeg:add_child(rightLeg)
rightLeg:scale(1.2, 10.0, 2.0)
rightLeg:translate(0.7, -1.8, 0)
rightLeg:set_material(blue)

leftKnee = gr.mesh('sphere', 'leftKnee')
leftLeg:add_child(leftKnee)
leftKnee:scale(1.0, 1.0, 1.0)

return rootnode
