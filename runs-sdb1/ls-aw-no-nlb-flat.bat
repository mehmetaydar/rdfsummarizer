cd ..
"Debug/RoleSim_Jaccard.exe" -f triple -g "data-sets/iswc/sdb/ls-aw/show-200.txt"
"Debug/RoleSim_Jaccard.exe" -f flat --auto_weights --literal_sims -g "data-sets/iswc/sdb/ls-aw/show-200.txt_flat.txt" -gm "data-sets/iswc/sdb/ls-aw/show-200.txt_map.txt" -cv "data-sets/iswc/sdb/show-200_verification.txt" -nw "data-sets/noise-words.txt" -cmst "0.5"
