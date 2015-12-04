-- Final Scene : Ray traced Backgammon

-- materials
board_mat = gr.textmaterial('Assets/backgammon.png', {0.3, 0.3, 0.3}, 20)
wood_mat = gr.textmaterial('Assets/wood.png', {0.3, 0.3, 0.3}, 20)
piece_mat = gr.material({0.1, 0.49, 0.39}, {0.5, 0.3, 0.5}, 10, 0, 1, 0, 1.2)

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

pieces = {}
num_pieces = 6

for i = 1, num_pieces do
	piece = gr.cylinder('piece' .. tostring(i))
	backgammon:add_child(piece)
	piece:set_material(piece_mat)
	piece:scale(2, 1, 2)
	piece:translate(0, -25, 25)
	piece:translate(i, 0, i * 5)
	pieces[i] = piece
end

area_light = gr.arealight({-10, 50, -100}, {0.9, 0.9, 0.9}, {1, 0, 0}, 20, 20)
--white_light = gr.light({-10, 45, -150.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
--orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

dof = gr.dof(5, 32)

gr.render(scene, 'backgammon.png', 512, 512,
	  {0, -20, -15}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {area_light})
