cd ..
"Debug/RoleSim_Jaccard.exe" -f triple -g "data-sets/iswc/univ3-less/ls-aw/no-nlb/triples.txt" -nlb "data-sets/iswc/univ3-less/aw/no-nlb/noise-props.txt"
"Debug/RoleSim_Jaccard.exe" -f flat --auto_weights --literal_sims -g "data-sets/iswc/univ3-less/ls-aw/no-nlb/triples.txt_flat.txt" -gm "data-sets/iswc/univ3-less/ls-aw/no-nlb/triples.txt_map.txt" -nw "data-sets/noise-words.txt"  -cmst "0.625"
