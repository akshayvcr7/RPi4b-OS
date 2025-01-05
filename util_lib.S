.globl get_el
get_el:
	mrs x0, CurrentEL
	lsr x0, x0, #2
	ret

.globl get_daif
get_daif:
	mrs x0, daif
	ret