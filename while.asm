inc 1
push 0
pushr 3.0
stm
while:
	pushr 18.0
	push 0
	mts
	jl endwhile
	push 0
	push 0
	mts
	push 2.0
	mult
	stm
	jp while
endwhile:
	push 0
	mts
	output
	halt
	end
