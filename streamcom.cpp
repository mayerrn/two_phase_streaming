#include "streamcom.hpp"
#include <iostream>
#include <algorithm>

DECLARE_string(communities_file);
DECLARE_int32(str_iters);

Streamcom::Streamcom(const Globals &GLOBALS) : globals(const_cast<Globals &>(GLOBALS))
{
    communities.resize(GLOBALS.NUM_VERTICES, 0);
//    internal_vertex_degrees.resize(GLOBALS.NUM_VERTICES, 0);
//    external_vertex_degrees.resize(GLOBALS.NUM_VERTICES, 0);
    volumes.resize(GLOBALS.NUM_VERTICES + 1, 0);
//    internal_degrees.resize(GLOBALS.NUM_VERTICES + 1, 0);
    if (globals.CLUSTER_QUALITY_EVAL){
    	external_degrees.resize(GLOBALS.NUM_VERTICES + 1, 0);
    	quality_scores.resize(GLOBALS.NUM_VERTICES + 1, 0.0);
    }
    next_community_id = 1;
}

void find_com_forwarder(void* object, std::vector<edge_t> edges)
{
    static_cast<Streamcom*>(object)->do_streamcom(edges);
}

void eval_com_forwarder(void* object, std::vector<edge_t> edges)
{
    static_cast<Streamcom*>(object)->do_communities_evaluation(edges);
}

std::vector<uint32_t> Streamcom::find_communities()
{
    // Decreasing community volume in the first run.
    // This makes to smaller communities after the first communities so that they would be avaiable
    // for extending in the second clustering

    LOG(INFO) << "Community detection... [will be processed " << FLAGS_str_iters << " times]";
    
    switch (FLAGS_str_iters)
    {
        case 1:
//            globals.MAX_COM_VOLUME *= 0.8;
            globals.read_and_do(find_com_forwarder, this, "communities");
            break;
        default:
//            globals.MAX_COM_VOLUME *= 0.3;
            globals.read_and_do(find_com_forwarder, this, "communities");

            if (globals.CLUSTER_QUALITY_EVAL){
            	evaluate_communities();
            }

//            globals.MAX_COM_VOLUME *= 1.2;
            globals.read_and_do(find_com_forwarder, this, "communities");


            for (int i = 3; i <= FLAGS_str_iters; i++)
            {
               	if (globals.CLUSTER_QUALITY_EVAL){
                		evaluate_communities();
                }
            	globals.read_and_do(find_com_forwarder, this, "communities");

            }
            break;
    }
    return communities;
}

void Streamcom::do_streamcom(std::vector<edge_t> &edges)
{
    for (auto& edge : edges)
    {
        auto u = edge.first;
        auto v = edge.second;

        auto& com_u = communities[u];
        auto& com_v = communities[v];
        if(com_u == 0)
        {
            com_u = next_community_id;
            volumes[com_u] += globals.DEGREES[u];
            ++next_community_id;
        }
        if(com_v == 0)
        {
            com_v = next_community_id;
            volumes[com_v] += globals.DEGREES[v];
            ++next_community_id;
        }

        auto& vol_u = volumes[com_u];
        auto& vol_v = volumes[com_v];


        auto real_vol_u = vol_u - globals.DEGREES[u];
        auto real_vol_v = vol_v - globals.DEGREES[v];

//        double utilization_u = (double) real_vol_u / globals.MAX_COM_VOLUME;
//        double utilization_v = (double) real_vol_v / globals.MAX_COM_VOLUME;

//        std::cout << "vol_u " << vol_u << " score_u " << score_u << " utilization_u " << utilization_u << std::endl;


        /**
         * using cluster and vertex conductance metrics
         */
        if((vol_u <= globals.MAX_COM_VOLUME) && (vol_v <= globals.MAX_COM_VOLUME))
        {
            if (globals.CLUSTER_QUALITY_EVAL){
            	auto& score_u = quality_scores[com_u];
            	auto& score_v = quality_scores[com_v];
            	// take into account quality scores when making the vertex move decision
            	if(real_vol_u <= real_vol_v && score_u >= score_v && vol_v + globals.DEGREES[u] <= globals.MAX_COM_VOLUME){
            		// move u to cluster of v
            		vol_u -= globals.DEGREES[u];
            		vol_v += globals.DEGREES[u];
            		communities[u] = communities[v];
            	}
            	else if (real_vol_v < real_vol_u && score_v >= score_u && vol_u + globals.DEGREES[v] <= globals.MAX_COM_VOLUME){
            		// move v to cluster of u
            		vol_v -= globals.DEGREES[v];
            		vol_u += globals.DEGREES[v];
            		communities[v] = communities[u];
            	}
            }
            else { // if cluster quality is not taken into account
                if(real_vol_u <= real_vol_v && vol_v + globals.DEGREES[u] <= globals.MAX_COM_VOLUME){
               		vol_u -= globals.DEGREES[u];
               		vol_v += globals.DEGREES[u];
               		communities[u] = communities[v];
                }
                else if (real_vol_v < real_vol_u && vol_u + globals.DEGREES[v] <= globals.MAX_COM_VOLUME) {
           			vol_v -= globals.DEGREES[v];
           			vol_u += globals.DEGREES[v];
           			communities[v] = communities[u];
                }
            }
        }
    }
}

void Streamcom::evaluate_communities()
{
//	std::fill(internal_degrees.begin(), internal_degrees.end(), 0);
	std::fill(external_degrees.begin(), external_degrees.end(), 0);
//	std::fill(internal_vertex_degrees.begin(), internal_degrees.end(), 0);
//	std::fill(external_vertex_degrees.begin(), external_degrees.end(), 0);
	globals.read_and_do(eval_com_forwarder, this, "quality evaluation");
}

void Streamcom::do_communities_evaluation(std::vector<edge_t> &edges)
{

	for (auto& edge : edges)
	{
		auto u = edge.first;
		auto v = edge.second;

		auto& com_u = communities[u];
		auto& com_v = communities[v];

//		if (com_u == com_v){
//			internal_degrees[com_u] += 2;
//			internal_vertex_degrees[u]++;
//			internal_vertex_degrees[v]++;
//		}
//		else {
		if (com_u != com_v){
			external_degrees[com_u]++;
			external_degrees[com_v]++;
//			external_vertex_degrees[u]++;
//			external_vertex_degrees[v]++;
		}

	}

	/**
	 * coverage
	 */
//	for (size_t i = 0; i <= globals.NUM_VERTICES; i++){
//		size_t total_degree = internal_degrees[i] + external_degrees[i];
//		if (total_degree != 0){
//			double score = (double) internal_degrees[i] / total_degree;
//			quality_scores[i] = score;
//		}
//	}

	/**
	 * conductance
	 */
	for (size_t i = 0; i <= globals.NUM_VERTICES; i++){
		size_t denominator = std::min(volumes[i], 2*globals.NUM_EDGES - volumes[i]);
		if (denominator != 0){
			double score = (double) external_degrees[i] / denominator;
			quality_scores[i] = score;
		}
	}
}

std::vector<uint64_t> Streamcom::get_volumes()
{
    return volumes;
}

std::vector<double> Streamcom::get_quality_scores()
{
    return quality_scores;
}


