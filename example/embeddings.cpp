//=======================================================================
// Copyright 2024 Hermann Stamm-Wilbrandt
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>

#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/embeddings.hpp>

using namespace boost;

std::size_t edge_count = 0;

template< typename Graph, typename PlanarEmbedding >
std::pair< typename Graph::edge_descriptor, bool >
add_edge(typename Graph::vertex_descriptor v,
         typename Graph::vertex_descriptor w,
         Graph& g,
         PlanarEmbedding& E)
{
    std::pair< typename Graph::edge_descriptor, bool > ep = add_edge(v, w, g);
    put(get(edge_index, g), ep.first, edge_count++);

    E[v].push_back(ep.first);
    E[w].push_back(ep.first);

    return ep;
}

template< typename Graph, typename PlanarEmbedding >
void output(Graph& g, PlanarEmbedding& E, const char *str)
{
    std::size_t n_faces = num_faces(g, &E[0]);
    std::cout << str << std::endl << "is_"
              << (is_planar_embedding(g, n_faces) ? "planar" : "")
              << (is_torodial_embedding(g, n_faces) ? "torodial" : "")
              << "_embedding  num_faces: "
              << n_faces << "  genus: "
              << genus(g, n_faces) << std::endl << std::endl;
}

int main(int argc, char** argv)
{
    typedef adjacency_list< vecS, vecS, undirectedS,
        no_property, property< edge_index_t, int > >
        Graph;

    typedef Graph::vertex_descriptor vertex_descriptor;
    typedef Graph::edge_descriptor edge_descriptor;
    typedef std::list< edge_descriptor > list_t;

    std::vector< list_t > embedding(4);
    Graph g(4);
    add_edge(0, 1, g, embedding);
    add_edge(0, 2, g, embedding);
    add_edge(0, 3, g, embedding);
    add_edge(1, 3, g, embedding);
    add_edge(1, 2, g, embedding);
    output(g, embedding, "+---1\n|  /|\n0-2 |\n|   |\n+---3");

    add_edge(2, 3, g, embedding);
    output(g, embedding, "+---1\n|  /|\n0-2-|-+\n|   | |\n+---3-+");

    embedding[3].reverse();
    output(g, embedding, "+---1\n|  /|\n0-2 |\n|  \\|\n+---3");

    return 0;
}
