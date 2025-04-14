#a list of custom asset flags to use when converting to Libradgon formats

filesystem/font/en/RifficFree-Bold.font64: MKFONT_FLAGS+= --char-spacing 2 --outline 2 --size 16  
filesystem/font/en/Aller_Rg.font64: 		MKFONT_FLAGS+= --outline 2 --size 16  
filesystem/font/en/Vera.font64: 			MKFONT_FLAGS+= --outline 2 --size 16  
filesystem/font/en/Halogen.font64: 		MKFONT_FLAGS+= --outline 2 --size 18  
filesystem/font/en/n1.font64: 				MKFONT_FLAGS+= --size 24  
filesystem/font/en/m1.font64: 				MKFONT_FLAGS+= --size 26  
filesystem/font/en/s1.font64: 				MKFONT_FLAGS+= --size 32  
filesystem/font/en/y1.font64: 				MKFONT_FLAGS+= --size 30  
filesystem/font/en/verily-serif-mono.font64: MKFONT_FLAGS+= --size 30  



filesystem/font/ru/ru_Ramona_title.font64: 			MKFONT_FLAGS+= --char-spacing 2 --outline 2 --size 16   --range 20-7F --range 0400-052F
filesystem/font/ru/ru_Nunito_main.font64: 			MKFONT_FLAGS+= --outline 2 --size 14   --range 20-7F --range 0400-052F
filesystem/font/ru/ru_Maki_poems.font64: 			MKFONT_FLAGS+= --outline 2 --size 16   --range 20-7F --range 0400-052F
filesystem/font/ru/ru_Hero_n.font64: 				MKFONT_FLAGS+= --size 18   --range 20-7F --range 0400-052F
filesystem/font/ru/ru_Rosarium_m.font64: 			MKFONT_FLAGS+= --size 26   --range 20-7F --range 0400-052F
filesystem/font/ru/ru_Pershotravneva55_s.font64: 	MKFONT_FLAGS+= --size 24   --range 20-7F --range 0400-052F
filesystem/font/ru/ru_Czizh_y.font64: 				MKFONT_FLAGS+= --size 28   --range 20-7F --range 0400-052F
filesystem/font/ru/ru_Naga.font64: 					MKFONT_FLAGS+= --size 30   --range 20-7F --range 0400-052F

filesystem/bgm/d.wav64: 				AUDIOCONV_FLAGS += --wav-compress 1,bits=3 --wav-resample 24000