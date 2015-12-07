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
tmat1 = gr.textmaterial('Assets/world.png', {0.5, 0.7, 0.5}, 25)
tmat2 = gr.textmaterial('Assets/checkerboard.png', {0.5, 0.4, 0.8}, 25)

scene = gr.node('root')
scene:translate(0, 0, -200)

-- create a dice
function create_dice()
	dice = gr.node('dice')

	CUBE_SIZE = 30

	cube = gr.cube('cube')
	cube:set_material(white)
	cube:scale(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE)

	joint = gr.joint('joint', 2)
	joint:add_child(cube)

	ones = {}
	num_ones = 21

	for i = 1, num_ones do
		one = gr.sphere('one' .. tostring(i))
		one:set_material(black)
		one:scale(2, 2, 2)
		ones[i] = one
	end

	i = 1
	one = ones[i]
	one:translate(CUBE_SIZE / 2, CUBE_SIZE / 2, CUBE_SIZE)

	for k = 1, 2 do
		i = i + 1
		one = ones[i]
		one:translate(0, k * CUBE_SIZE / 3, k * CUBE_SIZE / 3)
	end

	for k = 1, 3 do
		i = i + 1
		one = ones[i]
		one:translate(k * CUBE_SIZE / 4, 0, k * CUBE_SIZE / 4)
	end

	for k = 1, 2 do
		for m = 1, 2 do
			i = i + 1
			one = ones[i]
			one:translate(k * CUBE_SIZE / 3, CUBE_SIZE, m * CUBE_SIZE / 3)
		end
	end

	for k = 1, 2 do
		for m = 1, 2 do
			i = i + 1
			one = ones[i]
			one:translate(CUBE_SIZE, k * CUBE_SIZE / 3, m * CUBE_SIZE / 3)
		end
	end

	i = i + 1
	one = ones[i]
	one:translate(CUBE_SIZE, CUBE_SIZE / 2, CUBE_SIZE / 2)

	for k = 1, 2 do
		for m = 1, 3 do
			i = i + 1
			one = ones[i]
			one:translate(k * CUBE_SIZE / 3, m * CUBE_SIZE / 4, 0)
		end
	end

	for i = 1, num_ones do
		joint:add_child(ones[i])
	end

	isphere = gr.sphere('isphere')
	isphere:set_material(white)
	isphere:scale(CUBE_SIZE / 2 + 4, CUBE_SIZE / 2 + 4, CUBE_SIZE / 2 + 4)
	isphere:translate(CUBE_SIZE / 2, CUBE_SIZE / 2, CUBE_SIZE / 2)

	jointA = gr.joint('jointA', 1)
	jointA:add_child(joint)
	jointA:add_child(isphere)

	dice:add_child(jointA)

	return dice
end

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
rightwall:set_material(mirror)
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

dice = create_dice()
scene:add_child(dice)
dice:scale(0.1, 0.1, 0.1)
dice:translate(10, -25, 40)

area_light = gr.arealight({-10, 50, -100}, {0.9, 0.9, 0.9}, {1, 0, 0}, 20, 20)
white_light = gr.light({10, 40, -80.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({20, 20.0, -60.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

dof = gr.dof(5, 32)

gr.render(scene, 'images/reflection.png', 512, 512,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {area_light})
