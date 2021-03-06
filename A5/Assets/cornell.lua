-- Cornell box

-- materials
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 5)
red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 5)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 5)
grey = gr.material({0.5, 0.5, 0.5}, {0.1, 0.1, 0.1}, 5)
white = gr.material({1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, 25)

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 0.5, 1.0, 1.2)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0.0, 1.0, 0.0)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25, 0.5, 1.33, 0.0)
mat5 = gr.material({1.0, 1.0, 1.0}, {0.5, 0.5, 0.5}, 25, 0.0, 1.0, 0.0)
tmat1 = gr.textmaterial('Assets/world.png', {0.5, 0.7, 0.5}, 25, 0.0, 1.0, 0.0)
tmat2 = gr.textmaterial('Assets/checkerboard.png', {0.5, 0.4, 0.8}, 25, 0.0, 1.0, 0.0)

scene = gr.node('root')
scene:translate(0, 0, -200)

backwall = gr.cube('backwall')
scene:add_child(backwall)
backwall:set_material(green)
backwall:scale(200, 200, 0.1)
backwall:translate(-100, -100, 0)

bottomwall = gr.cube('bottomwall')
scene:add_child(bottomwall)
bottomwall:set_material(grey)
bottomwall:scale(100, 0.1, 100)
bottomwall:translate(-50, -50, 0)

leftwall = gr.cube('leftwall')
scene:add_child(leftwall)
leftwall:set_material(blue)
leftwall:scale(0.1, 100, 100)
leftwall:translate(-50, -50, 0)

rightwall = gr.cube('rightwall')
scene:add_child(rightwall)
rightwall:set_material(red)
rightwall:scale(0.1, 100, 100)
rightwall:translate(50, -50, 0)

topwall = gr.cube('topwall')
scene:add_child(topwall)
topwall:set_material(grey)
topwall:scale(100, 0.1, 100)
topwall:translate(-50, 50, 0)

lightpanel = gr.cube('lightpanel')
scene:add_child(lightpanel)
lightpanel:set_material(white)
lightpanel:scale(20, 0.1, 20)
lightpanel:translate(-10, 50, 50)

sphere = gr.sphere('sphere')
scene:add_child(sphere)
sphere:scale(20, 20, 20)
sphere:translate(0, 0, 10)
sphere:set_material(tmat1)

cube = gr.cube('cube')
scene:add_child(cube)
cube:scale(20, 20, 20)
cube:translate(0, 0, 60)
cube:set_material(mat3)

area_light = gr.arealight({-10, 50, 0.0}, {1.0, 1.0, 1.0}, {1, 0, 0}, 20, 20)
white_light = gr.light({-10, 45, -150.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
--orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

dof = gr.dof(5, 32)

gr.render(scene, 'cornell.png', 512, 512,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {area_light})
