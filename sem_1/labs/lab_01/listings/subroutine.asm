020A:07B9			sub_3		proc	near
; Сохранение значений регистров ds, ax:
020A:07B9  1E					push	ds
020A:07BA  50					push	ax
; Загрузка сегментного регистра ds:
020A:07BB  B8 0040				mov	ax,40h
020A:07BE  8E D8				mov	ds,ax
; Сохранение младшего байта регистра FLAGS в AH:
020A:07C0  9F					lahf				                ; Load ah from flags
; Проверка DF и старшего бита IOPL по адресу 0040:0314h:
020A:07C1  F7 06 0314 2400		test	word ptr ds:[314h],2400h	; (0040:0314=3200h)
020A:07C7  75 0C				jnz	loc_8			                ; Jump if not zero
; Установка 9 бита в 0 - сброс IF (запрет прерываний):
020A:07C9  F0> 81 26 0314 FDFF	lock and word ptr ds:[314h],0FDFFh	; (0040:0314=3200h)
020A:07D0			loc_7:
; Сохранение регистра AH в младший байт FLAGS:
020A:07D0  9E					sahf				                ; Store ah into flags
; Восстановление значений регистров ds, ax:
020A:07D1  58					pop	ax
020A:07D2  1F					pop	ds
020A:07D3  EB 03				jmp	short loc_9		                ; (07D8)
020A:07D5			loc_8:
; Сброс флага IF:
020A:07D5  FA					cli				                    ; Disable interrupts
020A:07D6  EB F8				jmp	short loc_7		                ; (07D0)
020A:07D8			loc_9:
; Возврат из подпрограммы:
020A:07D8  C3					retn
				    sub_3		endp
