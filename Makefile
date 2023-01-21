ALL_BINARIES = build/bin/fmindex_construct  build/bin/fmindex_search  build/bin/naive_search  build/bin/suffixarray_search
ALL_SRCS = src/fmindex_construct.cpp src/fmindex_search.cpp src/naive_search.cpp src/suffixarray_search.cpp
all: experiment_results.csv

$(ALL_BINARIES): build $(ALL_SRCS) 
	cd build && make -j 4

fm.index: build/bin/fmindex_construct data/hg38_partial.fasta.gz
	build/bin/fmindex_construct --reference data/hg38_partial.fasta.gz --index fm.index

experiment_results.txt: run_experiments.sh fm.index $(ALL_BINARIES)
	./run_experiments.sh > experiment_results.txt

experiment_results.csv: experiment_results.txt
	python convert_to_csv.py experiment_results.txt

compile: $(ALL_BINARIES)

build:
	mkdir build
	cd build && cmake ..

clean:
	rm -rf build
