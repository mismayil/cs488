-- Final Scene : Ray traced Backgammon

-- materials
board_mat = gr.textmaterial('Assets/backgammon.png', {0.3, 0.3, 0.3}, 20, 0.5)
wood_mat = gr.textmaterial('Assets/wood.png', {0.3, 0.3, 0.3}, 20)
black_piece_mat = gr.material({0.1, 0.49, 0.39}, {0.5, 0.3, 0.5}, 10, 0, 1, 0, 1.2)
white_piece_mat = gr.material({0.96, 0.94, 0.9}, {0.5, 0.3, 0.5}, 10, 0, 1, 0, 1.2)
white = gr.material({0.9, 0.9, 0.9}, {1.0, 1.0, 1.0}, 25, 0.5, 1.3, 0.3)
black = gr.material({0.1, 0.1, 0.1}, {0.5, 0.5, 0.5}, 25)
meeting = gr.textmaterial('Assets/meeting.png', {0.3, 0.3, 0.3}, 20)
tram = gr.textmaterial('Assets/tram.png', {0.3, 0.3, 0.3}, 10)

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

background = gr.cube('background')
scene:add_child(background)
background:set_material(tram)
background:scale(200, 100, 0.1)
background:translate(-80, -60, 0)

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
dices[0]:translate(5, -25, 60)
dices[1]:translate(-5, -25, 70)
dices[0]:rotate('y', 270)
dices[1]:rotate('y', 135)
backgammon:add_child(dices[0])
backgammon:add_child(dices[1])


area_light = gr.arealight({-10, 50, -100}, {0.9, 0.9, 0.9}, {1, 0, 0}, 20, 20)
white_light = gr.light({10, 40, -80.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
--orange_light = gr.light({20, 20.0, -60.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

dof = gr.dof(5, 16)

gr.render(scene, 'images/backgammon.png', 512, 512,
	  {0, -20, -15}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {area_light})
