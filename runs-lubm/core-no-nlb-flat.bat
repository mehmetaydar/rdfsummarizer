cd ..
"Debug/RoleSim_Jaccard.exe" -f triple -g "data-sets/iswc/lubm/core/no-nlb/university0_test.txt"
"Debug/RoleSim_Jaccard.exe" -f flat -g "data-sets/iswc/lubm/core/no-nlb/university0_test.txt_flat.txt" -gm "data-sets/iswc/lubm/core/no-nlb/university0_test.txt_map.txt" -nw "data-sets/noise-words.txt" -cv "data-sets/iswc/lubm/university0_test_verification-mod.txt" -cmst "0.3"
