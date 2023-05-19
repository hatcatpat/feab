JUMP start

x: 32 y: 32
k: 0

dec_x:
	CMP x 0
	ELSE @end
		SET x 70
	@end:
		DEC x
		RET
dec_y:
	CMP y 0
	ELSE @end
		SET y 70
	@end:
		DEC y
		RET
inc_x:
	CMP x 68
	ELSE @end
		SET x 0
	@end:
		INC x
		RET
inc_y:
	CMP y 68
	ELSE @end
		SET y 0
	@end:
		INC y
		RET

update:
	SET 8 x ; sprite 0, x
	SET 9 y ; sprite 0, y
	RET

start:
	SET 2 %000000 ; color 0
	SET 3 %110000 ; color 1
	SET 4 %001100 ; color 2
	SET 5 %000011 ; color 3
	OR 0 4 ; load sprite
	SET 6 ff ; enable sprites

	SET 8 32 ; sprite 0, x
	SET 9 32 ; sprite 0, y
	SET 10 %10000010 ; sprite 0, row 0
	SET 11 %11101011 ; sprite 0, row 1
	SET 12 %10010110 ; sprite 0, row 2
	SET 13 %10101010 ; sprite 0, row 3

	; F
	SET 14 24 ; sprite 1, x
	SET 15 24 ; sprite 1, y
	SET 16 %10100000 ; sprite 1, row 0
	SET 17 %10000000 ; sprite 1, row 1
	SET 18 %10100000 ; sprite 1, row 2
	SET 19 %10000000 ; sprite 1, row 3

	; E
	SET 20 27 ; sprite 2, x
	SET 21 24 ; sprite 2, y
	SET 22 %10101000 ; sprite 2, row 0
	SET 23 %10101000 ; sprite 2, row 1
	SET 24 %10000000 ; sprite 2, row 2
	SET 25 %10101000 ; sprite 2, row 3

	; A
	SET 26 31 ; sprite 3, x
	SET 27 24 ; sprite 3, y
	SET 28 %00000000 ; sprite 3, row 0
	SET 29 %10101000 ; sprite 3, row 1
	SET 30 %10001000 ; sprite 3, row 2
	SET 31 %10101010 ; sprite 3, row 3

	; B
	SET 32 36 ; sprite 4, x
	SET 33 24 ; sprite 4, y
	SET 34 %10000000 ; sprite 4, row 0
	SET 35 %10101000 ; sprite 4, row 1
	SET 36 %10001000 ; sprite 4, row 2
	SET 37 %10101000 ; sprite 4, row 3

loop:
	@left:
		SET k &1
		AND k 1
		CMP k 0
		IF @right
		CALL dec_x
	@right:
		SET k &1
		AND k 2
		CMP k 0
		IF @up
		CALL inc_x
	@up:
		SET k &1
		AND k 4
		CMP k 0
		IF @down
		CALL dec_y
	@down:
		SET k &1
		AND k 8
		CMP k 0
		IF @done
		CALL inc_y
	@done:
		CALL update
		WAIT
		JUMP loop
