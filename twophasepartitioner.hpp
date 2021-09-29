#ifndef HDRFPP__TWOPHASEPARTITIONER_HPP_
#define HDRFPP__TWOPHASEPARTITIONER_HPP_

#include <vector>
#include <unordered_map>
#include <fstream>
#include <bitset>

#include "globals.hpp"

class TwoPhasePartitioner
{
private:
    std::vector<uint64_t> edge_load;
    std::vector<std::bitset<MAX_NUM_PARTITION> > vertex_partition_matrix;
    uint64_t min_load = UINT64_MAX;
    uint64_t max_load = 0;
    bool tie_breaker_min_load = false;
    Globals& globals;
    const double epsilon = 1;
    std::ofstream part_file;

    std::vector<uint64_t> partition_volume;
    std::vector<uint32_t> com2part;

    std::vector<uint32_t> communities;
    std::vector<uint64_t> volumes;
    std::vector<double> quality_scores;

    int find_min_vol_partition();
    int find_max_score_partition(edge_t& e);
    int find_max_score_partition_hdrf(edge_t& e);
    void update_min_max_load(int max_p);
    void update_vertex_partition_matrix(edge_t& e, int max_p);

public:
    explicit TwoPhasePartitioner(Globals& GLOBALS);
    void perform_prepartition_and_partition(std::vector<uint32_t> communities, std::vector<uint64_t> volumes, std::vector<double> quality_scores);
    void do_sorted_com_prepartitioning(std::vector<edge_t> edges);
    void do_hdrf(std::vector<edge_t> &edges);
    void do_linear(std::vector<edge_t> &edges);
    void write_edge(edge_t e, int p);
    std::vector<uint64_t>& get_edge_load();
    void perform_partitioning();
    std::vector<std::bitset<MAX_NUM_PARTITION>> get_vertex_partition_matrix();

};

#endif
