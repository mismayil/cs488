white = gr.material({0.9, 0.9, 0.9}, {1.0, 1.0, 1.0}, 25)
black = gr.material({0.1, 0.1, 0.1}, {0.5, 0.5, 0.5}, 25, 0.5)

scene = gr.node('root')
scene:translate(0, 0, -100)

dice = gr.node('dice')
scene:add_child(dice)
dice:translate(-15, -15, 0)

CUBE_SIZE = 30

cube = gr.cube('cube')
cube:set_material(white)
cube:scale(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE)

joint = gr.joint('joint', 2)
dice:add_child(joint)
joint:add_child(cube)

ones = {}
num_ones = 10

for i = 1, num_ones do
	one = gr.sphere('one' .. tostring(i))
	one:set_material(black)
	one:scale(3, 3, 3)
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
		one:translate(k * CUBE_SIZE / 5, CUBE_SIZE, m * CUBE_SIZE / 5)
	end
end

for i = 1, num_ones do
	joint:add_child(ones[i])
end

dice:rotate('y', 45)
dice:rotate('x', 45)

area_light = gr.arealight({0, 50, -50}, {0.9, 0.9, 0.9}, {1, 0, 0}, 20, 20)
white_light = gr.light({-10, 45, -50.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
--orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})
black_light = gr.light({0, 50, -50}, {0.0, 0.0, 0.0}, {1, 0, 0}, 20, 20)


gr.render(scene, 'dice.png', 512, 512,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {black_light})
