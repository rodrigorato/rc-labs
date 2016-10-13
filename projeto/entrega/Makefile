# RC-TRANSLATE MAKEFILE

main: TRS-SUB-1 TRS-SUB-2 TCS-SUB USER-SUB 
	@echo ""
	@echo ""
	@echo "[MAIN-MAKE] Complete make done successfully"


TRS-SUB-1:
	@echo ""
	@echo "[MAIN-TRS-SUB] Started to make TRS-1."
	@echo ""
	cd ./trs_server_ingles/ && \
	make && \
	cd ../	
	@echo ""
	@echo "[MAIN-TRS-SUB] Compiled TRS-1 in it's directory successfully."
	@echo ""

TRS-SUB-2:
	@echo ""
	@echo "[MAIN-TRS-SUB] Started to make TRS-2."
	@echo ""
	cd ./trs_server_frances/ && \
	make
	@echo ""
	@echo "[MAIN-TRS-SUB] Compiled TRS-2 in it's directory successfully."
	@echo ""


TCS-SUB:
	@echo ""
	@echo "[MAIN-TCS-SUB] Started to make TCS."
	@echo ""
	cd ./tcs_server/ && \
	make
	@echo ""
	@echo "[MAIN-TCS-SUB] Brought TCS to main directory successfully."
	@echo ""

USER-SUB:
	@echo ""
	@echo "[MAIN-USER-SUB] Started to make user."
	@echo ""
	cd ./user_app/ && \
	make
	@echo ""
	@echo "[MAIN-USER-SUB] Brought user to main directory successfully."
	@echo ""


clean:
	@echo ""
	@echo "[MAIN-CLEAN] Started to clean everything."
	@echo "[MAIN-CLEAN] Cleaning subs."
	@echo ""

	cd ./trs_server_ingles/ && \
	make clean && \
	cd ../	

	cd ./trs_server_frances/ && \
	make clean && \
	cd ../

	cd ./tcs_server/ && \
	make clean && \
	cd ../

	cd ./user_app/ && \
	make clean && \
	cd ../
	
	@echo ""
	@echo "[MAIN-CLEAN] Done cleaning successfully."
	@echo ""
	
clean-imgs:
	@echo ""
	@echo "[MAIN-CLEAN] Started to clean images."
	@echo "[MAIN-CLEAN] Cleaning subs."
	@echo ""

	cd ./trs_server_ingles/ && \
	make clean-imgs && \
	cd ../	

	cd ./trs_server_frances/ && \
	make clean-imgs && \
	cd ../

	cd ./user_app/ && \
	make clean-imgs && \
	cd ../
	
	@echo ""
	@echo "[MAIN-CLEAN] Done cleaning successfully."
	@echo ""