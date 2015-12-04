white = gr.material({0.9, 0.9, 0.9}, {1.0, 1.0, 1.0}, 25)
black = gr.material({0.1, 0.1, 0.1}, {0.5, 0.5, 0.5}, 25, 0.5, 1, 0, 1.2)

scene = gr.node('root')
scene:translate(0, 0, -100)

dice = gr.node('dice')
scene:add_child(dice)
dice:translate(-15, -15, 0)

CUBE_SIZE = 30

cube = gr.cube('cube')
cube:set_material(white)
cube:scale(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE)

cornersphere = gr.sphere('cornersphere')
cornersphere:set_material(white)
cornersphere:scale(2, 2, 2)
cornersphere:translate(2, CUBE_SIZE - 2, 2)

cornerbox = gr.cube('cornerbox')
cornerbox:set_material(white)
cornerbox:scale(2, 2, 2)
cornerbox:translate(-1, CUBE_SIZE - 1, -1)

jointA = gr.joint('jointA', 2)
jointA:add_child(cornerbox)
jointA:add_child(cornersphere)

jointB = gr.joint('jointB', 2)
jointB:add_child(cube)
jointB:add_child(jointA)

dice:add_child(jointB)
--dice:add_child(cube)

dice:rotate('y', 90)
--dice:rotate('x', -45)

area_light = gr.arealight({0, 50, -50}, {0.9, 0.9, 0.9}, {1, 0, 0}, 20, 20)
white_light = gr.light({-10, 45, -50.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
--orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})


gr.render(scene, 'dice.png', 512, 512,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
