-- Cornell box

-- materials
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 5)
red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 5)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 5)
grey = gr.material({0.5, 0.5, 0.5}, {0.1, 0.1, 0.1}, 5)
white = gr.material({1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, 25)
mirror = gr.material({0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, 5, 0.5)

mat1 = gr.material({0.3, 0.3, 0.5}, {0.5, 0.3, 0.5}, 10, 0.5)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)
mat5 = gr.material({1.0, 1.0, 1.0}, {0.5, 0.5, 0.5}, 25)
world = gr.textmaterial('Assets/world.png', {0.5, 0.7, 0.5}, 25)
checkerboard = gr.textmaterial('Assets/checkerboard.png', {0.5, 0.4, 0.8}, 25)
parket = gr.textmaterial('Assets/parket.png', {0.5, 0.4, 0.8}, 25)

scene = gr.node('root')
scene:translate(0, 0, -200)

backwall = gr.cube('backwall')
scene:add_child(backwall)
backwall:set_material(green)
backwall:scale(200, 200, 0.1)
backwall:translate(-100, -100, 0)

bottomwall = gr.cube('bottomwall')
scene:add_child(bottomwall)
bottomwall:set_material(checkerboard)
bottomwall:scale(100, 0.1, 100)
bottomwall:translate(-50, -50, 0)

leftwall = gr.cube('leftwall')
scene:add_child(leftwall)
leftwall:set_material(mirror)
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

cylinder = gr.cylinder('cylinder')
scene:add_child(cylinder)
cylinder:scale(10, 20, 10)
cylinder:translate(10, -25, 30)
cylinder:set_material(mat3)

cone = gr.cone('cone')
scene:add_child(cone)
cone:scale(10, 20, 10)
cone:translate(10, -20, 60)
cone:set_material(mat4)

area_light = gr.arealight({-10, 50, -100}, {0.9, 0.9, 0.9}, {1, 0, 0}, 20, 20)
white_light = gr.light({10, 40, -80.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({20, 20.0, -60.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

dof = gr.dof(5, 32)

gr.render(scene, 'images/primitives.png', 512, 512,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {area_light})
