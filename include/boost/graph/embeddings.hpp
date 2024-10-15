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

template< class graph >
bool is_embedding(graph g, std::size_t n_faces, std::size_t genus,
                  std::size_t n_ccs=1)
{
    return n_faces + num_vertices(g) - num_edges(g) == 1 + n_ccs - 2 * genus;
}

template< class graph >
bool is_planar_embedding(graph g, std::size_t n_faces, std::size_t n_ccs = 1)
{
    return is_embedding(g, n_faces, 0, n_ccs);
}

template< class graph >
bool is_torodial_embedding(graph g, std::size_t n_faces, std::size_t n_ccs = 1)
{
    return is_embedding(g, n_faces, 1, n_ccs);
}

template< typename graph >
std::size_t genus(graph g, std::size_t n_faces, std::size_t n_ccs = 1)
{
    std::size_t u = num_edges(g) + 1 + n_ccs - n_faces - num_vertices(g);
    BOOST_ASSERT(u % 2 == 0);
    return u / 2;
}

template < typename Graph, typename PlanarEmbedding, typename EdgeIndexMap >
std::size_t num_faces(
    const Graph& g, PlanarEmbedding embedding, EdgeIndexMap em)
{
    struct : public planar_face_traversal_visitor
    {
        std::size_t n_faces = 0;

        void begin_face() { ++n_faces; };
    } cnt_vis;

    planar_face_traversal(g, &embedding[0], cnt_vis, em);

    return cnt_vis.n_faces;
}

template < typename Graph, typename PlanarEmbedding >
inline std::size_t num_faces(const Graph& g, PlanarEmbedding embedding)
{
    return num_faces(g, embedding, get(edge_index, g));
}

} // namespace boost

#endif //__EMBEDDINGS_HPP__
