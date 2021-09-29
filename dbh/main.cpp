#include <vector>
#include <string>

#include "globals.hpp"
#ifdef STATS
#include "stats.hpp"
#endif
#include "dbh.hpp"
#include "../converter/conversions.hpp"

DEFINE_string(filename, "", "name of the file to store edge list of a graph.");
DEFINE_int32(p, 0, "the number of partitions that the graph will be divided into.");
DEFINE_uint64(memsize, 100, "memory size in MB. "
                            "Memsize is used as a chunk size if shuffling and batch size while partitioning");
DEFINE_bool(write_parts, false, "write out the partitions to a file");
DEFINE_string(parts_filename, "", "partitions filename");


void start_partitioning(Globals& globals, DBH& partitioner, std::string& binedgelist);

int main(int argc, char *argv[]) 
{
    // set up glogs and gflags
    google::ParseCommandLineNonHelpFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;
    google::HandleCommandLineHelpFlags();

    // convert edge list to binary format
    Converter* converter;
    std::string binedgelist;
    converter = new Converter(FLAGS_filename);
    binedgelist = binedgelist_name(FLAGS_filename);
    convert(FLAGS_filename, converter);

    // set up globals
    std::ifstream fin(binedgelist, std::ios::binary | std::ios::ate);
    Globals globals(fin, FLAGS_filename, FLAGS_p);

    // init partitioner
    DBH dbh(globals);

    // start partitioner
    start_partitioning(globals, dbh, binedgelist);

#ifdef STATS
    Stats stats(dbh, globals);
    stats.compute_and_print_stats();
#endif

  return 0;
};

void start_partitioning(Globals& globals, DBH& partitioner, std::string& binedgelist)
{
    Timer partitioner_timer;
    partitioner_timer.start();

    globals.calculate_degrees();
    partitioner.perform_partitioning();

    partitioner_timer.stop();
    LOG(INFO) << "partitioning time: " << partitioner_timer.get_time();
}
