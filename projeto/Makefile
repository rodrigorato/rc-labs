# RC-TRANSLATE MAKEFILE

main: TRS-SUB TCS-SUB USER-SUB 
	@echo ""
	@echo ""
	@echo "[MAIN-MAKE] Complete make done successfully"


TRS-SUB:
	@echo ""
	@echo "[MAIN-TRS-SUB] Started to make TRS."
	@echo ""
	cd ./trs_server/ && \
	make && \
	cp ./TRS ../TRS && \
	cp ./*.txt ../ && \
	cd ../	
	@echo ""
	@echo "[MAIN-TRS-SUB] Brought TRS to main directory successfully."
	@echo ""

TCS-SUB:
	@echo ""
	@echo "[MAIN-TCS-SUB] Started to make TCS."
	@echo ""
	cd ./tcs_server/ && \
	make && \
	cp ./TCS ../TCS && \
	cd ../	
	@echo ""
	@echo "[MAIN-TCS-SUB] Brought TCS to main directory successfully."
	@echo ""

USER-SUB:
	@echo ""
	@echo "[MAIN-USER-SUB] Started to make user."
	@echo ""
	cd ./user_app/ && \
	make && \
	cp ./user ../user && \
	cd ../	
	@echo ""
	@echo "[MAIN-USER-SUB] Brought user to main directory successfully."
	@echo ""


clean:
	@echo ""
	@echo "[MAIN-CLEAN] Started to clean everything."
	@echo "[MAIN-CLEAN] Cleaning subs."
	@echo ""

	cd ./trs_server/ && \
	make clean && \
	cd ../
	rm *.txt	

	cd ./tcs_server/ && \
	make clean && \
	cd ../

	cd ./user_app/ && \
	make clean && \
	cd ../

	@echo ""
	@echo "[MAIN-CLEAN] Done cleaning subs, cleaning main."
	rm -f TCS TRS user
	@echo "[MAIN-CLEAN] Done cleaning successfully."
	@echo ""
	
