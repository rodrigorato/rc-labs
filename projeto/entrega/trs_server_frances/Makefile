# TRS SERVER MAKEFILE

trs-server: trs_server.cpp 
	@echo "[TRS] Compiling with warnings and with -std=c++11 flag." 
	g++ -o TRS -O3 -ansi -Wall -std=c++11 trs_server.cpp -lm
	@echo "[TRS] Compiled successfully to ./TRS"		

clean:
	@echo "[TRS] Cleaning output files."
	rm -rf TRS
	@echo "[TRS] Cleaned output files successfully."

clean-imgs:
	rm totranslate*