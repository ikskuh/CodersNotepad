
function love.update(dt)
	if love.keyboard.isDown("escape") then
		print("Escape!")
		love.event.quit()
	end
end

function love.draw()
	love.graphics.print("Hello World", 10, 10)
end