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

j_LeftElbow_leftBotArm = gr.joint('j_leftElbow_leftBotArm', {-PI/4, 0, PI/4}, {-PI/4, 0, PI/4})
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

j_RightElbow_rightBotArm = gr.joint('j_RightElbow_rightBotArm', {-PI/6, 0, PI/6}, {-PI/4, 0, PI/4})
rightElbow:add_child(j_RightElbow_rightBotArm)

rightBotArm = gr.mesh('sphere', 'rightBotArm')
j_RightElbow_rightBotArm:add_child(rightBotArm)
rightBotArm:scale(2.0, 1.0, 1.0)
rightBotArm:translate(2.0, 0.0, 0.0)
rightBotArm:set_material(blue)

leftWrist = gr.mesh('sphere', 'leftWrist')
leftBotArm:add_child(leftWrist)
leftWrist:scale(0.2, 0.4, 0.4)
leftWrist:translate(-1.0, 0.0, 0.0)
leftWrist:set_material(blue)

j_LeftWrist_LeftFinger1 = gr.joint('j_LeftWrist_LeftFinger1', {-PI/6, 0, PI/6}, {-PI/4, 0, PI/4})
leftWrist:add_child(j_LeftWrist_LeftFinger1)

j_LeftWrist_LeftFinger2 = gr.joint('j_LeftWrist_LeftFinger2', {-PI/6, 0, PI/6}, {-PI/4, 0, PI/4})
leftWrist:add_child(j_LeftWrist_LeftFinger2)

j_LeftWrist_LeftFinger3 = gr.joint('j_LeftWrist_LeftFinger3', {-PI/6, 0, PI/6}, {-PI/4, 0, PI/4})
leftWrist:add_child(j_LeftWrist_LeftFinger3)

leftFinger1 = gr.mesh('sphere', 'leftFinger1')
j_LeftWrist_LeftFinger1:add_child(leftFinger1)
leftFinger1:scale(2.0, 0.5, 0.5)
leftFinger1:rotate('z', -20)
leftFinger1:translate(-1.2, 0.4, 0.0)
leftFinger1:set_material(blue)

leftFinger2 = gr.mesh('sphere', 'leftFinger2')
j_LeftWrist_LeftFinger2:add_child(leftFinger2)
leftFinger2:scale(2.0, 0.5, 0.5)
leftFinger2:rotate('z', 10)
leftFinger2:translate(-1.2, 0.2, 0.0)
leftFinger2:set_material(blue)

leftFinger3 = gr.mesh('sphere', 'leftFinger3')
j_LeftWrist_LeftFinger3:add_child(leftFinger3)
leftFinger3:scale(2.0, 0.5, 0.5)
leftFinger3:rotate('z', 30)
leftFinger3:translate(-1.2, -0.1, 0.0)
leftFinger3:set_material(blue)

rightWrist = gr.mesh('sphere', 'rightWrist')
rightBotArm:add_child(rightWrist)
rightWrist:scale(0.2, 0.4, 0.4)
rightWrist:translate(1.0, 0.0, 0.0)
rightWrist:set_material(blue)

j_RightWrist_RightFinger1 = gr.joint('j_RightWrist_RightFinger1', {-PI/6, 0, PI/6}, {-PI/2, 0, PI/2})
rightWrist:add_child(j_RightWrist_RightFinger1)

j_RightWrist_RightFinger2 = gr.joint('j_RightWrist_RightFinger2', {-PI/6, 0, PI/6}, {-PI/2, 0, PI/2})
rightWrist:add_child(j_RightWrist_RightFinger2)

j_RightWrist_RightFinger3 = gr.joint('j_RightWrist_RightFinger3', {-PI/6, 0, PI/6}, {-PI/2, 0, PI/2})
rightWrist:add_child(j_RightWrist_RightFinger3)

rightFinger1 = gr.mesh('sphere', 'rightFinger1')
j_RightWrist_RightFinger1:add_child(rightFinger1)
rightFinger1:scale(2.0, 0.5, 0.5)
rightFinger1:rotate('z', 20)
rightFinger1:translate(1.2, 0.4, 0.0)
rightFinger1:set_material(blue)

rightFinger2 = gr.mesh('sphere', 'rightFinger2')
j_RightWrist_RightFinger2:add_child(rightFinger2)
rightFinger2:scale(2.0, 0.5, 0.5)
rightFinger2:rotate('z', 0)
rightFinger2:translate(1.2, 0.2, 0.0)
rightFinger2:set_material(blue)

rightFinger3 = gr.mesh('sphere', 'rightFinger3')
j_RightWrist_RightFinger3:add_child(rightFinger3)
rightFinger3:scale(2.0, 0.5, 0.5)
rightFinger3:rotate('z', -20)
rightFinger3:translate(1.2, -0.1, 0.0)
rightFinger3:set_material(blue)

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

j_LeftHip_LeftThigh = gr.joint('j_LeftHip_LeftThigh', {-PI/10, 0, PI/10}, {-PI/2, 0, PI/2})
leftHip:add_child(j_LeftHip_LeftThigh)

leftThigh = gr.mesh('sphere', 'leftThigh')
j_LeftHip_LeftThigh:add_child(leftThigh)
leftThigh:scale(1.2, 10.0, 2.0)
leftThigh:translate(-0.7, -1.8, 0)
leftThigh:set_material(blue)

rightHip = gr.mesh('sphere', 'rightHip')
waist:add_child(rightHip)
rightHip:scale(0.3, 0.1, 0.3);
rightHip:translate(0.4, -0.9, 0.0)
rightHip:set_material(blue)

j_RightHip_RightThigh = gr.joint('j_RightHip_RightThigh', {-PI/10, 0, PI/10}, {-PI/2, 0, PI/2})
rightHip:add_child(j_RightHip_RightThigh)

rightThigh = gr.mesh('sphere', 'rightThigh')
j_RightHip_RightThigh:add_child(rightThigh)
rightThigh:scale(1.2, 10.0, 2.0)
rightThigh:translate(0.7, -1.8, 0)
rightThigh:set_material(blue)

leftKnee = gr.mesh('sphere', 'leftKnee')
leftThigh:add_child(leftKnee)
leftKnee:scale(0.8, 0.3, 0.8)
leftKnee:translate(0.0, -0.9, 0.0)
leftKnee:set_material(blue)

rightKnee = gr.mesh('sphere', 'rightKnee')
rightThigh:add_child(rightKnee)
rightKnee:scale(0.8, 0.3, 0.8)
rightKnee:translate(0.0, -0.9, 0.0)
rightKnee:set_material(blue)

j_LeftKnee_LeftLeg = gr.joint('j_LeftKnee_LeftLeg', {-PI/6, 0, PI/6}, {-PI/2, 0, PI/2})
leftKnee:add_child(j_LeftKnee_LeftLeg)

leftLeg = gr.mesh('sphere', 'leftLeg')
j_LeftKnee_LeftLeg:add_child(leftLeg)
leftLeg:scale(1.0, 3.0, 1.0)
leftLeg:translate(0.0, -2.0, 0.0)
leftLeg:set_material(blue)

j_RightKnee_RightLeg = gr.joint('j_RightKnee_RightLeg', {-PI/6, 0, PI/6}, {-PI/2, 0, PI/2})
rightKnee:add_child(j_RightKnee_RightLeg)

rightLeg = gr.mesh('sphere', 'rightLeg')
j_RightKnee_RightLeg:add_child(rightLeg)
rightLeg:scale(1.0, 3.0, 1.0)
rightLeg:translate(0.0, -2.0, 0.0)
rightLeg:set_material(blue)

leftAnkle = gr.mesh('sphere', 'leftAnkle')
leftLeg:add_child(leftAnkle)
leftAnkle:scale(0.5, 0.3, 0.5)
leftAnkle:translate(0.0, -0.8, 0.0)
leftAnkle:set_material(blue)

j_LeftAnkle_LeftFeet = gr.joint('j_LeftAnkle_LeftFeet', {-PI/6, 0, PI/6}, {-PI/2, 0, PI/2})
leftAnkle:add_child(j_LeftAnkle_LeftFeet)

leftFeet = gr.mesh('sphere', 'leftFeet')
j_LeftAnkle_LeftFeet:add_child(leftFeet)
leftFeet:scale(2.0, 0.8, 2.5)
leftFeet:translate(0.0, -0.8, 0.5)
leftFeet:set_material(blue)

rightAnkle = gr.mesh('sphere', 'rightAnkle')
rightLeg:add_child(rightAnkle)
rightAnkle:scale(0.5, 0.3, 0.5)
rightAnkle:translate(0.0, -0.8, 0.0)
rightAnkle:set_material(blue)

j_RightAnkle_RightFeet = gr.joint('j_RightAnkle_RightFeet', {-PI/6, 0, PI/6}, {-PI/2, 0, PI/2})
rightAnkle:add_child(j_RightAnkle_RightFeet)

rightFeet = gr.mesh('sphere', 'rightFeet')
j_RightAnkle_RightFeet:add_child(rightFeet)
rightFeet:scale(2.0, 0.8, 2.5)
rightFeet:translate(0.0, -0.6, 0.5)
rightFeet:set_material(blue)

return rootnode
