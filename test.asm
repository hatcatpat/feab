# A 65 #	# B 66 #	# C 67 #	# D 68 #
# E 69 #	# F 70 #	# G 71 #	# H 72 #

JUMP start

p: 0 p_: 0
char: 0
string: C A B B A G E 0

print_string:
	@loop:
		GET char p
		CHAR char
		INC p_
		CMP char 0
		ELSE @loop
	CHAR 10
	RET

start:
	REF p string
	CALL print_string
	QUIT
