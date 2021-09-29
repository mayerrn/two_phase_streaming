#include <vector>
#include <math.h>

#include "stats.hpp"

Stats::Stats(TwoPhasePartitioner& partitioner, Globals& globals)
: partitioner(partitioner), globals(globals)
{}

void Stats::compute_num_total_replicas()
{
    const auto& vertex_partition_matrix = partitioner.get_vertex_partition_matrix();
    for (const auto& bitset : vertex_partition_matrix)
    {
        num_total_replicas += static_cast<uint64_t>(bitset.count());
    }
}

uint64_t Stats::get_num_total_replicas()
{
    if (num_total_replicas == 0) compute_num_total_replicas();
    return num_total_replicas;
}
void Stats::compute_replication_factor()
{
    uint64_t replicas = get_num_total_replicas();
    replication_factor = double(replicas)/double(globals.NUM_VERTICES);
}
void Stats::compute_std_dev_load()
{
    auto machine_edge_loads = partitioner.get_edge_load();
    auto num_machines = globals.NUM_PARTITIONS;
    double average_load = 0;
    for (auto machine_edge_load : machine_edge_loads)
    {
        average_load += machine_edge_load;
    }
    average_load /= num_machines;

    double num = 0;
    for (int m = 0; m < num_machines; m++)
    {
        num += pow(machine_edge_loads[m] - average_load, 2);
    }
    num /= num_machines;
    std_dev_load = sqrt(num);
    std_dev_load /= average_load;
}
void Stats::compute_and_print_stats()
{
    LOG(INFO) << "computing stats ...";
    compute_replication_factor();
    compute_std_dev_load();

    LOG(INFO) << "vertices: " << globals.NUM_VERTICES;
    LOG(INFO) << "replicas: " << num_total_replicas;
    LOG(INFO) << "replication factor: " << round(replication_factor * 10000.0) / 10000.0;

    auto machine_edge_loads = partitioner.get_edge_load();
    double max_edge_load = 0;
    uint64_t total_load = 0;
    for (size_t i = 0; i < machine_edge_loads.size(); i++)
    {
        LOG(INFO) << "load in partition " << i << ": " << machine_edge_loads[i];
        total_load += machine_edge_loads[i];
        if (max_edge_load < machine_edge_loads[i])
        {
            max_edge_load = machine_edge_loads[i];
        }
    }
    LOG(INFO) << "total edge load: " << total_load;
    LOG(INFO) << "load balance (standard deviation): " << round(std_dev_load * 10000.0) / 10000.0;
    LOG(INFO) << "load balance (max_edge_load / average_edge_load): " <<  max_edge_load / ((double)total_load/globals.NUM_PARTITIONS);
}
