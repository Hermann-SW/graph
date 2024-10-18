//=======================================================================
// Copyright 2024 Hermann Stamm-Wilbrandt
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
#include <boost/core/lightweight_test.hpp>
#include <boost/graph/adjacency_list.hpp>

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

int main(int, char**)
{
    typedef adjacency_list< vecS, vecS, undirectedS,
                            no_property, property< edge_index_t, int > > Graph;

    typedef Graph::edge_descriptor edge_descriptor;
    typedef std::list< edge_descriptor > list_t;

    std::vector< list_t > embedding(5);
    Graph g(4);
    add_edge(0, 1, g, embedding);
    add_edge(0, 2, g, embedding);
    add_edge(0, 3, g, embedding);
    add_edge(1, 3, g, embedding);
    add_edge(1, 2, g, embedding);

    std::size_t n_faces = num_faces(g, &embedding[0]);
    BOOST_ASSERT(n_faces == 3);
    BOOST_ASSERT(is_planar_embedding(g, n_faces));
    BOOST_ASSERT(!is_torodial_embedding(g, n_faces));
    BOOST_ASSERT(!is_embedding(g, n_faces, 2));
    BOOST_ASSERT(genus(g, n_faces) == 0);

    add_edge(2, 3, g, embedding);

    n_faces = num_faces(g, &embedding[0]);
    BOOST_ASSERT(n_faces == 2);
    BOOST_ASSERT(!is_planar_embedding(g, n_faces));
    BOOST_ASSERT(is_torodial_embedding(g, n_faces));
    BOOST_ASSERT(!is_embedding(g, n_faces, 2));
    BOOST_ASSERT(genus(g, n_faces) == 1);

    embedding[3].reverse();

    n_faces = num_faces(g, &embedding[0]);
    BOOST_ASSERT(n_faces == 4);
    BOOST_ASSERT(is_planar_embedding(g, n_faces));
    BOOST_ASSERT(!is_torodial_embedding(g, n_faces));
    BOOST_ASSERT(!is_embedding(g, n_faces, 2));
    BOOST_ASSERT(genus(g, n_faces) == 0);

    add_edge(0, 4, g, embedding);
    add_edge(2, 4, g, embedding);
    add_edge(1, 4, g, embedding);

    n_faces = num_faces(g, &embedding[0]);
    BOOST_ASSERT(n_faces == 2);
    BOOST_ASSERT(!is_planar_embedding(g, n_faces));
    BOOST_ASSERT(!is_torodial_embedding(g, n_faces));
    BOOST_ASSERT(is_embedding(g, n_faces, 2));
    BOOST_ASSERT(genus(g, n_faces) == 2);
    return boost::report_errors();
}
