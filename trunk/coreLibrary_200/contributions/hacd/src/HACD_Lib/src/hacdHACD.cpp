/* Copyright (c) 2011 Khaled Mamou (kmamou at gmail dot com)
 All rights reserved.
 
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#define _CRT_SECURE_NO_WARNINGS
#include <sstream>
#include <hacdGraph.h>
#include <hacdHACD.h>
#include <hacdICHull.h>
#include <string.h>
namespace HACD
{ 
	double  HACD::Concavity(ICHull & ch, std::map<long, DPoint> & distPoints)
    {
		TMMesh & meshCH = ch.GetMesh();
		double concavity = 0.0;
		double distance = 0.0;       
		std::map<long, DPoint>::iterator itDP(distPoints.begin());
		std::map<long, DPoint>::iterator itDPEnd(distPoints.end());
		std::map<long, DPoint>::iterator itDP1;
//        const double EPS = 0.1;
//        const Vec3<double> e[3] = {Vec3<double>(EPS, 0.0, 0.0), Vec3<double>(0.0, EPS, 0.0), Vec3<double>(0.0, 0.0, EPS) };
//        Vec3<double> normal1;
		for(; itDP != itDPEnd; ++itDP) 
		{
            if (!(itDP->second).m_computed)
            {
                (itDP->second).m_dist = meshCH.ComputeDistance(itDP->first, m_points[itDP->first], m_normals[itDP->first],
                                                               (itDP->second).m_computed, true);
/*
                for(int k = 0; k < 3 && !(itDP->second).m_computed; k++)
                {
                    normal1 = m_normals[itDP->first] + e[k];
                    normal1.Normalize();
                    (itDP->second).m_dist = meshCH.ComputeDistance(itDP->first, m_points[itDP->first], normal1,
                                                                   (itDP->second).m_computed, true);
                }
 */
            }
			distance = (itDP->second).m_dist;
			concavity = std::max<double>(concavity, distance);
		}
		return concavity;
    }

	void HACD::ComputeV2T()
	{
		m_v2T.clear();
		m_v2T.resize(m_nPoints);
		for(size_t t = 0; t < m_nTriangles; ++t)
		{
			m_v2T[m_triangles[t].X()].insert(t);
			m_v2T[m_triangles[t].Y()].insert(t);
			m_v2T[m_triangles[t].Z()].insert(t);
		}
	}
    void HACD::CreateGraph(bool connectCCs)
    {
		ComputeV2T();
		m_graph.Clear();
		m_graph.Allocate(m_nTriangles, 3 * m_nTriangles);
		unsigned long long tr1[3];
		unsigned long long tr2[3];
        long i1, j1, k1, i2, j2, k2;
        for (size_t v = 0; v < m_nPoints; v++) 
		{
			std::set<long>::const_iterator it1(m_v2T[v].begin()), itEnd(m_v2T[v].end()); 
			for(; it1 != itEnd; ++it1)
			{
                i1 = m_triangles[*it1].X();
                j1 = m_triangles[*it1].Y();
                k1 = m_triangles[*it1].Z();
				tr1[0] = GetEdgeIndex(i1, j1);
				tr1[1] = GetEdgeIndex(j1, k1);
				tr1[2] = GetEdgeIndex(k1, i1);
				std::set<long>::const_iterator it2(it1);
				for(++it2; it2 != itEnd; ++it2)
				{
                    i2 = m_triangles[*it2].X();
                    j2 = m_triangles[*it2].Y();
                    k2 = m_triangles[*it2].Z();
					tr2[0] = GetEdgeIndex(i2, j2);
					tr2[1] = GetEdgeIndex(j2, k2);
					tr2[2] = GetEdgeIndex(k2, i2);
					int shared = 0;
					for(int i = 0; i < 3; ++i)
					{
						for(int j = 0; j < 3; ++j)
						{
							if (tr1[i] == tr2[j])	
							{
								shared++;
							}
						}
					}
					if (shared == 1) // two triangles are connected if they share exactly one edge
					{
						m_graph.AddEdge(*it1, *it2);
					}
				}
			}
        }
		// free memory;
		m_v2T = std::vector< std::set<long> >();
        
        if (connectCCs) 
		{
			m_graph.ConnectCCs();
		}
    }
    void HACD::InitializeGraph()
    {
        long i, j, k;
        Vec3<double> u, v, w, normal;
		delete [] m_normals;
		m_normals = new Vec3<double>[m_nPoints];

		memset(m_normals, 0, sizeof(Vec3<double>) * m_nPoints);
        for(unsigned long f = 0; f < m_nTriangles; f++)
        {
            i = m_triangles[f].X();
            j = m_triangles[f].Y();
            k = m_triangles[f].Z();
		
			m_graph.m_vertices[f].m_distPoints[i].m_distOnly = false;
			m_graph.m_vertices[f].m_distPoints[j].m_distOnly = false;
			m_graph.m_vertices[f].m_distPoints[k].m_distOnly = false;
            
            ICHull  * ch = new ICHull;
            m_graph.m_vertices[f].m_convexHull = ch;
            ch->AddPoint(m_points[i], i);
            ch->AddPoint(m_points[j], j);
            ch->AddPoint(m_points[k], k);
			ch->SetDistPoints(&m_graph.m_vertices[f].m_distPoints);

			u = m_points[j] - m_points[i];
			v = m_points[k] - m_points[i];
			w = m_points[k] - m_points[j];
			normal = u ^ v;

			m_normals[i] += normal;
			m_normals[j] += normal;
			m_normals[k] += normal;

			m_graph.m_vertices[f].m_surf = normal.GetNorm();
			m_graph.m_vertices[f].m_perimeter = u.GetNorm() + v.GetNorm() + w.GetNorm();

			m_graph.m_vertices[f].m_boudaryEdges.insert(GetEdgeIndex(i,j));
			m_graph.m_vertices[f].m_boudaryEdges.insert(GetEdgeIndex(j,k));
			m_graph.m_vertices[f].m_boudaryEdges.insert(GetEdgeIndex(k,i));
            if (m_addExteraDistPoints)	
            {// we need a kd-tree structure to accelerate this part!
                long i1, j1, k1;
                Vec3<double> u1, v1, normal1;
                double distance;
                double distMin;
                size_t faceIndex = m_nTriangles;
                Vec3<double> seedPoint((m_points[i] + m_points[i] + m_points[k]) / 3.0);
                long nhit = 0;
                for(size_t f1 = 0; f1 < m_nTriangles; f1++)
                {
                    i1 = m_triangles[f1].X();
                    j1 = m_triangles[f1].Y();
                    k1 = m_triangles[f1].Z();
                    u1 = m_points[j1] - m_points[i1];
                    v1 = m_points[k1] - m_points[i1];
                    normal1 = (u1 ^ v1);
                    if (normal * normal1 < 0.0)
                    {
                        nhit = IntersectRayTriangle(seedPoint, -normal, m_points[i1], m_points[j1], m_points[k1], distance);
                        if ((nhit==1) && (distance >0.0) && ((distMin > distance) || (faceIndex == m_nTriangles)))
                        {
                            distMin = distance;
                            faceIndex = f1;
                        }

                    }
                }
                if (faceIndex < m_nTriangles )
                {
                    i1 = m_triangles[faceIndex].X();
                    j1 = m_triangles[faceIndex].Y();
                    k1 = m_triangles[faceIndex].Z();
                    m_graph.m_vertices[f].m_distPoints[i1].m_distOnly = true;
                    m_graph.m_vertices[f].m_distPoints[j1].m_distOnly = true;
                    m_graph.m_vertices[f].m_distPoints[k1].m_distOnly = true;
				}
            }
        }
        for (size_t v = 0; v < m_nPoints; v++) 
		{
			m_normals[v].Normalize();
		}
    }

	void HACD::NormalizeData()
	{
		if (m_nPoints == 0)
		{
			return;
		}
        m_barycenter = m_points[0];
		Vec3<double> min = m_points[0];
		Vec3<double> max = m_points[0];
		double x, y, z;
        for (size_t v = 1; v < m_nPoints ; v++) 
        {
			m_barycenter += m_points[v];
            x = m_points[v].X();
            y = m_points[v].Y();
            z = m_points[v].Z();
            if ( x < min.X()) min.X() = x;
			else if ( x > max.X()) max.X() = x;
            if ( y < min.Y()) min.Y() = y;
			else if ( y > max.Y()) max.Y() = y;
            if ( z < min.Z()) min.Z() = z;
			else if ( z > max.Z()) max.Z() = z;
        }
		m_barycenter /= m_nPoints;
        m_diag = (max-min).GetNorm();
        const double invDiag = 2.0 * m_scale / m_diag;
		if (m_diag != 0.0)
		{
			for (size_t v = 0; v < m_nPoints ; v++) 
			{
				m_points[v] = (m_points[v] - m_barycenter) * invDiag;
			}
		}
    }
	void HACD::DenormalizeData()
	{
		if (m_nPoints == 0)
		{
			return;
		}
		if (m_diag != 0.0)
		{
			const double diag = m_diag / (2.0 * m_scale);
			for (size_t v = 0; v < m_nPoints ; v++) 
			{
				m_points[v] = m_points[v] * diag + m_barycenter;
			}
		}
    }
	HACD::HACD(void)
	{
        m_convexHulls = 0;
		m_triangles = 0;
        m_points = 0;
        m_normals = 0;
        m_nTriangles = 0;
        m_nPoints = 0;
        m_nClusters = 0;
        m_concavity = 0.0;
        m_diag = 1.0;
		m_barycenter = Vec3<double>(0.0, 0.0,0.0);
        m_alpha = 0.1;
        m_beta = 0.1;
        m_nVerticesPerCH = 30;
		m_callBack = 0;
        m_addExteraDistPoints = false;
		m_scale = 1000.0;
	}																
	HACD::~HACD(void)
	{
		delete [] m_normals;
        delete [] m_convexHulls;
	}
    void HACD::ComputeEdgeCost(size_t e)
    {
		GraphEdge & gE = m_graph.m_edges[e];
        long v1 = gE.m_v1;
        long v2 = gE.m_v2;

        if (m_graph.m_vertices[v2].m_distPoints.size()>m_graph.m_vertices[v1].m_distPoints.size())
        {
            gE.m_v1 = v2;
            gE.m_v2 = v1;
            std::swap<long>(v1, v2);
        }
		GraphVertex & gV1 = m_graph.m_vertices[v1];
		GraphVertex & gV2 = m_graph.m_vertices[v2];
	
        // delete old convex-hull
        delete gE.m_convexHull;
        // create the edge's convex-hull
        ICHull  * ch = new ICHull;
        gE.m_convexHull = ch;
        (*ch) = (*gV1.m_convexHull);
        
		// update distPoints
		gE.m_distPoints = gV1.m_distPoints;
		std::map<long, DPoint>::iterator itDP(gV2.m_distPoints.begin());
		std::map<long, DPoint>::iterator itDPEnd(gV2.m_distPoints.end());
		std::map<long, DPoint>::iterator itDP1;
     
		for(; itDP != itDPEnd; ++itDP) 
		{
			itDP1 = gE.m_distPoints.find(itDP->first);
			if (itDP1 == gE.m_distPoints.end())
			{
                gE.m_distPoints[itDP->first].m_distOnly = (itDP->second).m_distOnly;
                if ( !(itDP->second).m_distOnly )
                {
                    ch->AddPoint(m_points[itDP->first], itDP->first);
                }
			}
            else
            {
                if ( (itDP1->second).m_distOnly && !(itDP->second).m_distOnly)
                {
                    gE.m_distPoints[itDP->first].m_distOnly = false;
                    ch->AddPoint(m_points[itDP->first], itDP->first);
                }
            }
		}

		ch->SetDistPoints(&gE.m_distPoints);

        // create the convex-hull

			
        while (!ch->Process())		// if we face problems when constructing the visual-hull. really ugly!!!!
		{
//			if (m_callBack) (*m_callBack)("\t Problem with convex-hull construction [HACD::ComputeEdgeCost]\n", 0.0, 0.0, 0);
			ch = new ICHull;
			CircularList<TMMVertex> & verticesCH = (gE.m_convexHull)->GetMesh().m_vertices;
			size_t nV = verticesCH.GetSize();
			long ptIndex = 0;
			verticesCH.Next();
			for(size_t v = 1; v < nV; ++v)
			{
				ptIndex = verticesCH.GetHead()->GetData().m_name;
				ch->AddPoint(m_points[ptIndex], ptIndex);
				verticesCH.Next();
			}
			delete gE.m_convexHull;
			gE.m_convexHull = ch;
		}

		// compute boudary edges
        double perimeter = 0.0;
		gE.m_boudaryEdges.clear();
		std::set_symmetric_difference (gV1.m_boudaryEdges.begin(), 
							  gV1.m_boudaryEdges.end(), 
							  gV2.m_boudaryEdges.begin(), 
							  gV2.m_boudaryEdges.end(),
							  std::inserter( gE.m_boudaryEdges, 
											 gE.m_boudaryEdges.begin() ) );

		std::set<unsigned long long>::const_iterator itBE(gE.m_boudaryEdges.begin());
		std::set<unsigned long long>::const_iterator itBEEnd(gE.m_boudaryEdges.end());
		for(; itBE != itBEEnd; ++itBE)
		{
                perimeter += (m_points[static_cast<long>((*itBE) >> 32)] - 
							   m_points[static_cast<long>((*itBE) & 0xFFFFFFFFULL)]).GetNorm();
		}
        double surf      = gV1.m_surf + gV2.m_surf;
        double ratio     = perimeter * perimeter / (4.0 * sc_pi * surf);
        double concavity = Concavity(*ch, gE.m_distPoints);
        double volume = ch->ComputeVolume() / pow(m_scale, 3.0);
        
        gE.m_surf      = surf;                          // clusterSurf  
        gE.m_perimeter = perimeter;                     // recompute the perimeter
        gE.m_concavity = concavity;                     // concavity
        gE.m_error     = concavity +  m_alpha * ratio + m_beta * volume;	// priority 
    }
    bool HACD::InitializePQ()
    {
        for (size_t e=0; e < m_graph.m_nE; ++e) 
        {
            ComputeEdgeCost(e);
            m_pqueue.push(GraphEdgePQ(e, m_graph.m_edges[e].m_error));
        }
		return true;
    }
    bool HACD::Compute(bool connectCCs, bool fullCH, bool exportDistPoints)
    {
		if ( !m_points || !m_triangles || !m_nPoints || !m_nTriangles)
		{
			return false;
		}
		size_t nV = m_nTriangles;
		if (m_callBack)
		{
			std::ostringstream msg;
			msg << "+ Mesh" << std::endl;
			msg << "\t # vertices                \t" << m_nPoints << std::endl;
			msg << "\t # triangles               \t" << m_nTriangles << std::endl;
			msg << "+ Parameters" << std::endl;
			msg << "\t min # of clusters         \t" << m_nClusters << std::endl;
			msg << "\t max concavity             \t" << m_concavity << std::endl;
			msg << "\t compacity weigth          \t" << m_alpha << std::endl;
            msg << "\t volume weigth             \t" << m_beta << std::endl;
			msg << "\t # vertices per convex-hull\t" << m_nVerticesPerCH << std::endl;
			msg << "\t Scale                     \t" << m_scale << std::endl;
			msg << "\t Add extra distance points \t" << m_addExteraDistPoints << std::endl;
			msg << "\t Connect CCs               \t" << connectCCs << std::endl;
			msg << "\t Produce full convex-hulls \t" << fullCH << std::endl;		
			(*m_callBack)(msg.str().c_str(), 0.0, 0.0, nV);
		}
		if (m_callBack) (*m_callBack)("+ Normalizing Data\n", 0.0, 0.0, nV);
		NormalizeData();
		if (m_callBack) (*m_callBack)("+ Creating Graph\n", 0.0, 0.0, nV);
		CreateGraph(connectCCs);
        // Compute the surfaces and perimeters of all the faces
		if (m_callBack) (*m_callBack)("+ Initializing Graph\n", 0.0, 0.0, nV);
		InitializeGraph();
		if (m_callBack) (*m_callBack)("+ Initializing PQ\n", 0.0, 0.0, nV);
        InitializePQ();
        
        //--- we simplify the stuff
		
        long v1 = -1;
        long v2 = -1;
        
        double progressOld = -1.0;
        double progress = 0.0;
		if (m_callBack) (*m_callBack)("+ Simplification ...\n", 0.0, 0.0, nV);

        double globalConcavity  = 0.0;     
		char msg[1024];
        while ( (globalConcavity < m_concavity) && 
				(m_graph.GetNVertices() > m_nClusters) && 
				(m_graph.GetNEdges()> 1)) 
		{
            progress = 100.0-m_graph.GetNVertices() * 100.0 / nV;
            if (fabs(progress-progressOld) > 0.1 && m_callBack)
            {
				sprintf(msg, "%3.2f %% V = %lu \t C = %f \t \t \r", progress, m_graph.GetNVertices(), globalConcavity);
				(*m_callBack)(msg, progress, globalConcavity,  m_graph.GetNVertices());
                progressOld = progress;
            }

			GraphEdgePQ currentEdge(0,0.0);
			bool done = false;
			do
			{
				done = false;
				if (m_pqueue.size() == 0)
				{
					done = true;
					break;
				}
				else
				{
					currentEdge = m_pqueue.top();
					m_pqueue.pop();
				}
			}
			while (  m_graph.m_edges[currentEdge.m_name].m_deleted || 
					 m_graph.m_edges[currentEdge.m_name].m_error != currentEdge.m_priority);

			

			if (m_graph.m_edges[currentEdge.m_name].m_concavity < m_concavity && !done)
			{
                globalConcavity = std::max<double>(globalConcavity ,m_graph.m_edges[currentEdge.m_name].m_concavity);
				v1 = m_graph.m_edges[currentEdge.m_name].m_v1;
				v2 = m_graph.m_edges[currentEdge.m_name].m_v2;	
				// update vertex info
				m_graph.m_vertices[v1].m_error     = m_graph.m_edges[currentEdge.m_name].m_error;
				m_graph.m_vertices[v1].m_surf	   = m_graph.m_edges[currentEdge.m_name].m_surf;
				m_graph.m_vertices[v1].m_concavity = m_graph.m_edges[currentEdge.m_name].m_concavity;
				m_graph.m_vertices[v1].m_perimeter = m_graph.m_edges[currentEdge.m_name].m_perimeter;
                m_graph.m_vertices[v1].m_distPoints   = m_graph.m_edges[currentEdge.m_name].m_distPoints;
                (*m_graph.m_vertices[v1].m_convexHull) = (*m_graph.m_edges[currentEdge.m_name].m_convexHull);
				(m_graph.m_vertices[v1].m_convexHull)->SetDistPoints(&(m_graph.m_vertices[v1].m_distPoints));
				m_graph.m_vertices[v1].m_boudaryEdges   = m_graph.m_edges[currentEdge.m_name].m_boudaryEdges;
				
				// We apply the optimal ecol
				m_graph.EdgeCollapse(v1, v2);
				// recompute the adjacent edges costs
				std::set<long>::const_iterator itE(m_graph.m_vertices[v1].m_edges.begin()), 
								   			   itEEnd(m_graph.m_vertices[v1].m_edges.end());
				for(; itE != itEEnd; ++itE)
				{
					size_t e = *itE;
					ComputeEdgeCost(e);
					m_pqueue.push(GraphEdgePQ(e, m_graph.m_edges[e].m_error));
				}
			}
            else
            {
                break;
            }
        }
        m_nClusters = m_graph.GetNVertices();
		if (m_callBack) (*m_callBack)("+ Denormalizing Data\n", 0.0, 0.0, m_nClusters);
		DenormalizeData();

		if (m_callBack) (*m_callBack)("+ Computing final convex-hulls\n", 0.0, 0.0, m_nClusters);
        delete [] m_convexHulls;
        m_convexHulls = new ICHull[m_nClusters];
        m_cVertices.clear();
        m_cVertices.reserve(m_nClusters);
        size_t p=0;
        for (size_t v = 0; v != m_graph.m_vertices.size(); ++v) 
		{
			if (!m_graph.m_vertices[v].m_deleted)
			{
                if (m_callBack) 
                {
                    char msg[1024];
                    sprintf(msg, "\t CH \t %lu \t %lf \t %lf\n", p, m_graph.m_vertices[v].m_concavity, m_graph.m_vertices[v].m_error);
                    (*m_callBack)(msg, 0.0, 0.0, m_nClusters);
                }
                m_cVertices.push_back(v);			
                // compute the convex-hull
                const std::map<long, DPoint> & pointsCH =  m_graph.m_vertices[v].m_distPoints;
                std::map<long, DPoint>::const_iterator itCH(pointsCH.begin());
                std::map<long, DPoint>::const_iterator itCHEnd(pointsCH.end());
                for(; itCH != itCHEnd; ++itCH) 
                {
                    if (!(itCH->second).m_distOnly)
                    {
                        m_convexHulls[p].AddPoint(m_points[itCH->first], itCH->first);
                    }
                }
                if (fullCH)
                {
                    m_convexHulls[p].Process();
                }
                else
                {
                    m_convexHulls[p].Process(m_nVerticesPerCH);
                }
                if (exportDistPoints)
                {
                    itCH = pointsCH.begin();
                    for(; itCH != itCHEnd; ++itCH) 
                    {
                        if ((itCH->second).m_distOnly)
                        {
                            m_convexHulls[p].AddPoint(m_points[itCH->first], itCH->first);
                        }
                    }
                }
                p++;
			}
		}

        //CleanClusters(fullCH);
        if (m_callBack)
        {
			sprintf(msg, "# clusters =  %lu \t C = %f\n", m_nClusters, globalConcavity);
			(*m_callBack)(msg, progress, globalConcavity,  m_graph.GetNVertices());
        }
        return true;
    }
    void HACD::CleanClusters(bool fullCH)
    {
        if (m_nClusters < 1)
        {
            return;
        }
        std::vector<long>::const_iterator it;
        long * partition = new long [m_nTriangles];
        memset(partition, sizeof(long) * m_nTriangles, 0); 
        long v;
        long i, j, k;
        m_nClusters = 0;
        for (size_t p = 0; p != m_cVertices.size(); ++p) 
		{
            bool empty = true;
            v = m_cVertices[0];
            m_graph.m_vertices[v].m_ancestors.push_back(v);
            for (it =  m_graph.m_vertices[v].m_ancestors.begin();
                 it != m_graph.m_vertices[v].m_ancestors.end(); ++it) 
            {
                if (partition[*it] == 0)
                {
                    partition[*it] = p+1;
                    empty = false;
                }
            }
            
            for (size_t p1 = p+1; p1 != m_cVertices.size(); ++p1) 
            {

                if (1/* intersection between CH(p) and CH(p1)*/)
                {
                    v = m_cVertices[p1];
                    m_graph.m_vertices[v].m_ancestors.push_back(v);
                    for (it =  m_graph.m_vertices[v].m_ancestors.begin();
                         it != m_graph.m_vertices[v].m_ancestors.end(); ++it) 
                    {
                        if (partition[*it] == 0)
                        {
                            i = m_triangles[*it].X();
                            j = m_triangles[*it].Y();
                            k = m_triangles[*it].Z();
                            if (m_convexHulls[p].IsInside(m_points[i]) &&
                                m_convexHulls[p].IsInside(m_points[j]) &&
                                m_convexHulls[p].IsInside(m_points[k])   ) //if triangle *it inside CH(p) add it to CH(p)
                            {
                                partition[*it] = p+1;
                                empty = false;
                            }
                        }
                    }
                }
            }
            if (!empty)
            {
                m_nClusters++;
            }
        }
        delete [] m_convexHulls;
        m_graph.Clear();
        std::set<long> * c2v = new std::set<long>[m_nClusters];
        for(size_t t = 0; t < m_nTriangles; ++t)
        {
            i = m_triangles[*it].X();
            j = m_triangles[*it].Y();
            k = m_triangles[*it].Z();
            c2v[partition[t]-1].insert(i);
            c2v[partition[t]-1].insert(j);
            c2v[partition[t]-1].insert(k);
        }
        m_convexHulls = new ICHull[m_nClusters];
        for(size_t p = 0; p < m_nClusters; ++p)
        {
            std::set<long>::const_iterator itP(c2v[p].begin()), 
                                           itPEnd(c2v[p].end());
            for(; itP != itPEnd; ++itP)
            {
                m_convexHulls[p].AddPoint(m_points[*itP]);
            }
            if (fullCH)
            {
                m_convexHulls[p].Process();
            }
            else
            {
                m_convexHulls[p].Process(m_nVerticesPerCH);
            }
        }
        
        delete [] partition;
        delete [] c2v;
    }
    void HACD::ComputePartition(long * partition) const
    {
        // we fill the partition       
        if (m_callBack) (*m_callBack)("+ Filling partition...\n", 0.0, 0.0, m_graph.GetNVertices());
		std::vector<long>::const_iterator it;
        size_t v;
        for (size_t p = 0; p != m_cVertices.size(); ++p) 
		{
            v = m_cVertices[p];
            partition[v] = static_cast<long>(p);            
            for (it =  m_graph.m_vertices[v].m_ancestors.begin();
                 it != m_graph.m_vertices[v].m_ancestors.end(); ++it) 
            {
                partition[*it] = static_cast<long>(p);
            }
        }
    }
    size_t HACD::GetNPointsCH(size_t numCH) const
    {
        if (numCH >= m_nClusters)
        {
            return 0;
        }
        return m_convexHulls[numCH].GetMesh().GetNVertices();
    }
    size_t HACD::GetNTrianglesCH(size_t numCH) const
    {
        if (numCH >= m_nClusters)
        {
            return 0;
        }
        return m_convexHulls[numCH].GetMesh().GetNTriangles();
    }
    bool HACD::GetCH(size_t numCH, Vec3<double> * const points, Vec3<long> * const triangles)
    {
        if (numCH >= m_nClusters)
        {
            return false;
        }
        m_convexHulls[numCH].GetMesh().GetIFS(points, triangles);
        return true;
    }

    bool HACD::Save(const char * fileName, bool uniColor, long numCluster) const
    {
        std::ofstream fout(fileName);
        if (fout.is_open())
        {
            if (m_callBack)
            {
                char msg[1024];
                sprintf(msg, "Saving %s\n", fileName);
                (*m_callBack)(msg, 0.0, 0.0, m_graph.GetNVertices());
            }
            Material mat;
            if (numCluster < 0)
            {
                for (size_t p = 0; p != m_cVertices.size(); ++p) 
                {
                    if (!uniColor)
                    {
                        mat.m_diffuseColor.X() = mat.m_diffuseColor.Y() = mat.m_diffuseColor.Z() = 0.0;
                        while (mat.m_diffuseColor.X() == mat.m_diffuseColor.Y() ||
                               mat.m_diffuseColor.Z() == mat.m_diffuseColor.Y() ||
                               mat.m_diffuseColor.Z() == mat.m_diffuseColor.X()  )
                        {
                            mat.m_diffuseColor.X() = (rand()%100) / 100.0;
                            mat.m_diffuseColor.Y() = (rand()%100) / 100.0;
                            mat.m_diffuseColor.Z() = (rand()%100) / 100.0;
                        }
                    }
                    m_convexHulls[p].GetMesh().SaveVRML2(fout, mat);
                }
            }
            else if (numCluster < static_cast<long>(m_cVertices.size()))
            {
                m_convexHulls[numCluster].GetMesh().SaveVRML2(fout, mat);
            }
            fout.close();
            return true;
        }
        else
        {
            if (m_callBack)
            {
                char msg[1024];
                sprintf(msg, "Error saving %s\n", fileName);
                (*m_callBack)(msg, 0.0, 0.0, m_graph.GetNVertices());
            }
            return false;
        }
    }
}