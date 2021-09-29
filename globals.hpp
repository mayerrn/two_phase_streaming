#ifndef HDRFPP__GLOBALS_HPP_
#define HDRFPP__GLOBALS_HPP_

#ifndef MAX_NUM_PARTITION
    #warning MAX NUMBER OF PARTITIONS is defined to 256.
    #define MAX_NUM_PARTITION 256
#endif

#include <string>
#include <fstream>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "converter/util.hpp"

class Globals
{
private:
    std::ifstream FIN;
    void print();
public:
    uint32_t NUM_VERTICES{};
    size_t NUM_EDGES{};
    size_t FILESIZE{};

    // batch processing globals
    uint32_t NUM_BATCHES{};
    uint32_t NUM_EDGES_PER_BATCH{};
    std::string INPUT_FILE_NAME;
    const int NUM_PARTITIONS;
    std::string PREPARTITIONER;
    double LAMBDA;

    bool CLUSTER_QUALITY_EVAL;

    // prepartitioner global
    uint64_t MAX_COM_VOLUME{};
    std::vector<uint32_t> DEGREES;

    uint64_t MAX_PARTITION_LOAD{};

    Globals(std::ifstream &fin, std::string input_file_name, uint32_t p, std::string prepartitioner, double lambda, bool cluster_quality_eval);
    void read_and_do(void (*f)(void*, std::vector<edge_t>), void* context, std::string context_name);
    void calculate_degrees();
    void do_degree_calculation(std::vector<edge_t> edges);
};

#endif //HDRFPP__GLOBALS_HPP_
