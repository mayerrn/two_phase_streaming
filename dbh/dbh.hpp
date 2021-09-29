#ifndef HDRFPP__DBH_HPP_
#define HDRFPP__DBH_HPP_

#include "globals.hpp"
#include "bitset"

class DBH
{
private:
    Globals& globals;
    uint64_t min_load = UINT64_MAX;
    uint64_t max_load = 0;
    double seed;
    int shrink{};

#ifdef STATS
    std::vector<uint64_t> edge_load;
    std::vector<std::bitset<MAX_NUM_PARTITION> > vertex_partition_matrix;
#endif
public:
    static const int MAX_SHRINK = 100;
    std::ofstream part_file;
    explicit DBH(Globals& GLOBALS);
    void perform_partitioning();
    void do_dbh(const std::vector<edge_t>& edges);
    int hash_vertex(uint32_t vertex);
    void write_edge(edge_t e, int p);  

#ifdef STATS
    std::vector<uint64_t>& get_edge_load();
    std::vector<std::bitset<MAX_NUM_PARTITION>> get_vertex_partition_matrix();
#endif
};

#endif //HDRFPP__DBH_HPP_
