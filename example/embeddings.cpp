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
#include <boost/graph/connected_components.hpp>

#include <boost/graph/planar_face_traversal.hpp>
#include <boost/graph/embeddings.hpp>

using namespace boost;

template< typename Graph, typename Embedding >
void output(Graph& g, Embedding& E, const char *str)
{
    std::size_t n_faces = num_faces(g, &E[0]);

    std::vector< int > component(num_vertices(g));
    int n_ccs = connected_components(g, &component[0]);

    std::cout << str << std::endl << "is_"
              << (is_planar_embedding(g, n_faces, n_ccs) ? "planar" : "")
              << (is_torodial_embedding(g, n_faces, n_ccs) ? "torodial" : "")
              << (is_embedding(g, n_faces, 2, n_ccs) ? "genus2" : "")
              << "_embedding  num_faces: "
              << n_faces << "  genus: "
              << genus(g, n_faces, n_ccs) << std::endl << std::endl;
}

struct vertex_output_visitor : public planar_face_traversal_visitor
{
    void begin_face() { std::cout << "New face: "; }
    void end_face() { std::cout << std::endl; }
    template < typename Vertex > void next_vertex(Vertex v)
    {
        std::cout << v << " ";
    }
} v_vis;

int main(int, char**)
{
    typedef adjacency_list< vecS, vecS, undirectedS,
        no_property, property< edge_index_t, int > >
        Graph;

    typedef Graph::edge_descriptor edge_descriptor;
    typedef std::list< edge_descriptor > list_t;

    std::vector< list_t > embedding(5);
    Graph g(5);
    emb_edge_index_update_visitor vis(get(edge_index, g), 0, &embedding);

    vis.visit_vertex_pair(0, 1, g);
    vis.visit_vertex_pair(0, 2, g);
    vis.visit_vertex_pair(0, 3, g);
    vis.visit_vertex_pair(1, 3, g);
    vis.visit_vertex_pair(1, 2, g);
    output(g, embedding, "  +---1\n  |  /|\n4 0-2 |\n  |   |\n  +---3");

    vis.visit_vertex_pair(2, 3, g);
    output(g, embedding, "  +---1\n  |  /|\n4 0-2-|-+\n  |   | |\n  +---3-+");

    embedding[3].reverse();
    output(g, embedding, "  +---1\n  |  /|\n4 0-2 |\n  |  \\|\n  +---3");

    vis.visit_vertex_pair(0, 4, g);
    vis.visit_vertex_pair(2, 4, g);
    vis.visit_vertex_pair(1, 4, g);
    output(g, embedding,
      "  +---+\n+-|---1+\n| |  / |\n4-0-2  |\n| | |\\ |\n+-|-+ \\|\n  +----3");

    std::cout << "Vertices on the faces: " << std::endl;
    planar_face_traversal(g, &embedding[0], v_vis);

    return 0;
}
