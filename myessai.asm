	inc	0
	jp	main
f2:
	libp	-3
	libp	0
	pushr	1.000000
	move	1
	libp	-2
	copy	1
	pushr	1.0
	add
	move	1
	ret
main:
	inc	1
	pushr	25.000000
	pushr	4.000000
	savebp
	inc	1
	call	f2
	dec	1
	rstrbp
	dec	1
	output
	halt
	end
