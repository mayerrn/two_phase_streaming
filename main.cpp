#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "globals.hpp"
#include "stats.hpp"
#include "twophasepartitioner.hpp"
#include "streamcom.hpp"
#include "converter/conversions.hpp"
//#include "rabbit_order/reorder.hpp"

#include "converter/shuffler.hpp"

DEFINE_string(filename, "", "name of the file to store edge list of a graph.");
DEFINE_int32(p, 4, "the number of partitions that the graph will be divided into.");
DEFINE_double(lambda, 1, "lambda value for hdrf");
DEFINE_bool(shuffle, false, "shuffle the dataset");
DEFINE_uint64(memsize, 100, "memory size in MB. "
                            "Memsize is used as a chunk size if shuffling and batch size while partitioning");
DEFINE_string(prepartitioner_type, "", "name of the community detector algorithm as a prepartitioner");
DEFINE_double(balance_ratio, 1.05, "balance ratio");
DEFINE_bool(print_coms, false, "print communities in ascending order of vertex ids");
DEFINE_bool(cluster_quality_eval, false, "evaluate the cluster quality between two subsequent streaming clustering passes");
DEFINE_bool(use_hdrf, false, "use HDRF instead of linear-time streaming in the second phase of 2PS");
DEFINE_string(coms_filename, "", "community filename");
DEFINE_string(parts_filename, "", "partitions filename");
DEFINE_int32(str_iters, 1, "the number of iterations of streaming clustering");
DEFINE_bool(write_parts, false, "write out the partitions to a file");

void start_partitioning(Globals &globals, TwoPhasePartitioner &partitioner, std::string &binedgelist);

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
    if (FLAGS_shuffle)
    {
        LOG(INFO) << "Using shuffled dataset";
        converter = new Shuffler(FLAGS_filename);
        binedgelist = shuffled_binedgelist_name(FLAGS_filename);
    }
    else
    {
        converter = new Converter(FLAGS_filename);
        binedgelist = binedgelist_name(FLAGS_filename);
    }
    convert(FLAGS_filename, converter);

    // set up globals
    std::ifstream fin(binedgelist, std::ios::binary | std::ios::ate);
    Globals globals(fin, FLAGS_filename, FLAGS_p, FLAGS_prepartitioner_type, FLAGS_lambda, FLAGS_cluster_quality_eval);

    // init partitioner
    TwoPhasePartitioner hdrf(globals);

    // start partitioner
    start_partitioning(globals, hdrf, binedgelist);

    // stats
    Stats stats(hdrf, globals);
    stats.compute_and_print_stats();

  return 0;
}

void start_partitioning(Globals &globals, TwoPhasePartitioner &partitioner, std::string &binedgelist)
{
    Timer partitioner_timer;
    partitioner_timer.start();

    // check if a user set any prepartitioner
    if (!FLAGS_prepartitioner_type.empty())
    {
        std::vector<uint32_t> communities;
        std::vector<uint64_t> volumes;
        std::vector<double> quality_scores;

        globals.calculate_degrees();
        if (FLAGS_prepartitioner_type == "streamcom")
        {
            Streamcom streamcom(globals);
            communities = streamcom.find_communities();

            // get volumes of the communities
            volumes = streamcom.get_volumes();

            // get qscores of the communities
            quality_scores = streamcom.get_quality_scores();
        }
        else 
        {
            LOG(ERROR) << "Only str is supported as a prepartitioner";
            exit(-1);
        }

        if (FLAGS_print_coms)
        {
            std::ofstream com_file;
            com_file.open(FLAGS_coms_filename + "_" + std::to_string(FLAGS_str_iters) + "x.txt");
            for (auto com : communities) com_file << com << " ";
            com_file << std::endl;
            com_file.close();
            exit(0);
        }

        // prepartition using communities and partition the rest of edges using hdrf
        partitioner.perform_prepartition_and_partition(communities, volumes, quality_scores);
    }
    else
    {
        // partition with hdrf
        partitioner.perform_partitioning();
    }
    partitioner_timer.stop();
    LOG(INFO) << "partitioning time: " << partitioner_timer.get_time();
}
