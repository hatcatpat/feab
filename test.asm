# A 65 #	# B 66 #	# C 67 #	# D 68 #
# E 69 #	# F 70 #	# G 71 #	# H 72 #

JUMP start

ptr: 0 ptr_: 0
char: 0
string: C A B B A G E 0
string2: B A D G E 0

print_string:
	@loop:
		DEREF char ptr
		CHAR char
		INC ptr_
		CMP char 0
		ELSE @loop
	CHAR 10
	RET

start:
	REF ptr string
	CALL print_string
	REF ptr string2
	CALL print_string
	QUIT
