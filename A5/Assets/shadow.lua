mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0.5, 1.0, 0.0)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 0.5, 1.0, 1.2)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0.5, 1.0, 0.0)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 0.5, 1.33, 0.0)
mat5 = gr.material({1.0, 1.0, 1.0}, {0.5, 0.5, 0.5}, 25, 0.5, 1.0, 0.0)
tmat1 = gr.textmaterial('Assets/world.png', {0.5, 0.7, 0.5}, 25, 0.0, 1.0, 0.0)
tmat2 = gr.textmaterial('Assets/checkerboard.png', {0.5, 0.4, 0.8}, 25, 0.0, 1.0, 0.0)

scene = gr.node('root')
scene:translate(0, 0, -100)

wall = gr.cube('wall')
scene:add_child(wall)
wall:scale(200, 0.1, 200)
wall:translate(-100, -20, -150)
--wall:rotate('x', 45)
wall:set_material(mat3)

--cube = gr.cube('cube')
--scene:add_child(cube)
--cube:scale(10, 10, 10)
--cube:translate(-10, -10, 50)
--cube:set_material(mat5)

sphere = gr.sphere('sphere')
scene:add_child(sphere)
sphere:scale(10, 10, 10)
sphere:translate(0, 0, 10)
sphere:set_material(mat5)

area_light = gr.arealight({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0}, 30, 30)
white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene, 'shadow.png', 512, 512,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {area_light})
