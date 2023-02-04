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
    seqan3::argument_parser parser{"fmindex_search", argc, argv, seqan3::update_notifications::off};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";

    auto index_path = std::filesystem::path{};
    parser.add_option(index_path, '\0', "index", "path to the query file");

    auto query_file = std::filesystem::path{};
    parser.add_option(query_file, '\0', "query", "path to the query file");

    unsigned long int query_length = 100;
    parser.add_option(query_length, query_length, "query-lim", "query limit");

    unsigned char max_error_total = 0;
    parser.add_option(max_error_total, max_error_total, "error-total", "number of total errors");

    try {
         parser.parse();
    } catch (seqan3::argument_parser_error const& ext) {
        seqan3::debug_stream << "Parsing error. " << ext.what() << "\n";
        return EXIT_FAILURE;
    }

    // loading our files
    auto query_stream = seqan3::sequence_file_input{query_file};

    // read query into memory
    std::vector<std::vector<seqan3::dna5>> queries;
    for (auto& record : query_stream) {
        queries.push_back(record.sequence());
        if (queries.size() == query_length) break;
    }

    size_t remaining = query_length - queries.size();
    for (size_t i = 0; i < remaining; ++i) {
        queries.push_back(queries[i]);
    }

    // loading fm-index into memory
    using Index = decltype(seqan3::fm_index{std::vector<std::vector<seqan3::dna5>>{}}); // Some hack
    Index index; // construct fm-index
    {
        seqan3::debug_stream << "Loading 2FM-Index ... " << std::flush;
        std::ifstream is{index_path, std::ios::binary};
        cereal::BinaryInputArchive iarchive{is};
        iarchive(index);
        seqan3::debug_stream << "done\n";
    }
    //!TODO here adjust the number of searches

    auto t1 = high_resolution_clock::now();
    seqan3::configuration const cfg = seqan3::search_cfg::max_error_total{seqan3::search_cfg::error_count{max_error_total}};
    auto results = search(queries, index, cfg);
    unsigned int total_count = 0;
    for (auto && result : results) {   
        total_count++;
    }
    auto t2 = high_resolution_clock::now();  
    auto t_diff = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
    std::cout << ">>>>>" << std::endl;
    std::cout << "> Method: FM-Index" << std::endl;
    std::cout << "> Query File: " << query_file << std::endl;
    std::cout << "> Query Limit: " << query_length << std::endl;
    unsigned int max_error_total_int = (unsigned int) max_error_total;
    std::cout << "> Excepted Errors: " << max_error_total_int << std::endl;
    std::cout << "> Total Count: " << total_count << std::endl;
    std::cout << "> Search duration: " << t_diff.count() << " ns\n";
    std::cout << "<<<<" << std::endl;
    return 0;
}
