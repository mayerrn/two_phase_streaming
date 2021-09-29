#ifndef HDRFPP__STATISTICS_HPP_
#define HDRFPP__STATISTICS_HPP_

#include "globals.hpp"
#include "dbh.hpp"

#ifdef STATS
class Stats
{
private:
    DBH& partitioner;
    Globals& globals;
    double replication_factor{};
    double std_dev_load{};
    uint64_t num_total_replicas = 0;

    void compute_num_total_replicas();
    uint64_t get_num_total_replicas();
    void compute_replication_factor();
    void compute_std_dev_load();

public:
    explicit Stats(DBH& partitiner, Globals& globals);
    void compute_and_print_stats();
};
#endif

#endif
