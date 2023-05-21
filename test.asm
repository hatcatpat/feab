JUMP start

t: 0
t2: 0
k: 0 0

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

start:
	REF ptr string
	CALL print_string

	SET PALETTE_1 %110000
	SET PALETTE_2 %001100
	SET PALETTE_3 %000011
	OR FLAGS 4 ; load sprite
	SET SPRITES_ROW_0 ff ; enable sprites

	SET SPRITE_0_X 32
	SET SPRITE_0_Y 32
	SET 10 %10000010
	SET 11 %11101011
	SET 12 %10010110
	SET 13 %10101010

	@loop:
		GET PALETTE_0 t2
		MOD t 8
		CMP t 0
		ELSE @a
			INC t2
		@a:
			GET k KEYS
			AND k LEFT
			CMP k 0
			IF @b
				QUIT
			@b:
				INC t
				WAIT
				JUMP @loop
