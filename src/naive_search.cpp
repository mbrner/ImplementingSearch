#include <sstream>

#include <filesystem>

#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/core/debug_stream/debug_stream_type.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/search/fm_index/fm_index.hpp>
#include <seqan3/search/search.hpp>

// prints out all occurences of query inside of ref
unsigned int findOccurences(std::vector<seqan3::dna5> const& ref, std::vector<seqan3::dna5> const& query) {
    unsigned int count = 0;
    for (size_t i = 0; i < ref.size() - query.size() + 1; i++) {
        bool match = true;
        for (size_t j = 0; j < query.size(); j++) {
            if (ref[i + j] != query[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            count++;
        }
    }
    return count;
}


int main(int argc, char const* const* argv) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::nanoseconds;
    seqan3::argument_parser parser{"naive_search", argc, argv, seqan3::update_notifications::off};

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
    std::vector<std::vector<seqan3::dna5>> reference;
    for (auto& record : reference_stream) {
        reference.push_back(record.sequence());
    }

    // read query into memory
    std::vector<std::vector<seqan3::dna5>> queries;
    for (auto& record : query_stream) {
        queries.push_back(record.sequence());
    }

    //!TODO !CHANGEME here adjust the number of searches
    queries.resize(query_length); // will reduce the amount of searches

    //! search for all occurences of queries inside of reference
    auto t1 = high_resolution_clock::now();
    unsigned int total_count = 0;
    for (auto& r : reference) {
        for (auto& q : queries) {
            total_count += findOccurences(r, q);
        }
    }
    auto t2 = high_resolution_clock::now();
    auto t_diff = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
    std::cout << ">>>>>" << std::endl;
    std::cout << "> Method: Naive Search" << std::endl;
    std::cout << "> Query File: " << query_file << std::endl;
    std::cout << "> Query Limit: " << query_length << std::endl;
    std::cout << "> Total Count: " << total_count << std::endl;
    std::cout << "> Search duration: " << t_diff.count() << " ns\n";
    std::cout << "<<<<" << std::endl;

    return 0;
}
