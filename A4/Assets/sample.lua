scene = gr.node('root')

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)

hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)
grass = gr.material({0.1, 0.7, 0.1}, {0.0, 0.0, 0.0}, 0)

scene:rotate('X', 45)
scene:rotate('Z', 45)

for x = -1,1 do
	for y = -1,1 do
		s = gr.nh_sphere('s', {x * 100, y * 100, -200}, 50)
		scene:add_child(s)
		s:set_material(mat1)

		b = gr.nh_box('b', {x * 100, y * 100 , 0}, 50)
		scene:add_child(b)
		b:set_material(mat3)

 	end
end

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
orange_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene, 'sample.png', 512, 512,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, orange_light})
