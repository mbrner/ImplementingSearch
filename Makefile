ALL_BINARIES = build/bin/fmindex_construct  build/bin/fmindex_search  build/bin/naive_search  build/bin/suffixarray_search
ALL_SRCS = src/fmindex_construct.cpp src/fmindex_search.cpp src/naive_search.cpp src/suffixarray_search.cpp

all: experiment_results_assignment1.csv experiment_results_assignment2.csv

# Assignment 2
assignment_2: experiment_results_assignment2_part1.csv experiment_results_assignment2_part2.csv

## Part 2
experiment_results_assignment2_part2.txt: run_experiments_assignment2_part2.sh fm_hg38_full.index $(ALL_BINARIES)
	./run_experiments_assignment2_part2.sh > experiment_results_assignment2_part2.txt

experiment_results_assignment2_part2.csv: experiment_results_assignment2_part2.txt
	python convert_to_csv.py experiment_results_assignment2_part2.txt

fm_hg38_full.index: build/bin/fmindex_construct grch38/GCF_000001405.26_GRCh38_genomic.fna.gz
	build/bin/fmindex_construct --reference grch38/GCF_000001405.26_GRCh38_genomic.fna.gz --index fm_hg38_full.index

grch38/GCF_000001405.26_GRCh38_genomic.fna.gz
	rsync --copy-links --times --verbose rsync://ftp.ncbi.nlm.nih.gov/genomes/all/GCF/000/001/405/GCF_000001405.26_GRCh38/GCF_000001405.26_GRCh38_genomic.fna.gz grch38/

## Part 1

experiment_results_assignment2_part1.csv: experiment_results_assignment2_part1.txt
	python convert_to_csv.py experiment_results_assignment2_part1.txt
experiment_results_assignment2_part1.txt: run_experiments_assignment2_part1.sh fm_hg38.index $(ALL_BINARIES)
	./run_experiments_assignment2_part1.sh > experiment_results_assignment2_part1.txt

# Assignment 1
assignment_1: experiment_results_assignment1.csv

experiment_results_assignment1.csv: experiment_results_assignment1.txt
	python convert_to_csv.py experiment_results_assignment1.txt
experiment_results_assignment1.txt: run_experiments_assignment1.sh fm_hg38.index $(ALL_BINARIES)
	./run_experiments_assignment1.sh > experiment_results_assignment1.txt
fm_hg38.index: build/bin/fmindex_construct data/hg38_partial.fasta.gz
	build/bin/fmindex_construct --reference data/hg38_partial.fasta.gz --index fm_hg38.index

# General
$(ALL_BINARIES): build $(ALL_SRCS) 
	cd build && make -j 4

build:
	mkdir build
	cd build && cmake ..

clean:
	rm -rf build
