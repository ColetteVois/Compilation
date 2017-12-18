	jp	main
myfunc:
	libp	-4
	libp	0
	libp	-3
	move	1
	move	1
	libp	-3
	copy	1
	libp	0
	copy	1
	add
	move	1
	ret
main:
	inc	1
	push	0
	pushr	  1
	stm
	push	0
	mts
	push	0
	savebp
	inc	2
	call	myfunc
	dec	2
	rstrbp
	dec	2
	output
	halt
	end
