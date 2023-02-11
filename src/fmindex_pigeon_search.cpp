#include <sstream>
#include <stdexcept>

#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>
#include <seqan3/core/debug_stream.hpp>
#include <seqan3/io/sequence_file/all.hpp>
#include <seqan3/search/fm_index/fm_index.hpp>
#include <seqan3/search/search.hpp>


size_t count_errors_with_indels(std::vector<seqan3::dna5> const & str1, std::vector<seqan3::dna5> const & str2)
{
    size_t m = str1.size(), n = str2.size();
    std::vector<std::vector<size_t>> dp(m + 1, std::vector<size_t>(n + 1));

    for (size_t i = 0; i <= m; ++i)
        dp[i][0] = i;
    for (size_t j = 0; j <= n; ++j)
        dp[0][j] = j;

    for (size_t i = 1; i <= m; ++i)
    {
        for (size_t j = 1; j <= n; ++j)
        {
            if (str1[i - 1] == str2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
        }
    }
    return dp[m][n];
}

bool hamming_distance(std::vector<seqan3::dna5> const & str1, std::vector<seqan3::dna5> const & str2, size_t limit)
{
    size_t m = str1.size(), n = str2.size();
    if (m != n) {
        throw std::invalid_argument( "got 2 strings of unequal length" );
    }
    size_t distance = 0;
    for (size_t i = 1; i <= m; ++i)
    {
        if (str1[i] != str2[i]) {
            distance++;
            if (distance >= limit) {
                return false;
            }
        }
    }
    return true;
}

bool verify(std::vector<seqan3::dna5> const & ref, std::vector<seqan3::dna5> const & query, size_t start, size_t end, size_t limit)
{
    std::vector<seqan3::dna5> ref_part;
    for (size_t i = start; i <= end; ++i) {
        ref_part.push_back(ref[i]);
    }
    // auto errors = count_errors_with_indels(ref_part, query);
    return hamming_distance(ref_part, query, limit);
}

int main(int argc, char const* const* argv) {
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::nanoseconds;
    seqan3::argument_parser parser{"fmindex_pigeon_search", argc, argv, seqan3::update_notifications::off};

    parser.info.author = "SeqAn-Team";
    parser.info.version = "1.0.0";

    auto index_path = std::filesystem::path{};
    parser.add_option(index_path, '\0', "index", "path to the query file");

    auto query_file = std::filesystem::path{};
    parser.add_option(query_file, '\0', "query", "path to the query file");

    auto reference_file = std::filesystem::path{};
    parser.add_option(reference_file, '\0', "reference", "path to the reference file");

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
    auto reference_stream = seqan3::sequence_file_input{reference_file};
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

    std::vector<seqan3::dna5> reference;
    for (auto& record : reference_stream) {
        auto r = record.sequence();
        reference.insert(reference.end(), r.begin(), r.end());
    }


    // loading fm-index into memory
    using Index = decltype(seqan3::fm_index{std::vector<std::vector<seqan3::dna5>>{}}); // Some hack
    Index index; // construct fm-index
    {
        seqan3::debug_stream << "Loading 2FM-Index ... " << std::endl;
        std::ifstream is{index_path, std::ios::binary};
        cereal::BinaryInputArchive iarchive{is};
        iarchive(index);
        seqan3::debug_stream << "done\n";
    }

    auto t1 = high_resolution_clock::now();
    seqan3::configuration const cfg = seqan3::search_cfg::max_error_total{seqan3::search_cfg::error_count{0}};

    unsigned int total_count = 0;
    size_t n_parts = max_error_total+1;
    for (auto & query : queries)
    {
        std::vector<std::vector<seqan3::dna5>> parts(n_parts);
        size_t part_length = (query.size() / n_parts) + 1;
        size_t nth_part;

        for (size_t i = 0; i < query.size(); ++i)
        {
            nth_part = i / part_length;
            parts[nth_part].push_back(query[i]);
        }
        auto results = seqan3::search(parts, index, cfg);
        
        // use unordered_set to not double count matches where multiple parts match without an error
        std::unordered_set<size_t> hash_set;
        for (auto & res : results)
        {  
            size_t ref_pos = res.reference_begin_position();
            size_t nth_part = res.query_id();
            int signed_start = ref_pos - (nth_part*part_length);
            size_t end = signed_start + query.size() - 1;
            if (end < reference.size() && signed_start >= 0) {
                size_t start = (size_t)signed_start;
                if (hash_set.count(start) == 0) {
                    if (verify(reference, query, start, end, max_error_total))
                    {
                        total_count++;
                        hash_set.insert(start);
                    }
                }
            }
        }
    }
    auto t2 = high_resolution_clock::now();  
    auto t_diff = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1);
    std::cout << ">>>>>" << std::endl;
    std::cout << "> Method: FM-Index-Pigeon" << std::endl;
    std::cout << "> Query File: " << query_file << std::endl;
    std::cout << "> Query Limit: " << query_length << std::endl;
    unsigned int max_error_total_int = (unsigned int) max_error_total;
    std::cout << "> Excepted Errors: " << max_error_total_int << std::endl;
    std::cout << "> Total Count: " << total_count << std::endl;
    std::cout << "> Search duration: " << t_diff.count() << " ns\n";
    std::cout << "<<<<" << std::endl;
    return 0;
}
