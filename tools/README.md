python Simulation_LDPC_ProbLogDomain.py --log=DEBUG 
python Simulation_LDPC_ProbLogDomain.py --config=configs/ldpc_7_3_default.ini
python Simulation_LDPC_ProbLogDomain.py --config=configs/ldpc_7_3_default.ini --outmat=result.mat

##config example
see configs/ldpc_7_3_default.ini 

##Compare BER performance build reference plots

python Simulation_LDPC_ProbLogDomain.py --config=configs/CCSDS_ldpc_n32_k16.ini --outmat=result.mat

../lib/aff3ct/build_exe/bin/aff3ct-simon-0.0.1 --sim-type BFER --sim-cde-type LDPC -m 0.0 -M 4.01 -s 1.0 --mnt-max-fe 100 --src-type AZCW -K 16 -N 32 --enc-type AZCW --chn-type AWGN --chn-implem FAST --dec-type BP_FLOODING --dec-implem SPA --dec-h-path ../codes/ldpc/CCSDS_ldpc_n32_k16.alist --dec-ite 40
