JUMP start

t: 0
k: 0
x: 0 y: 0
moving: 0

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

	SET PALETTE_1 %110000 ; 01 R
	SET PALETTE_2 %001100 ; 10 G
	SET PALETTE_3 %000011 ; 11 B

	GET SPRITE_0_S s

	SET x 32
	SET y 32
	GET SPRITE_0_X x
	GET SPRITE_0_Y y
	SET SPRITE_0_ROW_0 %10000010
	SET SPRITE_0_ROW_1 %11101011
	SET SPRITE_0_ROW_2 %10010110
	SET SPRITE_0_ROW_3 %10101010

	SET SPRITE_1_ROW_0 %00001010
	SET SPRITE_1_ROW_1 %00101110
	SET SPRITE_1_ROW_2 %00100101
	SET SPRITE_1_ROW_3 %00101010

	SET SPRITE_2_ROW_0 %10100000
	SET SPRITE_2_ROW_1 %10111000
	SET SPRITE_2_ROW_2 %01011000
	SET SPRITE_2_ROW_3 %10101000

	OR FLAGS 4 ; load sprite

	@loop:
		SET moving 0
		@left:
			CMPLEFT IF @right
				DEC x
				SET moving 1
				SET SPRITE_0_S 2
		@right:
			CMPRIGHT IF @up
				INC x
				SET moving 1
				SET SPRITE_0_S 1
		@up:
			CMPUP IF @down
				DEC y
				SET moving 1
		@down:
			CMPDOWN IF @move
				INC y
				SET moving 1
		@move:
			CMP moving 0
			ELSE @done
				SET t 0
				SET SPRITE_0_S 0
		@done:
			GET SPRITE_0_X x
			GET SPRITE_0_Y y
			WAIT
			JUMP @loop
