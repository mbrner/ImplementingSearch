#!/bin/bash
query_limits=("5" "20" "50" "100" "200" "400")
query_files=("data/illumina_reads_40.fasta.gz" "data/illumina_reads_60.fasta.gz" "data/illumina_reads_80.fasta.gz" "data/illumina_reads_100.fasta.gz")
repeats=1
for lim in ${query_limits[@]}; do
    for query in ${query_files[@]}; do
        for i in $(seq $repeats); do
            /usr/bin/time -v ./build/bin/naive_search --reference data/hg38_partial.fasta.gz --query ${query} --query-lim ${lim} 2>&1
            /usr/bin/time -v ./build/bin/suffixarray_search --reference data/hg38_partial.fasta.gz --query ${query} --query-lim ${lim} 2>&1
            /usr/bin/time -v ./build/bin/fmindex_search --index fm.index --query ${query} --query-lim ${lim} 2>&1
        done
    done
done
