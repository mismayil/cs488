-- Final Scene : Ray traced Backgammon

-- materials
board_mat = gr.textmaterial('Assets/backgammon.png', {0.3, 0.3, 0.3}, 20)
wood_mat = gr.textmaterial('Assets/wood.png', {0.3, 0.3, 0.3}, 20)
black_piece_mat = gr.material({0.1, 0.49, 0.39}, {0.5, 0.3, 0.5}, 10, 0, 1, 0, 1.2)
white_piece_mat = gr.material({0.96, 0.94, 0.9}, {0.5, 0.3, 0.5}, 10, 0, 1, 0, 1.2)
white = gr.material({0.9, 0.9, 0.9}, {1.0, 1.0, 1.0}, 25)
black = gr.material({0.1, 0.1, 0.1}, {0.5, 0.5, 0.5}, 25, 0.5)
mirror_mat = gr.material({0.0, 0.0, 0.0}, {0.3, 0.3, 0.3}, 10, 1.0)
meeting = gr.textmaterial('Assets/meeting.png', {0.3, 0.3, 0.3}, 20)
tmat2 = gr.textmaterial('Assets/checkerboard.png', {0.5, 0.4, 0.8}, 25)

-- create a dice
function create_dice()
	dice = gr.node('dice')

	CUBE_SIZE = 10

	cube = gr.cube('cube')
	cube:set_material(black)
	cube:scale(CUBE_SIZE, CUBE_SIZE, CUBE_SIZE)

	joint = gr.joint('joint', 2)
	dice:add_child(joint)
	joint:add_child(cube)

	ones = {}
	num_ones = 21

	for i = 1, num_ones do
		one = gr.sphere('one' .. tostring(i))
		one:set_material(white)
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

	return dice
end

scene = gr.node('root')
scene:translate(0, 0, -100)

backgammon = gr.node('backgammon')
scene:add_child(backgammon)

board = gr.cube('board')
backgammon:add_child(board)
board:set_material(board_mat)
board:scale(50, 0.1, 100)
board:translate(-25, -25, 0)

leftside = gr.cube('leftside')
backgammon:add_child(leftside)
leftside:set_material(wood_mat)
leftside:scale(0.1, 5, 100)
leftside:translate(-25, -25, 0)

rightside = gr.cube('rightside')
backgammon:add_child(rightside)
rightside:set_material(wood_mat)
rightside:scale(0.1, 5, 100)
rightside:translate(25, -25, 0)

backside = gr.cube('backside')
backgammon:add_child(backside)
backside:set_material(wood_mat)
backside:scale(100, 5, 0.1)
backside:translate(-25, -25, 0)

mirror = gr.cube('mirror')
backgammon:add_child(mirror)
mirror:set_material(mirror_mat)
mirror:scale(200, 100, 0.1)
mirror:translate(-75, -25, -5)

black_pieces = {}
num_black_pieces = 6

for i = 1, num_black_pieces do
	piece = gr.cylinder('piece' .. tostring(i))
	backgammon:add_child(piece)
	piece:set_material(black_piece_mat)
	piece:scale(2, 1, 2)
	piece:translate(0, -25, 25)
	piece:translate(math.random(-20, 20), 0, math.random(-30, 30))
	black_pieces[i] = piece
end

white_pieces = {}
num_white_pieces = 6

for i = 1, num_white_pieces do
	piece = gr.cylinder('piece' .. tostring(i))
	backgammon:add_child(piece)
	piece:set_material(white_piece_mat)
	piece:scale(2, 1, 2)
	piece:translate(-5, -25, 10)
	piece:translate(math.random(0, 20), 0, math.random(0, 30))
	white_pieces[i] = piece
end

dices = {}
dices[0] = create_dice()
dices[1] = create_dice()
dices[0]:scale(0.1, 0.1, 0.1)
dices[1]:scale(0.1, 0.1, 0.1)
dices[0]:translate(0, -25, 60)
dices[1]:translate(-5, -25, 50)
dices[0]:rotate('y', 90)
dices[1]:rotate('x', 90)
backgammon:add_child(dices[0])
backgammon:add_child(dices[1])

area_light = gr.arealight({-10, 50, -100}, {0.9, 0.9, 0.9}, {1, 0, 0}, 20, 20)
white_light = gr.light({10, 40, -80.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
--orange_light = gr.light({20, 20.0, -60.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

dof = gr.dof(5, 16)

gr.render(scene, 'images/backgammon.png', 512, 512,
	  {0, -20, -15}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {area_light}, dof)
