# TCS SERVER MAKEFILE

tcs-server: tcs_server.cpp 
	@echo "[TCS] Compiling with warnings and with -std=c++11 flag." 
	g++ -o TCS -O3 -ansi -Wall -std=c++11 tcs_server.cpp -lm
	@echo "[TCS] Compiled successfully to ./TCS"		

clean:
	@echo "[TCS] Cleaning output files."
	rm -rf TCS
	@echo "[TCS] Cleaned output files successfully."
