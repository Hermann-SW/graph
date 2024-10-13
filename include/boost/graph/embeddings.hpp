//=======================================================================
// Copyright (c) Hermann Stamm-Wilbrandt 2024
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#ifndef __EMBEDDINGS_HPP__
#define __EMBEDDINGS_HPP__

#include <vector>
#include <list>
#include <boost/config.hpp>
#include <boost/next_prior.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_map/property_map.hpp>

namespace boost
{

std::size_t embeddings_num_faces;

struct count_visitor : public planar_face_traversal_visitor
{
    void begin_traversal() { embeddings_num_faces = 0; }
    void begin_face() { ++embeddings_num_faces; }
};

template< class graph >
bool is_embedding(graph g, std::size_t n_faces, std::size_t genus)
{
    return n_faces + num_vertices(g) - num_edges(g) == 2 - 2 * genus;
}

template< class graph >
bool is_planar_embedding(graph g, std::size_t n_faces)
{
    return is_embedding(g, n_faces, 0);
}

template< class graph >
bool is_torodial_embedding(graph g, std::size_t n_faces)
{
    return is_embedding(g, n_faces, 1);
}

template< typename graph >
std::size_t genus(graph g, std::size_t n_faces)
{
    std::size_t u = num_edges(g) + 2 - n_faces - num_vertices(g);
    BOOST_ASSERT(u % 2 == 0);
    return u / 2;
}

template < typename Graph, typename PlanarEmbedding >
std::size_t num_faces(
    const Graph& g, PlanarEmbedding embedding)
{
    count_visitor c_vis;
    planar_face_traversal(g, &embedding[0], c_vis);
    return embeddings_num_faces;

}

} // namespace boost

#endif //__EMBEDDINGS_HPP__
