JUMP start

t: 0
t2: 0
k: 0 0
x: 0 y: 0

ptr: 0 ptr_: 0
char: 0
string: W E L C O M E SPACE T O SPACE F E A B ! ? ! ? 0

print_string:
	@loop:
		DEREF char ptr
		CHAR char
		INC ptr_
		CMP char 0
		ELSE @loop
	CHAR 10
	RET

# CMPLEFT 	GET k KEYS AND k LEFT CMP k 0 #
# CMPRIGHT 	GET k KEYS AND k RIGHT CMP k 0 #
# CMPUP	 	GET k KEYS AND k UP CMP k 0 #
# CMPDOWN	GET k KEYS AND k DOWN CMP k 0 #

start:
	REF ptr string
	CALL print_string

	SET PALETTE_1 %110000
	SET PALETTE_2 %001100
	SET PALETTE_3 %000011
	OR FLAGS 4 ; load sprite
	SET SPRITES_ROW_0 ff ; enable sprites

	SET x 32
	SET y 32
	GET SPRITE_0_X x
	GET SPRITE_0_Y y
	SET 10 %10000010
	SET 11 %11101011
	SET 12 %10010110
	SET 13 %10101010

	@loop:
		GET PALETTE_0 t2
		MOD t 8
		CMP t 0
		ELSE @1 INC t2
		@1:
			CMPLEFT IF @2 DEC x
		@2:
			CMPRIGHT IF @3 INC x
		@3:
			CMPUP IF @4 DEC y
		@4:
			CMPDOWN IF @done INC y
		@done:
			GET SPRITE_0_X x
			GET SPRITE_0_Y y
			INC t
			WAIT
			JUMP @loop
