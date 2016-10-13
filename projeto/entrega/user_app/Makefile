# USER MAKEFILE

user: user.cpp 
	@echo "[USER] Compiling with warnings and with -std=c++11 flag." 
	g++ -o user -O3 -ansi -Wall -std=c++11 user.cpp -lm
	@echo "[USER] Compiled successfully to ./user"		

clean:
	@echo "[USER] Cleaning output files."
	rm -rf user
	@echo "[USER] Cleaned output files successfully."

clean-imgs:
	rm translated*