//=======================================================================
// Copyright (c) Hermann Stamm-Wilbrandt 2024
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#ifndef __EMBEDDINGS_HPP__
#define __EMBEDDINGS_HPP__

#include <boost/foreach.hpp>
#include <boost/graph/planar_face_traversal.hpp>

namespace boost
{

template< class graph >
bool is_embedding(graph& g, std::size_t n_faces, std::size_t genus,
                  std::size_t n_ccs=1)
{
    return n_faces + num_vertices(g) - num_edges(g) == 1 + n_ccs - 2 * genus;
}

template< class graph >
bool is_planar_embedding(graph& g, std::size_t n_faces, std::size_t n_ccs = 1)
{
    return is_embedding(g, n_faces, 0, n_ccs);
}

template< class graph >
bool is_torodial_embedding(graph& g, std::size_t n_faces, std::size_t n_ccs = 1)
{
    return is_embedding(g, n_faces, 1, n_ccs);
}

template< typename graph >
std::size_t genus(graph& g, std::size_t n_faces, std::size_t n_ccs = 1)
{
    std::size_t u = num_edges(g) + 1 + n_ccs - n_faces - num_vertices(g);
    BOOST_ASSERT(u % 2 == 0);
    return u / 2;
}

template < typename Graph, typename Embedding, typename EdgeIndexMap >
std::size_t num_faces(
    const Graph& g, Embedding embedding, EdgeIndexMap em)
{
    struct : public planar_face_traversal_visitor
    {
        std::size_t n_faces = 0;

        void begin_face() { ++n_faces; };
    } cnt_vis;

    planar_face_traversal(g, &embedding[0], cnt_vis, em);

    return cnt_vis.n_faces;
}

template < typename Graph, typename Embedding >
inline std::size_t num_faces(const Graph& g, Embedding embedding)
{
    return num_faces(g, embedding, get(edge_index, g));
}


// enhanced "struct edge_index_update_visitor" in planar_detail/add_edge_visitors.hpp
//
template < typename EdgeIndexMap, typename Embedding > struct emb_edge_index_update_visitor
{
    typedef
        typename property_traits< EdgeIndexMap >::value_type edge_index_value_t;

    emb_edge_index_update_visitor(
        EdgeIndexMap em, edge_index_value_t next_index_available, Embedding *E)
    : m_em(em), m_next_index(next_index_available), m_E(E)
    {
    }

    template < typename Graph, typename Vertex >
    void visit_vertex_pair(Vertex u, Vertex v, Graph& g,
        typename std::list< typename graph_traits< Graph >::edge_descriptor >::iterator itu,
        typename std::list< typename graph_traits< Graph >::edge_descriptor >::iterator itv)
    {
        typedef typename graph_traits< Graph >::edge_descriptor edge_t;
        std::pair< edge_t, bool > return_value = add_edge(u, v, g);
        if (return_value.second) {
            put(m_em, return_value.first, m_next_index++);

            (*m_E)[u].insert(itu, return_value.first);
            (*m_E)[v].insert(itv, return_value.first);
        }
    }

    template < typename Graph, typename Vertex >
    void visit_vertex_pair(Vertex u, Vertex v, Graph& g,
        typename std::list< typename graph_traits< Graph >::edge_descriptor >::iterator itu)
    {
        visit_vertex_pair(u, v, g, itu, (*m_E)[v].end());
    }

    template < typename Graph, typename Vertex >
    void visit_vertex_pair(Vertex u, Vertex v, Graph& g)
    {
        visit_vertex_pair(u, v, g, (*m_E)[u].end());
    }

private:
    EdgeIndexMap m_em;
    edge_index_value_t m_next_index;
    Embedding *m_E;
};

template< typename graph, typename Embedding >
void print_embedding(graph& g, Embedding *E, std::ostream& os = std::cout)
{
    for(int i=0; i<E->size(); ++i)
    {
        os << i << ": ";
        typedef typename graph_traits< graph >::edge_descriptor edge_descriptor;
        edge_descriptor e;
        BOOST_FOREACH(e, (*E)[i]) {
            os << source(e, g) << "--" << target(e, g) << " ";
        }
        os << std::endl;
    }
}

/*
https://www.researchgate.net/publication/360912158_A_simple_linear_time_algorithm_for_embedding_maximal_planar_graphs_1

"simple_maximal_planar_random_graph()" implements only type==3 of chapter 8.
With type==4 and type==5 added it would be full blown "maximal_planar_random_graph()".


     triangular face T[t] transformation, t chosen randomly:

            T[t][0]                    T[t][0]
         +--#--+                    +--#--+
         |     |                    |  |  |
         |     |                    |  |  |
         |     |        ==>         |  i  |
         |     |                    | / \ |
         |     |                    |/   \|
         #-----#                    #-----#
   T[t][1]     T[t][2]        T[t][1]     T[t][2]
*/
template< typename graph, typename Embedding >
void simple_maximal_planar_random_embedding(graph& g, Embedding *E, int n)
{
    BOOST_ASSERT(n > 2);

    g.clear();
    for(int i=0; i<n; ++i)  (*E)[i].clear();

    typedef typename graph::vertex_descriptor vertex_descriptor;
    typedef typename graph::edge_descriptor edge_descriptor;
    typedef std::list< edge_descriptor > list_t;
    typedef typename list_t::iterator list_iterator;
    typedef std::pair< vertex_descriptor, list_iterator > vertex_t;
    typedef std::vector< vertex_t > vec_vertex_t;

    std::vector< vertex_descriptor > V;  V.reserve(n);
    emb_edge_index_update_visitor vis(get(edge_index, g), 0, E);

    V[0] = add_vertex(g);
    V[1] = add_vertex(g);
    V[2] = add_vertex(g);

    std::vector< vec_vertex_t > T;
    // start with inside and outside face of complete graph on 3 vertices
    T.reserve(2*n-4);
    T.push_back(vec_vertex_t()); T[0].reserve(3);
    T.push_back(vec_vertex_t()); T[1].reserve(3);

    vis.visit_vertex_pair(V[0], V[1], g);
    T[0].push_back( { V[0], std::prev((*E)[0].end()) } );
    list_iterator e1 = std::prev((*E)[1].end());

    vis.visit_vertex_pair(V[1], V[2], g);
    T[0].push_back( { V[1], std::prev((*E)[1].end()) } );
    T[1].push_back( { V[2], std::prev((*E)[2].end()) } );

    vis.visit_vertex_pair(V[2], V[0], g);
    T[0].push_back( { V[2], std::prev((*E)[2].end()) } );
    T[1].push_back( { V[1], e1 } );
    T[1].push_back( { V[0], std::prev((*E)[0].end()) } );

    for (int i=3; i < n; ++i)
    {
        unsigned t = random() % T.size();
        V[i] = add_vertex(g);

        vertex_t f = T[t][2];
        vis.visit_vertex_pair(V[f.first], V[i], g, f.second);
        vertex_t a2 = { V[f.first], std::prev(f.second) };
        vertex_t b2 = { V[i], std::prev((*E)[i].end()) };

        f = T[t][1];
        vis.visit_vertex_pair(V[f.first], V[i], g, f.second);
        vertex_t a1 = { V[f.first], std::prev(f.second) };
        vertex_t b1 = { V[i], std::prev((*E)[i].end()) };

        f = T[t][0];
        vis.visit_vertex_pair(V[f.first], V[i], g, f.second);
        vertex_t a0 = { V[f.first], std::prev(f.second) };
        vertex_t b0 = { V[i], std::prev((*E)[i].end()) };

        T.push_back(T[t]); T.back()[0] = a0; T.back()[1] = b2;
        T.push_back(T[t]); T.back()[1] = a1; T.back()[2] = b0;
                               T[t][2] = a2;     T[t][0] = b1;
    }
}

} // namespace boost

#endif //__EMBEDDINGS_HPP__
