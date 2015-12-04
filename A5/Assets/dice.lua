white = gr.material({0.9, 0.9, 0.9}, {1.0, 1.0, 1.0}, 25)
black = gr.material({0.0, 0.0, 0.0}, {0.5, 0.5, 0.5}, 25)

scene = gr.node('root')
scene:translate(0, 0, -100)

dice = gr.node('dice')
scene:add_child(dice)

cube = gr.cube('cube')
dice:add_child(cube)
cube:set_material(white)
cube:scale(20, 20, 20)
cube:translate(-15, -15, 0)

area_light = gr.arealight({0, 50, -100}, {0.9, 0.9, 0.9}, {1, 0, 0}, 20, 20)
white_light = gr.light({-10, 45, -100.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
--orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})


gr.render(scene, 'dice.png', 512, 512,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
