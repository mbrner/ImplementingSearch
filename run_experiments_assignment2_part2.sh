#!/bin/bash
query_limits=("1000")
total_errors=("0" "1" "2" "3")
query_files=("data/illumina_reads_100.fasta.gz")
repeats=1
for lim in ${query_limits[@]}; do
    for query in ${query_files[@]}; do
        for accepted_errors in ${total_errors[@]}; do
            for i in $(seq $repeats); do
                /usr/bin/time -v ./build/bin/fmindex_search --index fm_hg38_full.index --query ${query} --query-lim ${lim} --error-total=${accepted_errors} 2>&1
            done
        done
    done
done