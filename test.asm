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
