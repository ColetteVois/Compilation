	jp	main
f2:
	libp	-4
	libp	-3
	copy	1
	libp	-2
	copy	1
	add
	move	1
	ret
main:
	inc	1
	pushr	25.000000
	pushr	4.000000
	savebp
	inc	2
	call	f2
	dec	2
	rstrbp
	dec	2
	output
	halt
	end
