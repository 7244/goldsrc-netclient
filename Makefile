OUTPUT = a.exe

LINK = -lev -lbz2 -lcrypto
INCLUDE = 
CS = -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare

debug:
	clang $(CS) -g main.c -o $(OUTPUT) $(INCLUDE) $(LINK)

release:
	clang $(CS) -s -O3 main.c -o $(OUTPUT) $(INCLUDE) $(LINK)

clean:
	rm $(OUTPUT)
