#include "dbh.hpp"

void dbh_forwarder(void* object, std::vector<edge_t> edges);

DECLARE_bool(write_parts);
DECLARE_string(parts_filename);


DBH::DBH(Globals &GLOBALS) : globals(GLOBALS)
{
    seed = static_cast<double>(random()) / RAND_MAX;
    shrink = static_cast<int>(random()) % MAX_SHRINK;

    if (FLAGS_write_parts) {
        part_file.open(FLAGS_parts_filename + ".txt");
    }
#ifdef STATS
    vertex_partition_matrix.resize(globals.NUM_VERTICES);
    edge_load.resize(globals.NUM_PARTITIONS, 0);
#endif
}

void DBH::perform_partitioning()
{
    globals.read_and_do(dbh_forwarder, this, "partitions with DBH");
    if (FLAGS_write_parts) {
        part_file.close();
    }
}

void DBH::write_edge(edge_t e, int p){
    part_file << e.first << " " << e.second << " " << p << std::endl;
}


void DBH::do_dbh(const std::vector<edge_t>& edges)
{
    for (const edge_t& e : edges)
    {
        auto u = e.first;
        auto v = e.second;

        auto degree_u = globals.DEGREES[u];
        auto degree_v = globals.DEGREES[v];

        int p_id;
        if (degree_v < degree_u)
        {
            p_id = hash_vertex(v);
        }
        else if (degree_u < degree_v)
        {
            p_id = hash_vertex(u);
        }
        else
        {
            int choice = static_cast<int>(random()) % 2;
            if (choice == 0)
            {
                p_id = hash_vertex(u);
            }
            else if (choice == 1)
            {
                p_id = hash_vertex(v);
            }
            else
            {
                LOG(ERROR) << "ERROR IN RANDOM CHOICE DBH";
                exit(-1);
            }
        }

	if (FLAGS_write_parts){
            write_edge(e, p_id);
        }

#ifdef STATS
        vertex_partition_matrix[e.first][p_id] = true;
        vertex_partition_matrix[e.second][p_id] = true;

        auto& load = ++edge_load[p_id];
        if (load < min_load) min_load = load;
        if (load > max_load) max_load = load;
#endif
    }
}

int DBH::hash_vertex(uint32_t v) 
{
   return abs(static_cast<int>(static_cast<uint32_t>(v * seed * shrink) % globals.NUM_PARTITIONS)); 
}

#ifdef STATS
std::vector<uint64_t>& DBH::get_edge_load()
{
    return edge_load;
}

std::vector<std::bitset<MAX_NUM_PARTITION>> DBH::get_vertex_partition_matrix()
{
    return vertex_partition_matrix;
}
#endif

void dbh_forwarder(void* object, std::vector<edge_t> edges)
{
    static_cast<DBH*>(object)->do_dbh(edges);
}
