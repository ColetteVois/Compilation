inc 1
push 0
pushr 10.0
stm
if:
	pushr 15.0
	push 0
	mts
	outchar
	jg else
	outchar "Asuperieur15"
	jp endif
else:
	outchar "Ainferieur15"
endif:
	outchar "endif"
	halt
	end 
	
