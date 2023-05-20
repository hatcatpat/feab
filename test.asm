JUMP start

x: 0 y: 0

start:
	SET y x
	ADD y 48
	CHAR y
	INC x
	CMP x 10
	ELSE @ahead
		SET x 0
		CHAR 10
	@ahead:
		WAIT
		JUMP start
