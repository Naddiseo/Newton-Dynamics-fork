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
//	int debug = 0;
	double  HACD::Concavity(ICHull & ch, std::map<long, DPoint> & distPoints)
    {
		double concavity = 0.0;
		double distance = 0.0;       
		std::map<long, DPoint>::iterator itDP(distPoints.begin());
		std::map<long, DPoint>::iterator itDPEnd(distPoints.end());
		for(; itDP != itDPEnd; ++itDP) 
		{
            if (!(itDP->second).m_computed)
            {
                if (itDP->first >= 0)
                {
                    ch.ComputeDistance(itDP->first, m_points[itDP->first], m_normals[itDP->first], (itDP->second).m_computed, true, false);
                }
                else
                {
                     ch.ComputeDistance(itDP->first, m_facePoints[-itDP->first-1], m_faceNormals[-itDP->first-1], (itDP->second).m_computed, true, false);
                }
            }
			distance = (itDP->second).m_dist;
//			if (debug) std::cout << itDP->first << " " << distance << std::endl;
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
		m_t2T.resize(m_nTriangles);
		m_graph.Clear();
		m_graph.Allocate(m_nTriangles, 3 * m_nTriangles);
		unsigned long long tr1[3];
		unsigned long long tr2[3];
        long i1, j1, k1, i2, j2, k2;
        long t1, t2;
        for (size_t v = 0; v < m_nPoints; v++) 
		{
			std::set<long>::const_iterator it1(m_v2T[v].begin()), itEnd(m_v2T[v].end()); 
			for(; it1 != itEnd; ++it1)
			{
                t1 = *it1;
                i1 = m_triangles[t1].X();
                j1 = m_triangles[t1].Y();
                k1 = m_triangles[t1].Z();
				tr1[0] = GetEdgeIndex(i1, j1);
				tr1[1] = GetEdgeIndex(j1, k1);
				tr1[2] = GetEdgeIndex(k1, i1);
				std::set<long>::const_iterator it2(it1);
				for(++it2; it2 != itEnd; ++it2)
				{
                    t2 = *it2;
                    i2 = m_triangles[t2].X();
                    j2 = m_triangles[t2].Y();
                    k2 = m_triangles[t2].Z();
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
						m_graph.AddEdge(t1, t2);
						m_t2T[t1].insert(t2);
						m_t2T[t2].insert(t1);
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
//		if (m_addExtraDistPoints)
//		{
//			m_distPointsMapping.resize(m_nTriangles, -1);
//		}
        if (m_addFacesPoints)
        {
            delete [] m_facePoints;
            delete [] m_faceNormals;
            m_facePoints = new Vec3<double>[m_nTriangles];
            m_faceNormals = new Vec3<double>[m_nTriangles];
        }
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
            ch->SetDistPointsPositions(m_points);
            ch->SetDistPointsNormals(m_normals);
            ch->SetFacePointsPositions(m_facePoints);
            ch->SetFacePointsNormals(m_faceNormals);

			u = m_points[j] - m_points[i];
			v = m_points[k] - m_points[i];
			w = m_points[k] - m_points[j];
			normal = u ^ v;

			m_normals[i] += normal;
			m_normals[j] += normal;
			m_normals[k] += normal;

			m_graph.m_vertices[f].m_surf = normal.GetNorm();
			m_graph.m_vertices[f].m_perimeter = u.GetNorm() + v.GetNorm() + w.GetNorm();
            
            normal.Normalize();

			m_graph.m_vertices[f].m_boudaryEdges.insert(GetEdgeIndex(i,j));
			m_graph.m_vertices[f].m_boudaryEdges.insert(GetEdgeIndex(j,k));
			m_graph.m_vertices[f].m_boudaryEdges.insert(GetEdgeIndex(k,i));
            if(m_addFacesPoints)
            {
                m_faceNormals[f] = normal;
                m_facePoints[f] = (m_points[i] + m_points[j] + m_points[k]) / 3.0;
                m_graph.m_vertices[f].m_distPoints[-f-1].m_distOnly = true;
                m_graph.m_vertices[f].m_distPoints[-f-1].m_distOnly = true;
                m_graph.m_vertices[f].m_distPoints[-f-1].m_distOnly = true;
            }
            if (m_addExtraDistPoints)	
            {// we need a kd-tree structure to accelerate this part!
                long i1, j1, k1;
                Vec3<double> u1, v1, normal1;
                double distance = 0.0;
                double distMin = 0.0;
                size_t faceIndex = m_nTriangles;
                Vec3<double> seedPoint((m_points[i] + m_points[j] + m_points[k]) / 3.0);
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
//					m_distPointsMapping[f] = faceIndex;
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
        m_addExtraDistPoints = false;
		m_addNeighboursDistPoints = false;
		m_scale = 1000.0;
		m_partition = 0;
		m_nMinClusters = 3;
        m_facePoints = 0;
        m_faceNormals = 0;
	}																
	HACD::~HACD(void)
	{
		delete [] m_normals;
        delete [] m_convexHulls;
		delete [] m_partition;
        delete [] m_facePoints;
        delete [] m_faceNormals;
	}
	int iteration = 0;
    bool HACD::ComputeEdgeCost(size_t e, bool fast)
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
        ch->SetDistPointsPositions(m_points);
        ch->SetDistPointsNormals(m_normals);
        ch->SetFacePointsPositions(m_facePoints);
        ch->SetFacePointsNormals(m_faceNormals);

        // create the convex-hull
        while (ch->Process() == ICHullErrorInconsistent)		// if we face problems when constructing the visual-hull. really ugly!!!!
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
		double volume = ch->ComputeVolume();
        double concavity = 0.0;
		if (volume == 0.0)
		{
			bool insideHull;
            std::map<long, DPoint>::iterator itDP(gE.m_distPoints.begin());
            std::map<long, DPoint>::iterator itDPEnd(gE.m_distPoints.end());
            for(; itDP != itDPEnd; ++itDP) 
            {	
                if (itDP->first >= 0)
                {
                    concavity = std::max<double>(concavity, ch->ComputeDistance(itDP->first, m_points[itDP->first], m_normals[itDP->first], insideHull, false, false));
                }
			}
		}
        else
        {
            if (m_addNeighboursDistPoints)
            {  // add distance points from adjacent clusters
                std::set<long> eEdges;
                std::set_union(gV1.m_edges.begin(), 
                               gV1.m_edges.end(), 
                               gV2.m_edges.begin(), 
                               gV2.m_edges.end(),
                               std::inserter( eEdges, eEdges.begin() ) );
                
                std::set<long>::const_iterator ed(eEdges.begin());
                std::set<long>::const_iterator itEnd(eEdges.end());
                long a, b, c;
                for(; ed != itEnd; ++ed) 
                {
                    a = m_graph.m_edges[*ed].m_v1;
                    b = m_graph.m_edges[*ed].m_v2;
                    if ( a != v2 && a != v1)
                    {
                        c = a;
                    }
                    else if ( b != v2 && b != v1)
                    {
                        c = b;
                    }
                    else
                    {
                        c = -1;
                    }
                    if ( c > 0)
                    {
                        GraphVertex & gVC = m_graph.m_vertices[c];
                        std::map<long, DPoint>::iterator itDP(gVC.m_distPoints.begin());
                        std::map<long, DPoint>::iterator itDPEnd(gVC.m_distPoints.end());
                        std::map<long, DPoint>::iterator itDP1;
                        for(; itDP != itDPEnd; ++itDP) 
                        {
                            itDP1 = gE.m_distPoints.find(itDP->first);
                            if (itDP->first >= 0 && itDP1 == gE.m_distPoints.end() && ch->IsInside(m_points[itDP->first]))
                            {
                                gE.m_distPoints[itDP->first].m_distOnly = true;
                            }
                        }
                    }
                }
            }
            concavity = Concavity(*ch, gE.m_distPoints);
            volume /= pow(m_scale, 3.0);
        }
        

		
        double perimeter = 0.0;
		double surf = 1.0;
		if (!fast)
		{
			// compute boudary edges
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
			surf      = gV1.m_surf + gV2.m_surf;
		}
        double ratio     = perimeter * perimeter / (4.0 * sc_pi * surf);
        gE.m_volume	   = volume;						// cluster's volume
        gE.m_surf      = surf;                          // cluster's area  
        gE.m_perimeter = perimeter;                     // cluster's perimeter
        gE.m_concavity = concavity;                     // cluster's concavity
        gE.m_error     = concavity +  m_alpha * ratio + m_beta * volume;	// cluster's priority
		return true;
	}
    bool HACD::InitializePQ(bool fast)
    {
        for (size_t e=0; e < m_graph.m_nE; ++e) 
        {
            if (ComputeEdgeCost(e, fast))
			{
				m_pqueue.push(GraphEdgePQ(e, m_graph.m_edges[e].m_error));
			}
        }
		return true;
    }
	void HACD::Simplify(bool fast)
	{
		long v1 = -1;
        long v2 = -1;        
        double progressOld = -1.0;
        double progress = 0.0;
		if (m_callBack) (*m_callBack)("+ Simplification ...\n", 0.0, 0.0, m_nTriangles);

        double globalConcavity  = 0.0;     
		char msg[1024];
        while ( (globalConcavity < m_concavity) && 
				(m_graph.GetNVertices() > m_nMinClusters) && 
				(m_graph.GetNEdges()> 1)) 
		{
            progress = 100.0-m_graph.GetNVertices() * 100.0 / m_nTriangles;
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
				m_graph.m_vertices[v1].m_volume	   = m_graph.m_edges[currentEdge.m_name].m_volume;
				m_graph.m_vertices[v1].m_concavity = m_graph.m_edges[currentEdge.m_name].m_concavity;
				m_graph.m_vertices[v1].m_perimeter = m_graph.m_edges[currentEdge.m_name].m_perimeter;
                m_graph.m_vertices[v1].m_distPoints   = m_graph.m_edges[currentEdge.m_name].m_distPoints;
                (*m_graph.m_vertices[v1].m_convexHull) = (*m_graph.m_edges[currentEdge.m_name].m_convexHull);
				(m_graph.m_vertices[v1].m_convexHull)->SetDistPoints(&(m_graph.m_vertices[v1].m_distPoints));
                (m_graph.m_vertices[v1].m_convexHull)->SetDistPointsPositions(m_points);
                (m_graph.m_vertices[v1].m_convexHull)->SetDistPointsNormals(m_normals);
                (m_graph.m_vertices[v1].m_convexHull)->SetFacePointsPositions(m_facePoints);
                (m_graph.m_vertices[v1].m_convexHull)->SetFacePointsNormals(m_faceNormals);
				m_graph.m_vertices[v1].m_boudaryEdges   = m_graph.m_edges[currentEdge.m_name].m_boudaryEdges;
				
				// We apply the optimal ecol
				m_graph.EdgeCollapse(v1, v2);
				// recompute the adjacent edges costs
				std::set<long>::const_iterator itE(m_graph.m_vertices[v1].m_edges.begin()), 
								   			   itEEnd(m_graph.m_vertices[v1].m_edges.end());
				for(; itE != itEEnd; ++itE)
				{
					size_t e = *itE;
					if (ComputeEdgeCost(e, fast))
					{
						m_pqueue.push(GraphEdgePQ(e, m_graph.m_edges[e].m_error));
					}
				}
			}
            else
            {
                break;
            }
        }
		while (!m_pqueue.empty())
		{
			m_pqueue.pop();
		}
		
        m_cVertices.clear();
        m_cVertices.reserve(m_nClusters);
		m_nClusters = m_graph.GetNVertices();
		for (size_t p=0, v = 0; v != m_graph.m_vertices.size(); ++v) 
		{
			if (!m_graph.m_vertices[v].m_deleted)
			{
                if (m_callBack) 
                {
                    char msg[1024];
                    sprintf(msg, "\t CH \t %lu \t %lf \t %lf\n", p, m_graph.m_vertices[v].m_concavity, m_graph.m_vertices[v].m_error);
					(*m_callBack)(msg, 0.0, 0.0, m_nClusters);
					p++;
                }
                m_cVertices.push_back(v);			
			}
		}
        if (m_callBack)
        {
			sprintf(msg, "# clusters =  %lu \t C = %f\n", m_nClusters, globalConcavity);
			(*m_callBack)(msg, progress, globalConcavity,  m_graph.GetNVertices());
        }

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
			msg << "\t # vertices                     \t" << m_nPoints << std::endl;
			msg << "\t # triangles                    \t" << m_nTriangles << std::endl;
			msg << "+ Parameters" << std::endl;
			msg << "\t min # of clusters              \t" << m_nClusters << std::endl;
			msg << "\t max concavity                  \t" << m_concavity << std::endl;
			msg << "\t compacity weigth               \t" << m_alpha << std::endl;
            msg << "\t volume weigth                  \t" << m_beta << std::endl;
			msg << "\t # vertices per convex-hull     \t" << m_nVerticesPerCH << std::endl;
			msg << "\t Scale                          \t" << m_scale << std::endl;
			msg << "\t Add extra distance points      \t" << m_addExtraDistPoints << std::endl;
            msg << "\t Add neighbours distance points \t" << m_addNeighboursDistPoints << std::endl;
            msg << "\t Add face distance points       \t" << m_addFacesPoints << std::endl;
			msg << "\t Connect CCs                    \t" << connectCCs << std::endl;
			msg << "\t Produce full convex-hulls      \t" << fullCH << std::endl;		
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
        InitializePQ(false);
        
        // we simplify the stuff		
		Simplify(false);
//		CleanClusters();


		if (m_callBack) (*m_callBack)("+ Denormalizing Data\n", 0.0, 0.0, m_nClusters);
		DenormalizeData();

		if (m_callBack) (*m_callBack)("+ Computing final convex-hulls\n", 0.0, 0.0, m_nClusters);
        delete [] m_convexHulls;
        m_convexHulls = new ICHull[m_nClusters];
		delete [] m_partition;
	    m_partition = new long [m_nTriangles];
		for (size_t p = 0; p != m_cVertices.size(); ++p) 
		{
			size_t v = m_cVertices[p];
			m_partition[v] = p;
			for(size_t a = 0; a < m_graph.m_vertices[v].m_ancestors.size(); a++)
			{
				m_partition[m_graph.m_vertices[v].m_ancestors[a]] = p;
			}
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
			m_convexHulls[p].SetDistPoints(&m_graph.m_vertices[v].m_distPoints);
			m_convexHulls[p].SetDistPointsPositions(m_points);
			m_convexHulls[p].SetDistPointsNormals(m_normals);
			m_convexHulls[p].SetFacePointsPositions(m_facePoints);
			m_convexHulls[p].SetFacePointsNormals(m_faceNormals);            
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
                        if (itCH->first >= 0)
                        {
                            m_convexHulls[p].AddPoint(m_points[itCH->first], itCH->first);
                        }
                        else
                        {
                            m_convexHulls[p].AddPoint(m_facePoints[-itCH->first-1], itCH->first);
                        }
                    }
                }
            }
		}       
        return true;
    }
    
    size_t HACD::GetNTrianglesCH(size_t numCH) const
    {
        if (numCH >= m_nClusters)
        {
            return 0;
        }
        return m_convexHulls[numCH].GetMesh().GetNTriangles();
    }
    size_t HACD::GetNPointsCH(size_t numCH) const
    {
        if (numCH >= m_nClusters)
        {
            return 0;
        }
        return m_convexHulls[numCH].GetMesh().GetNVertices();
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
                for (size_t p = 0; p != m_nClusters; ++p) 
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



/*
 void HACD::CleanClusters()
 {
 if (m_nClusters < 1)
 {
 return;
 }
 std::vector<long>::const_iterator it;
 delete [] m_partition;
 m_partition = new long [m_nTriangles];
 memset(m_partition, 0, sizeof(long) * m_nTriangles); 
 
 m_nClusters = 0;
 bool empty;
 for (size_t p = 0; p != m_cVertices.size(); ++p) 
 {
 m_graph.m_vertices[m_cVertices[p]].m_ancestors.push_back(m_cVertices[p]);
 }
 
 
 
 for (size_t p = 0; p != m_cVertices.size(); ++p) 
 {
 long v, vn;
 long i, j, k;
 empty = true;
 v = m_cVertices[p];
 ICHull  * ch = m_graph.m_vertices[v].m_convexHull;
 for (it =  m_graph.m_vertices[v].m_ancestors.begin();
 it != m_graph.m_vertices[v].m_ancestors.end(); ++it) 
 {
 if (m_partition[*it] == 0)
 {
 m_partition[*it] = p+1;
 empty = false;
 }
 }
 std::set<long>::const_iterator itDE(m_graph.m_vertices[v].m_edges.begin());
 std::set<long>::const_iterator itDEEnd(m_graph.m_vertices[v].m_edges.end());
 for(; itDE != itDEEnd; ++itDE) 
 {
 vn = (m_graph.m_edges[*itDE].m_v1 != v)? m_graph.m_edges[*itDE].m_v1 : m_graph.m_edges[*itDE].m_v2;
 if (vn > v)
 {
 for (it =  m_graph.m_vertices[vn].m_ancestors.begin();
 it != m_graph.m_vertices[vn].m_ancestors.end(); ++it) 
 {
 if (m_partition[*it] == 0)
 {
 i = m_triangles[*it].X();
 j = m_triangles[*it].Y();
 k = m_triangles[*it].Z();
 if (ch->IsInside(m_points[i]) &&
 ch->IsInside(m_points[j]) &&
 ch->IsInside(m_points[k])   ) //if triangle *it inside CH(p) add it to CH(p)
 {
 m_partition[*it] = p+1;
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
 m_graph.Clear();
 m_cVertices.clear();
 
 // update partition to have each CC in a different cluster
 std::vector<long> T;
 char * tags = new char [m_nTriangles];
 memset(tags, 0, m_nTriangles);
 size_t nClusters = 0;
 for(size_t t = 0; t < m_nTriangles; ++t)
 {
 if (tags[t] == 0)
 {
 long currentCluster = m_partition[t];
 m_partition[t] = nClusters;
 tags[t] = 1;
 T.push_back(t);
 long currentTriangle;
 while(T.size() > 0)
 {
 currentTriangle = T.back();
 T.pop_back();
 std::set<long>::const_iterator itN(m_t2T[currentTriangle].begin());
 std::set<long>::const_iterator itNEnd(m_t2T[currentTriangle].end());
 for(; itN != itNEnd; ++itN) 
 {
 if (tags[*itN] == 0 && m_partition[*itN]==currentCluster)
 {
 m_partition[*itN] = nClusters;
 tags[*itN] = 1;
 T.push_back(*itN);
 }
 }
 }
 T.clear();
 nClusters++;
 }
 }
 delete [] tags;
 m_nClusters = nClusters;
 
 // create graph
 m_graph.Allocate(m_nClusters, m_nClusters * (m_nClusters+1) / 2);
 for (size_t p0 = 0; p0 != nClusters; ++p0) 
 {
 ICHull  * ch = new ICHull;
 m_graph.m_vertices[p0].m_convexHull = ch;
 ch->SetDistPoints(&m_graph.m_vertices[p0].m_distPoints);
 ch->SetDistPointsPositions(m_points);
 ch->SetDistPointsNormals(m_normals);
 ch->SetFacePointsPositions(m_facePoints);
 ch->SetFacePointsNormals(m_faceNormals);
 }
 
 long currentCluster;
 long neighbourCluster;
 std::map<long, DPoint>::iterator itMap;
 long ver[3];
 long ver1[3];
 for(size_t t = 0; t < m_nTriangles; ++t)
 {
 ver[0] = m_triangles[t].X();
 ver[1] = m_triangles[t].Y();
 ver[2] = m_triangles[t].Z();
 currentCluster = m_partition[t];
 m_graph.m_vertices[currentCluster].m_ancestors.push_back(t);
 
 std::set<long>::const_iterator itN(m_t2T[t].begin());
 std::set<long>::const_iterator itNEnd(m_t2T[t].end());
 for(; itN != itNEnd; ++itN) 
 {
 neighbourCluster = m_partition[*itN];
 if (neighbourCluster != currentCluster && m_graph.GetEdgeID(neighbourCluster, currentCluster) < 0)
 {
 m_graph.AddEdge(neighbourCluster, currentCluster);
 }
 }
 
 for (int k = 0; k < 3; k++)
 {
 itMap = m_graph.m_vertices[currentCluster].m_distPoints.find(ver[k]);
 if (itMap == m_graph.m_vertices[currentCluster].m_distPoints.end() || (itMap->second).m_distOnly)
 {
 m_graph.m_vertices[currentCluster].m_convexHull->AddPoint(m_points[ver[k]], ver[k]);
 }
 m_graph.m_vertices[currentCluster].m_distPoints[ver[k]].m_distOnly = false;
 }
 
 if (m_addExtraDistPoints && m_distPointsMapping[t]>=0)
 {
 ver1[0] = m_triangles[m_distPointsMapping[t]].X();
 ver1[1] = m_triangles[m_distPointsMapping[t]].Y();
 ver1[2] = m_triangles[m_distPointsMapping[t]].Z();
 for(int k = 0; k < 3; k++)
 {
 if (m_graph.m_vertices[currentCluster].m_distPoints.find(ver1[k]) == m_graph.m_vertices[currentCluster].m_distPoints.end())
 {
 m_graph.m_vertices[currentCluster].m_distPoints[ver1[k]].m_distOnly = true;
 }
 }
 }
 }
 for (size_t p0 = 0; p0 != nClusters; ++p0) 
 {
 m_graph.m_vertices[p0].m_convexHull->Process();
 double concavity = Concavity((*m_graph.m_vertices[p0].m_convexHull), m_graph.m_vertices[p0].m_distPoints);
 double volume = m_graph.m_vertices[p0].m_convexHull->ComputeVolume() / pow(m_scale, 3.0);
 m_graph.m_vertices[p0].m_volume =  volume;
 m_graph.m_vertices[p0].m_surf      = 1.0;
 m_graph.m_vertices[p0].m_perimeter = 0.0;
 m_graph.m_vertices[p0].m_concavity = concavity;
 m_graph.m_vertices[p0].m_error     = concavity +  m_beta * volume;
 }
 for (size_t p0 = 0; p0 != nClusters; ++p0) 
 {
 if (m_callBack) 
 {
 char msg[1024];
 sprintf(msg, "\t CH \t %lu \t %lf \t %lf\n", p0, m_graph.m_vertices[p0].m_concavity, m_graph.m_vertices[p0].m_error);
 (*m_callBack)(msg, 0.0, 0.0, m_nClusters);
 }
 }
 
 InitializePQ(true);
 Simplify(true);
 }
 */
