white = gr.material({0.9, 0.9, 0.9}, {1.0, 1.0, 1.0}, 25)
black = gr.material({0.1, 0.1, 0.1}, {0.5, 0.5, 0.5}, 25)

scene = gr.node('root')
scene:translate(0, 0, -100)

dice = gr.node('dice')
scene:add_child(dice)
dice:translate(-15, -15, 0)

CUBE_SIZE = 30

cube = gr.cube('cube')
cube:set_material(white)
cube:scale(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE)

isphere = gr.sphere('isphere')
isphere:set_material(white)
isphere:scale(CUBE_SIZE / 2 + 4, CUBE_SIZE / 2 + 4, CUBE_SIZE / 2 + 4)
isphere:translate(CUBE_SIZE / 2, CUBE_SIZE / 2, CUBE_SIZE / 2)

ccube = gr.cube('ccube')
ccube:set_material(white)
ccube:scale(4, 4, 4)
ccube:translate(0, CUBE_SIZE-3, 0)

jointA = gr.joint('jointA', 1)
jointA:add_child(cube)
jointA:add_child(isphere)

jointB = gr.joint('jointB', 2)
jointB:add_child(cube)
jointB:add_child(jointA)

dice:add_child(jointA)
--dice:add_child(cube)

dice:rotate('y', 100)
--dice:rotate('x', -45)

area_light = gr.arealight({0, 50, -50}, {0.9, 0.9, 0.9}, {1, 0, 0}, 20, 20)
white_light = gr.light({-10, 45, -50.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
--orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})


gr.render(scene, 'images/dicecorner.png', 512, 512,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
