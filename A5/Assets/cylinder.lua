mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 0.0, 1.0, 0.0, 1.2)
mat5 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0.5, 1.33, 0.0)
tmat1 = gr.textmaterial('Assets/world.png', {0.5, 0.7, 0.5}, 25, 0.5, 1.0, 0.0)
tmat2 = gr.textmaterial('Assets/checkerboard.png', {0.5, 0.4, 0.8}, 25, 0.0, 1.33, 0.0)

scene = gr.node('root')
--scene:translate(0, 0, -10)
scene:rotate('x', 45)

--c = gr.nh_cylinder('cylinder', {0, 0, 0}, 100, 20)
--scene:add_child(c)
--c:set_material(mat4)

c = gr.cylinder('cylinder')
scene:add_child(c)
c:scale(10, 100, 10)
c:set_material(mat4)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})
black_light = gr.light({400.0, 100.0, 150.0}, {0.0, 0.0, 0.0}, {1, 0, 0})

gr.render(scene, 'cylinder.png', 512, 512,
	  {0, 0, 300}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
