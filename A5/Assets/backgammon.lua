-- Final Scene : Ray traced Backgammon

-- materials
board_mat = gr.textmaterial('Assets/backgammon.png', {0.3, 0.3, 0.3}, 20)

scene = gr.node('root')
scene:translate(0, 0, -100)

board = gr.cube('board')
scene:add_child(board)
board:set_material(board_mat)
board:scale(50, 0.1, 50)
board:translate(-25, -25, 0)

area_light = gr.arealight({-10, 50, -100}, {0.9, 0.9, 0.9}, {1, 0, 0}, 20, 20)
--white_light = gr.light({-10, 45, -150.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
--orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

dof = gr.dof(5, 32)

gr.render(scene, 'backgammon.png', 512, 512,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {area_light})
