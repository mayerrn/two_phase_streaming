#ifndef HDRFPP__STREAMCOM_HPP_
#define HDRFPP__STREAMCOM_HPP_

#include <unordered_map>
#include <fstream>

#include "globals.hpp"
#include "twophasepartitioner.hpp"

DECLARE_string(streamcom_type);

class Streamcom
{
private:
    std::vector<uint32_t> communities; // index is vertex id, community of a vertex
//    std::vector<uint64_t> internal_vertex_degrees; // index is vertex id, number of intra-cluster edges
//    std::vector<uint64_t> external_vertex_degrees; // index is vertex id, number of inter-cluster edges
    std::vector<uint64_t> volumes; // index is community id, volume of a community
//	std::vector<uint64_t> internal_degrees; // internal degree of each community; index is community id
	std::vector<uint64_t> external_degrees; // external degree of each community; index is community id
    std::vector<double> quality_scores; // quality of the communities (intra-cluster edges / inter-cluster edges)
    uint32_t next_community_id;
    Globals &globals;

public:
    explicit Streamcom(const Globals& GLOBALS);
    std::vector<uint32_t> find_communities();
    void evaluate_communities();
    std::vector<uint64_t> get_volumes();
    std::vector<double> get_quality_scores();
    void do_streamcom(std::vector<edge_t> &edges);
    void do_communities_evaluation(std::vector<edge_t> &edges);
    typedef std::vector<std::pair<uint64_t, uint32_t>> sort_communities();
};

#endif //HDRFPP__STREAMCOM_HPP_
