#include <gflags/gflags.h>
#include <fstream>
#include <iostream>
#include <unordered_map>


DEFINE_string(input, "", "binary edgelist input filename, .binedgelist extension");
DEFINE_string(coms, "", "communities filename, received with twophasepartitioner program");

typedef uint32_t vid_t;
struct edge_t {
    vid_t first, second;
    edge_t() : first(0), second(0) {}
    edge_t(vid_t first, vid_t second) : first(first), second(second) {}

    bool operator==(const edge_t& other) const noexcept
    {
        return (first == other.first && second == other.second) || (first == other.second && second == other.first);
    }
};

int main(int argc, char *argv[])
{
    google::ParseCommandLineNonHelpFlags(&argc, &argv, true);

    vid_t num_vertices;
    size_t num_edges;
    std::vector<edge_t> edges;

    std::cout << "Reading edges from file..." << std::endl;
    std::ifstream fin(FLAGS_input, std::ios::binary | std::ios::ate);
    if (!fin)
    {
        std::cout << "The file in the the given path -  " + FLAGS_input + " cannot be read."; 
        exit(-1);
    }
    
    fin.seekg(0, std::ios::beg);
    fin.read((char*)&num_vertices, sizeof(vid_t));
    fin.read((char*)&num_edges, sizeof(size_t));

    edges.resize(num_edges);
    fin.read((char*)&edges[0], sizeof(edge_t) * num_edges);
    fin.close();
    std::cout << "Finished reading edges from file." << std::endl;

    std::cout << "Reading communities from file..." << std::endl;
    std::ifstream fincoms(FLAGS_coms);
    if (!fincoms)
    {
        std::cout << "The file in the the given path -  " + FLAGS_coms + " cannot be read."; 
        exit(-1);
    }

    std::vector<uint32_t > coms(num_vertices, 0);
    uint32_t c;
    uint32_t v_id = 0; 
    while (fincoms >> c)
    {
        coms[v_id] = c;
        v_id++;
    }

    fincoms.close();
    std::cout << "Finished reading communities from file" << std::endl;

    std::cout << "calculating modularity score ..." << std::endl;
    double Q = 0;

    std::unordered_map<uint32_t, uint32_t> com2intra; // community id to intra-community edge count
    std::unordered_map<uint32_t, uint32_t> com2degrees; // sum of degrees of the vertices in a community
    for (auto com : coms)
    {
        com2intra[com] = 0;
        com2degrees[com] = 0;
    }

    for (edge_t e : edges)
    {
        auto com_first = coms[e.first];
        auto com_second = coms[e.second];

        if (com_first == com_second)
        {
	    com2intra[com_first]++; 
        }

        com2degrees[com_first]++; 
        com2degrees[com_second]++; 
        
    }

    auto edge_count = edges.size();
    for (auto const& x : com2intra)
    {
        double a1 = (double) com2degrees[x.first]/ ((double) edge_count * 2.0);
	double a2 = a1 * a1; 

        Q = Q + (((double) x.second / (double) edge_count) - a2);
    }

   std::cout << Q << std::endl;
   return 0;
}

