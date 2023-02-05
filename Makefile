ALL_BINARIES = build/bin/fmindex_construct  build/bin/fmindex_search  build/bin/naive_search  build/bin/suffixarray_search  build/bin/fmindex_pigeon_search
ALL_SRCS = src/fmindex_construct.cpp src/fmindex_search.cpp src/naive_search.cpp src/suffixarray_search.cpp src/fmindex_pigeon_search.cpp
PYTHON_VERSION := $(shell command -v python)
ifeq ($(PYTHON_VERSION),)
    PYTHON_VERSION := $(shell command -v python3)
endif

all: results/experiment_results_assignment1.csv results/experiment_results_assignment2_part1.csv results/experiment_results_assignment2_part2.csv results/experiment_results_assignment3.csv


# Assigment 3
assignment3: results/experiment_results_assignment3.csv

results/experiment_results_assignment3.txt: run_experiments_assignment3.sh results/fm_hg38_full.index $(ALL_BINARIES)
	./run_experiments_assignment3.sh > results/experiment_results_assignment3.txt

results/experiment_results_assignment3.csv: results/experiment_results_assignment3.txt
	$(PYTHON_VERSION) convert_to_csv.py results/experiment_results_assignment3.txt



# Assignment 2
assignment2: results/experiment_results_assignment2_part1.csv results/experiment_results_assignment2_part2.csv

## Part 2
results/experiment_results_assignment2_part2.txt: run_experiments_assignment2_part2.sh results/fm_hg38_full.index $(ALL_BINARIES)
	./run_experiments_assignment2_part2.sh > results/experiment_results_assignment2_part2.txt

results/experiment_results_assignment2_part2.csv: results/experiment_results_assignment2_part2.txt
	$(PYTHON_VERSION) convert_to_csv.py results/experiment_results_assignment2_part2.txt

## Part 1

results/experiment_results_assignment2_part1.csv: results/experiment_results_assignment2_part1.txt
	$(PYTHON_VERSION) convert_to_csv.py results/experiment_results_assignment2_part1.txt
results/experiment_results_assignment2_part1.txt: run_experiments_assignment2_part1.sh results/fm_hg38.index $(ALL_BINARIES)
	./run_experiments_assignment2_part1.sh > results/experiment_results_assignment2_part1.txt

# Assignment 1
assignment1: results/experiment_results_assignment1.csv

results/experiment_results_assignment1.csv: results/experiment_results_assignment1.txt
	$(PYTHON_VERSION) convert_to_csv.py results/experiment_results_assignment1.txt
results/experiment_results_assignment1.txt: run_experiments_assignment1.sh results/fm_hg38.index $(ALL_BINARIES)
	./run_experiments_assignment1.sh > results/experiment_results_assignment1.txt


### Create FM-Indices
results/fm_hg38.index: build/bin/fmindex_construct data/hg38_partial.fasta.gz
	build/bin/fmindex_construct --reference data/hg38_partial.fasta.gz --index results/fm_hg38.index

results/fm_hg38_full.index: build/bin/fmindex_construct grch38/GCF_000001405.26_GRCh38_genomic.fna.gz
	build/bin/fmindex_construct --reference grch38/GCF_000001405.26_GRCh38_genomic.fna.gz --index results/fm_hg38_full.index

grch38/GCF_000001405.26_GRCh38_genomic.fna.gz:
	rsync --copy-links --times --verbose rsync://ftp.ncbi.nlm.nih.gov/genomes/all/GCF/000/001/405/GCF_000001405.26_GRCh38/GCF_000001405.26_GRCh38_genomic.fna.gz grch38/

# General
$(ALL_BINARIES): build $(ALL_SRCS) results
	cd build && make

results:
	mkdir results

build:
	mkdir build
	cd build && cmake .. "-DCMAKE_CXX_FLAGS=-Wl,-rpath -Wl,/import/GCC/12.2.1/lib64" -DCMAKE_CXX_COMPILER=/import/GCC/12.2.1/bin/g++

clean:
	rm -rf build/
	rm -rf results/
