#include <divsufsort.h>
#include <sstream>
#include <filesystem>

#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/search/fm_index/fm_index.hpp>
#include <seqan3/search/search.hpp>

int main(int argc, char const* const* argv) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::nanoseconds;
    seqan3::argument_parser parser{"suffixarray_search", argc, argv, seqan3::update_notifications::off};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";

    auto reference_file = std::filesystem::path{};
    parser.add_option(reference_file, '\0', "reference", "path to the reference file");

    auto query_file = std::filesystem::path{};
    parser.add_option(query_file, '\0', "query", "path to the query file");

    unsigned long int query_length = 100;
    parser.add_option(query_length, query_length, "query-lim", "query limit");

    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }

    // loading our files
    auto reference_stream = seqan3::sequence_file_input{reference_file};
    auto query_stream     = seqan3::sequence_file_input{query_file};

    // read reference into memory
    // Attention: we are concatenating all sequences into one big combined sequence
    //            this is done to simplify the implementation of suffix_arrays
    std::vector<seqan3::dna5> reference;
    for (auto& record : reference_stream) {
        auto r = record.sequence();
        reference.insert(reference.end(), r.begin(), r.end());
    }

    // read query into memory
    std::vector<std::vector<seqan3::dna5>> queries;
    for (auto& record : query_stream) {
        queries.push_back(record.sequence());
    }

    //!TODO here adjust the number of searches
    queries.resize(query_length); // will reduce the amount of searches

    // Array that should hold the future suffix array
    std::vector<saidx_t> suffixarray;
    suffixarray.resize(reference.size()); // resizing the array, so it can hold the complete SA

    // Implement suffix array sort
    sauchar_t const* str = reinterpret_cast<sauchar_t const*>(reference.data());
    divsufsort(str, suffixarray.data(), reference.size());
    unsigned int total_count = 0;
    auto t1 = high_resolution_clock::now();
    for (auto& q : queries) {
        // apply binary search and find q  in reference using binary search on `suffixarray`
        // using the naive approach
        unsigned long int left = 0;
        unsigned long int right = reference.size() - 1;
        while (left <= right) {
            auto mid = (left + right) / 2;
            bool equal = true;
            for (size_t i = 0; i < q.size(); ++i) {
                if (q[i] != reference[suffixarray[mid] + i]) {
                    equal = false;
                    break;
                }
            }
            if (equal) {
                total_count++;
                // q found at position suffixarray[mid]
                // seqan3::debug_stream << "Query: " << q << " occurs at position: suffixarray[mid] & mid=" << mid << std::endl;
                break;
            } else {
                bool less = true;
                for (size_t i = 0; i < q.size(); ++i) {
                    if (q[i] != reference[suffixarray[mid] + i]) {
                        if (q[i] > reference[suffixarray[mid] + i]) {
                            less = false;
                        }
                        break;
                    }
                }
                if (less) {
                    right = mid - 1;
                } else {
                    left = mid + 1;
                }
            }
        }
    }
    auto t2 = high_resolution_clock::now();
    auto t_diff = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
    std::cout << ">>>>>" << std::endl;
    std::cout << "> Method: Suffix-Array" << std::endl;
    std::cout << "> Query File: " << query_file << std::endl;
    std::cout << "> Query Limit: " << query_length << std::endl;
    std::cout << "> Total Count: " << total_count << std::endl;
    std::cout << "> Search duration: " << t_diff.count() << " ns\n";
    std::cout << "<<<<" << std::endl;
    return 0;
}
