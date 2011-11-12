/* Copyright (c) 2011 Khaled Mamou (kmamou at gmail dot com)
 All rights reserved.
 
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <hacdICHull.h>
#include <limits>
namespace HACD
{   
    const long ICHull::sc_dummyIndex = static_cast<size_t>(-1);
	ICHull::ICHull(void)
    {
		m_distPoints = 0;
        m_edgesToDelete.reserve(100);
        m_edgesToUpdate.reserve(100);
        m_trianglesToDelete.reserve(100);
        m_distPointsPositions = 0;
        m_distPointsNormals = 0;
        m_facePointsPositions = 0;
        m_facePointsNormals = 0;
		m_isFlat = false;
		m_dummyVertex = 0;
    }
	bool ICHull::AddPoints(const Vec3<double> * points, size_t nPoints)
	{
		if (!points)
		{
			return false;
		}
		CircularListElement<TMMVertex> * vertex = NULL;
		for (size_t i = 0; i < nPoints; i++)
		{
			vertex = m_mesh.AddVertex();
			vertex->GetData().m_pos = points[i];
            vertex->GetData().m_name = i;
		}     
		return true;
	}
	bool ICHull::AddPoints(std::vector< Vec3<double> > points)
	{
		CircularListElement<TMMVertex> * vertex = NULL;
		for (size_t i = 0; i < points.size(); i++)
		{
			vertex = m_mesh.AddVertex();
			vertex->GetData().m_pos = points[i];
		}     
		return true;
	}

    bool ICHull::AddPoint(const Vec3<double> & point, long id)
	{
		if (AddPoints(&point, 1))
		{
			m_mesh.m_vertices.GetData().m_name = id;
			return true;
		}
		return false;
	}

	ICHullError ICHull::Process()
	{
        unsigned long addedPoints = 0;	
		if (m_mesh.GetNVertices() < 3)
		{
			return ICHullErrorNotEnoughPoints;
		}
		if (m_mesh.GetNVertices() == 3)
		{
			m_isFlat = true;
			CircularListElement<TMMTriangle> * currentTriangle = m_mesh.AddTriangle();
			CircularListElement<TMMVertex> * v0 = m_mesh.m_vertices.GetHead();
	        CircularListElement<TMMVertex> * v1 = v0->GetNext();
	        CircularListElement<TMMVertex> * v2 = v1->GetNext();
			// Compute the normal to the plane
            Vec3<double> p0 = v0->GetData().m_pos;
            Vec3<double> p1 = v1->GetData().m_pos;            
            Vec3<double> p2 = v2->GetData().m_pos;			
            m_normal = (p1-p0) ^ (p2-p0);
            m_normal.Normalize();
			if (m_distPointsNormals && m_distPointsNormals[v0->GetData().m_name] * m_normal < 0.0)
			{
				currentTriangle->GetData().m_vertices[0] = v1;
				currentTriangle->GetData().m_vertices[1] = v0;
				currentTriangle->GetData().m_vertices[2] = v2;
				m_normal = -m_normal;
			}
			else
			{
				currentTriangle->GetData().m_vertices[0] = v0;
				currentTriangle->GetData().m_vertices[1] = v1;
				currentTriangle->GetData().m_vertices[2] = v2;
			}
			return ICHullErrorOK;
		}
		if (m_isFlat)
		{
            m_mesh.m_edges.Clear();
			m_mesh.m_triangles.Clear();
			m_isFlat = false;
		}
        if (m_mesh.GetNTriangles() == 0) // we have to create the first polyhedron
        {
			ICHullError res = DoubleTriangle();
            if (res != ICHullErrorOK)
            {
                return res;
            }
            else
            {
                addedPoints += 3;
            }
        }
        CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
        // go to the first added and not processed vertex
        while (!(vertices.GetHead()->GetPrev()->GetData().m_tag))
        {
            vertices.Prev();
        }
        while (!vertices.GetData().m_tag) // not processed
        {
            vertices.GetData().m_tag = true;
            if (ProcessPoint())
            {
                addedPoints++;
                CleanUp(addedPoints);
	            vertices.Next();
				if (!GetMesh().CheckConsistancy())
				{
					return ICHullErrorInconsistent;
				}
            }
        }
		if (m_isFlat)
		{
			
			size_t nT = m_mesh.GetNTriangles();
			for(size_t f = 0; f < nT; f++)
			{
				TMMTriangle & currentTriangle = m_mesh.m_triangles.GetHead()->GetData();
				if( currentTriangle.m_vertices[0]->GetData().m_name == sc_dummyIndex ||
					currentTriangle.m_vertices[1]->GetData().m_name == sc_dummyIndex ||
					currentTriangle.m_vertices[2]->GetData().m_name == sc_dummyIndex )
				{
					m_trianglesToDelete.push_back(m_mesh.m_triangles.GetHead());
                    for(int k = 0; k < 3; k++)
                    {
                        for(int h = 0; h < 2; h++)
                        {
                            if (currentTriangle.m_edges[k]->GetData().m_triangles[h] == m_mesh.m_triangles.GetHead())
                            {
                                currentTriangle.m_edges[k]->GetData().m_triangles[h] = 0;
                                break;
                            }
                        }
                    }
				}
				m_mesh.m_triangles.Next();
			}
            size_t nE = m_mesh.GetNEdges();
            for(size_t e = 0; e < nE; e++)
			{
				TMMEdge & currentEdge = m_mesh.m_edges.GetHead()->GetData();
				if( currentEdge.m_triangles[0] == 0 && currentEdge.m_triangles[1] == 0) 
				{
					m_edgesToDelete.push_back(m_mesh.m_edges.GetHead());
				}
				m_mesh.m_edges.Next();
			}
			m_mesh.m_vertices.Delete(m_dummyVertex);
			m_dummyVertex = 0;
            size_t nV = m_mesh.GetNVertices();
            CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
			for(size_t v = 0; v < nV; ++v)
			{
                vertices.GetData().m_tag = false;
                vertices.Next();
            }
            CleanEdges();
			CleanTriangles();
		}
		return ICHullErrorOK;
	}
    ICHullError ICHull::Process(unsigned long nPointsCH)
	{
        unsigned long addedPoints = 0;  
        if (nPointsCH < 3 || m_mesh.GetNVertices() < 3)
		{
			return ICHullErrorNotEnoughPoints;
		}
		if (m_mesh.GetNVertices() == 3)
		{
			m_isFlat = true;
			CircularListElement<TMMTriangle> * currentTriangle = m_mesh.AddTriangle();
			CircularListElement<TMMVertex> * v0 = m_mesh.m_vertices.GetHead();
	        CircularListElement<TMMVertex> * v1 = v0->GetNext();
	        CircularListElement<TMMVertex> * v2 = v1->GetNext();
			// Compute the normal to the plane
            Vec3<double> p0 = v0->GetData().m_pos;
            Vec3<double> p1 = v1->GetData().m_pos;            
            Vec3<double> p2 = v2->GetData().m_pos;			
            m_normal = (p1-p0) ^ (p2-p0);
            m_normal.Normalize();
			if (m_distPointsNormals && m_distPointsNormals[v0->GetData().m_name] * m_normal < 0.0)
			{
				currentTriangle->GetData().m_vertices[0] = v1;
				currentTriangle->GetData().m_vertices[1] = v0;
				currentTriangle->GetData().m_vertices[2] = v2;
				m_normal = -m_normal;
			}
			else
			{
				currentTriangle->GetData().m_vertices[0] = v0;
				currentTriangle->GetData().m_vertices[1] = v1;
				currentTriangle->GetData().m_vertices[2] = v2;
			}
			return ICHullErrorOK;
		}

		if (m_isFlat)
		{
			m_mesh.m_triangles.Clear();
            m_mesh.m_edges.Clear();
            m_isFlat = false;
		}
        
        if (m_mesh.GetNTriangles() == 0) // we have to create the first polyhedron
        {
			ICHullError res = DoubleTriangle();
            if (res != ICHullErrorOK)
            {
                return res;
            }
            else
            {
                addedPoints += 3;
            }
        }
        CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
        while (!vertices.GetData().m_tag && addedPoints < nPointsCH) // not processed
        {
            if (!FindMaxVolumePoint())
            {
                break;
            }                  
            vertices.GetData().m_tag = true;                      
            if (ProcessPoint())
            {
                addedPoints++;
                CleanUp(addedPoints);
				if (!GetMesh().CheckConsistancy())
				{
					return ICHullErrorInconsistent;
				}
                vertices.Next();
            }
        }
        // delete remaining points
        while (!vertices.GetData().m_tag)
        {
            vertices.Delete();
        }
		if (m_isFlat)
		{
			
			size_t nT = m_mesh.GetNTriangles();
			for(size_t f = 0; f < nT; f++)
			{
				TMMTriangle & currentTriangle = m_mesh.m_triangles.GetHead()->GetData();
				if( currentTriangle.m_vertices[0]->GetData().m_name == sc_dummyIndex ||
                   currentTriangle.m_vertices[1]->GetData().m_name == sc_dummyIndex ||
                   currentTriangle.m_vertices[2]->GetData().m_name == sc_dummyIndex )
				{
					m_trianglesToDelete.push_back(m_mesh.m_triangles.GetHead());
                    for(int k = 0; k < 3; k++)
                    {
                        for(int h = 0; h < 2; h++)
                        {
                            if (currentTriangle.m_edges[k]->GetData().m_triangles[h] == m_mesh.m_triangles.GetHead())
                            {
                                currentTriangle.m_edges[k]->GetData().m_triangles[h] = 0;
                                break;
                            }
                        }
                    }
				}
				m_mesh.m_triangles.Next();
			}
            size_t nE = m_mesh.GetNEdges();
            for(size_t e = 0; e < nE; e++)
			{
				TMMEdge & currentEdge = m_mesh.m_edges.GetHead()->GetData();
				if( currentEdge.m_triangles[0] == 0 && currentEdge.m_triangles[1] == 0) 
				{
					m_edgesToDelete.push_back(m_mesh.m_edges.GetHead());
				}
				m_mesh.m_edges.Next();
			}
			m_mesh.m_vertices.Delete(m_dummyVertex);
			m_dummyVertex = 0;
            size_t nV = m_mesh.GetNVertices();
            CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
			for(size_t v = 0; v < nV; ++v)
			{
                vertices.GetData().m_tag = false;
                vertices.Next();
            }
            CleanEdges();
			CleanTriangles();
		}
		return ICHullErrorOK;
	}
    bool ICHull::FindMaxVolumePoint()
	{
        CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
        CircularListElement<TMMVertex> * vMaxVolume = 0;
        CircularListElement<TMMVertex> * vHeadPrev = vertices.GetHead()->GetPrev();
        
        double maxVolume = 0.0;
        double volume = 0.0;
        
        while (!vertices.GetData().m_tag) // not processed
        {
            if (ComputePointVolume(volume, false))
            {
                if ( maxVolume < volume)
                {
                    maxVolume = volume;
                    vMaxVolume = vertices.GetHead();
                }
                vertices.Next();
            }
        }
        CircularListElement<TMMVertex> * vHead = vHeadPrev->GetNext();
        vertices.GetHead() = vHead;  
        
        if (!vMaxVolume)
        {
            return false;
        }
        
        if (vMaxVolume != vHead)
        {
            Vec3<double> pos = vHead->GetData().m_pos;
            long id = vHead->GetData().m_name;
            vHead->GetData().m_pos = vMaxVolume->GetData().m_pos;
            vHead->GetData().m_name = vMaxVolume->GetData().m_name;
            vMaxVolume->GetData().m_pos = pos;
            vHead->GetData().m_name = id;
        }
        
 
        return true;
    }
	ICHullError ICHull::DoubleTriangle()
	{
        // find three non colinear points
		m_isFlat = false;
        CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
        CircularListElement<TMMVertex> * v0 = vertices.GetHead();
        while( Colinear(v0->GetData().m_pos, 
                        v0->GetNext()->GetData().m_pos, 
                        v0->GetNext()->GetNext()->GetData().m_pos))
        {
            if ( (v0 = v0->GetNext()) == vertices.GetHead())
            {
                return ICHullErrorCoplanarPoints;
            }
        }
        CircularListElement<TMMVertex> * v1 = v0->GetNext();
        CircularListElement<TMMVertex> * v2 = v1->GetNext();
        // mark points as processed
        v0->GetData().m_tag = v1->GetData().m_tag = v2->GetData().m_tag = true;
        
        // create two triangles
        CircularListElement<TMMTriangle> * f0 = MakeFace(v0, v1, v2, 0);
        MakeFace(v2, v1, v0, f0);

        // find a fourth non-coplanar point to form tetrahedron
        CircularListElement<TMMVertex> * v3 = v2->GetNext();
        vertices.GetHead() = v3;

		double vol = Volume(v0->GetData().m_pos, v1->GetData().m_pos, v2->GetData().m_pos, v3->GetData().m_pos);
		while (vol == 0.0 && !v3->GetNext()->GetData().m_tag)
		{
			v3 = v3->GetNext();
			vol = Volume(v0->GetData().m_pos, v1->GetData().m_pos, v2->GetData().m_pos, v3->GetData().m_pos);
		}			
		if (vol == 0.0)
		{
			// compute the barycenter
			Vec3<double> bary(0.0,0.0,0.0);
			CircularListElement<TMMVertex> * vBary = v0;
			do
			{
				bary += vBary->GetData().m_pos;
			}
			while ( (vBary = vBary->GetNext()) != v0);
			bary /= vertices.GetSize();

			// Compute the normal to the plane
            Vec3<double> p0 = v0->GetData().m_pos;
            Vec3<double> p1 = v1->GetData().m_pos;            
            Vec3<double> p2 = v2->GetData().m_pos;			
            m_normal = (p1-p0) ^ (p2-p0);
            m_normal.Normalize();
			// add dummy vertex placed at (bary + normal)
			vertices.GetHead() = v2;
			AddPoint(bary + m_normal, sc_dummyIndex); 
			m_dummyVertex = vertices.GetHead();
			m_isFlat = true;
            v3 = v2->GetNext();
            vol = Volume(v0->GetData().m_pos, v1->GetData().m_pos, v2->GetData().m_pos, v3->GetData().m_pos);
			return ICHullErrorOK;
		}
		else if (v3 != vertices.GetHead())
		{
			TMMVertex temp;
			temp.m_name = v3->GetData().m_name;
			temp.m_pos = v3->GetData().m_pos;
			v3->GetData().m_name = vertices.GetHead()->GetData().m_name;
			v3->GetData().m_pos = vertices.GetHead()->GetData().m_pos;
			vertices.GetHead()->GetData().m_name = temp.m_name;
			vertices.GetHead()->GetData().m_pos = temp.m_pos;
		}
		return ICHullErrorOK;
	}
    CircularListElement<TMMTriangle> *	ICHull::MakeFace(CircularListElement<TMMVertex> * v0,  
                                                         CircularListElement<TMMVertex> * v1,
                                                         CircularListElement<TMMVertex> * v2,
                                                         CircularListElement<TMMTriangle> * fold)
	{        
        CircularListElement<TMMEdge> * e0;
        CircularListElement<TMMEdge> * e1;
        CircularListElement<TMMEdge> * e2;
        long index = 0;
        if (!fold) // if first face to be created
        {
            e0 = m_mesh.AddEdge(); // create the three edges
            e1 = m_mesh.AddEdge();
            e2 = m_mesh.AddEdge();            
        }
        else // otherwise re-use existing edges (in reverse order)
        {
            e0 = fold->GetData().m_edges[2];
            e1 = fold->GetData().m_edges[1];
            e2 = fold->GetData().m_edges[0];
            index = 1;
        }
        e0->GetData().m_vertices[0] = v0; e0->GetData().m_vertices[1] = v1;
        e1->GetData().m_vertices[0] = v1; e1->GetData().m_vertices[1] = v2;
        e2->GetData().m_vertices[0] = v2; e2->GetData().m_vertices[1] = v0;
        // create the new face
        CircularListElement<TMMTriangle> * f = m_mesh.AddTriangle();   
        f->GetData().m_edges[0]    = e0; f->GetData().m_edges[1]    = e1; f->GetData().m_edges[2]    = e2;
        f->GetData().m_vertices[0] = v0; f->GetData().m_vertices[1] = v1; f->GetData().m_vertices[2] = v2;     
        // link edges to face f
        e0->GetData().m_triangles[index] = e1->GetData().m_triangles[index] = e2->GetData().m_triangles[index] = f;
        UpdateFaceDistances(f);
		return f;
	}
	CircularListElement<TMMTriangle> * ICHull::MakeConeFace(CircularListElement<TMMEdge> * e, CircularListElement<TMMVertex> * p)
	{
        // create two new edges if they don't already exist
        CircularListElement<TMMEdge> * newEdges[2];
        for(int i = 0; i < 2; ++i)
        {
            if ( !( newEdges[i] = e->GetData().m_vertices[i]->GetData().m_duplicate ) )  
            { // if the edge doesn't exits add it and mark the vertex as duplicated
                newEdges[i] = m_mesh.AddEdge();
                newEdges[i]->GetData().m_vertices[0] = e->GetData().m_vertices[i];
                newEdges[i]->GetData().m_vertices[1] = p;
                e->GetData().m_vertices[i]->GetData().m_duplicate = newEdges[i];
            }
        }
        // make the new face
        CircularListElement<TMMTriangle> * newFace = m_mesh.AddTriangle();
        newFace->GetData().m_edges[0] = e;
        newFace->GetData().m_edges[1] = newEdges[0];
        newFace->GetData().m_edges[2] = newEdges[1];
        MakeCCW(newFace, e, p);
        for(int i=0; i < 2; ++i)
        {
            for(int j=0; j < 2; ++j)
            {
                if ( ! newEdges[i]->GetData().m_triangles[j] )
                {
                    newEdges[i]->GetData().m_triangles[j] = newFace;
                    break;
                }
            }
        }
        UpdateFaceDistances(newFace);
		return newFace;
	}
    void ICHull::UpdateFaceDistances(CircularListElement<TMMTriangle> * face)
    {
		if (m_isFlat)
		{
			// to be updated
		}
		else
		{
			if (m_distPoints && m_distPointsPositions && m_distPointsNormals)
			{
				TMMTriangle & currentTriangle = face->GetData();
				double dist;
				int nhit;
				std::map<long, DPoint>::iterator itDP(m_distPoints->begin());
				std::map<long, DPoint>::iterator itDPEnd(m_distPoints->end());
				for(; itDP != itDPEnd; ++itDP) 
				{
					if ((itDP->second).m_computed)
					{

						for(int k = 0; k < 3; k++)
						{
                            if (itDP->first >= 0)
                            {
                                nhit = IntersectRayTriangle(m_distPointsPositions[itDP->first], m_distPointsNormals[itDP->first], 
														currentTriangle.m_vertices[k]->GetData().m_pos, 
														currentTriangle.m_vertices[(k+1)%3]->GetData().m_pos, 
														currentTriangle.m_vertices[(k+2)%3]->GetData().m_pos, dist);
                            }
                            else if (m_facePointsPositions && m_facePointsNormals)
                            {
                                nhit = IntersectRayTriangle(m_facePointsPositions[-itDP->first-1], m_facePointsNormals[-itDP->first-1], 
                                                            currentTriangle.m_vertices[k]->GetData().m_pos, 
                                                            currentTriangle.m_vertices[(k+1)%3]->GetData().m_pos, 
                                                            currentTriangle.m_vertices[(k+2)%3]->GetData().m_pos, dist);
                            }
							if (nhit == 1 && dist > 0.0)
							{
								 if(dist > (itDP->second).m_dist)
								{
									(itDP->second).m_dist = dist;
									(itDP->second).m_computed = true;
									face->GetData().m_incidentPoints.insert(itDP->first);
								}
							}
						}
					}
				}
			}
		}  
    }
    bool ICHull::ComputePointVolume(double &totalVolume, bool markVisibleFaces)
    {
        // mark visible faces
        CircularListElement<TMMTriangle> * fHead = m_mesh.GetTriangles().GetHead();
        CircularListElement<TMMTriangle> * f = fHead;
        CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
        CircularListElement<TMMVertex> * vertex0 = vertices.GetHead();
        bool visible = false;
        Vec3<double> pos0 = vertex0->GetData().m_pos;
        double vol = 0.0;
        totalVolume = 0.0;
		Vec3<double> ver0, ver1, ver2;
        do 
        {
			ver0 = f->GetData().m_vertices[0]->GetData().m_pos;
			ver1 = f->GetData().m_vertices[1]->GetData().m_pos;
			ver2 = f->GetData().m_vertices[2]->GetData().m_pos;
			vol = Volume(ver0, ver1, ver2, pos0);
			if ( vol < 0.0 )
			{
				vol = fabs(vol);
				totalVolume += vol;
				if (markVisibleFaces)
				{
					f->GetData().m_visible = true;
					m_trianglesToDelete.push_back(f);
				}
				visible = true;
			}
			f = f->GetNext();
        } 
        while (f != fHead);

		if (m_trianglesToDelete.size() == m_mesh.m_triangles.GetSize())
		{
			for(size_t i = 0; i < m_trianglesToDelete.size(); i++)
			{
				m_trianglesToDelete[i]->GetData().m_visible = false;
			}
			visible = false;
		}
        // if no faces visible from p then p is inside the hull
        if (!visible && markVisibleFaces)
        {
            vertices.Delete();
			m_trianglesToDelete.clear();
            return false;
        }
        return true;
    }
	bool ICHull::ProcessPoint()
	{
        double totalVolume = 0.0;
        if (!ComputePointVolume(totalVolume, true))
        {
            return false;
        }
        // Mark edges in interior of visible region for deletion.
        // Create a new face based on each border edge
        CircularListElement<TMMVertex> * v0 = m_mesh.GetVertices().GetHead();
        CircularListElement<TMMEdge> * eHead = m_mesh.GetEdges().GetHead();
        CircularListElement<TMMEdge> * e = eHead;    
        CircularListElement<TMMEdge> * tmp = 0;
        long nvisible = 0;
        m_edgesToDelete.clear();
        m_edgesToUpdate.clear();
        do 
        {
            tmp = e->GetNext();
            nvisible = 0;
            for(int k = 0; k < 2; k++)
            {
                if ( e->GetData().m_triangles[k]->GetData().m_visible )
                {
                    nvisible++;
                }
            }
            if ( nvisible == 2)
            {
                m_edgesToDelete.push_back(e);
            }
            else if ( nvisible == 1)
            {
                e->GetData().m_newFace = MakeConeFace(e, v0);
                m_edgesToUpdate.push_back(e);
            }
            e = tmp;
        }
        while (e != eHead);        
		return true;
	}
    bool ICHull::MakeCCW(CircularListElement<TMMTriangle> * f,
                         CircularListElement<TMMEdge> * e, 
                         CircularListElement<TMMVertex> * v)
    {
        // the visible face adjacent to e
        CircularListElement<TMMTriangle> * fv; 
        if (e->GetData().m_triangles[0]->GetData().m_visible)
        {
            fv = e->GetData().m_triangles[0];
        }
        else
        {
            fv = e->GetData().m_triangles[1];
        }
        
        //  set vertex[0] and vertex[1] to have the same orientation as the corresponding vertices of fv.
        long i;                                 // index of e->m_vertices[0] in fv
        CircularListElement<TMMVertex> * v0 = e->GetData().m_vertices[0];
        CircularListElement<TMMVertex> * v1 = e->GetData().m_vertices[1];
        for(i = 0; fv->GetData().m_vertices[i] !=  v0; i++);
        
        if ( fv->GetData().m_vertices[(i+1) % 3] != e->GetData().m_vertices[1] )
        {
            f->GetData().m_vertices[0] = v1;
            f->GetData().m_vertices[1] = v0;
        }
        else
        {
            f->GetData().m_vertices[0] = v0;
            f->GetData().m_vertices[1] = v1;  
            // swap edges
            CircularListElement<TMMEdge> * tmp = f->GetData().m_edges[0];
            f->GetData().m_edges[0] = f->GetData().m_edges[1];
            f->GetData().m_edges[1] = tmp;
        }
        f->GetData().m_vertices[2] = v;
        return true;
    }
    bool ICHull::CleanUp(unsigned long & addedPoints)
    {
        bool r0 = CleanEdges();
        bool r1 = CleanTriangles();
        bool r2 = CleanVertices(addedPoints);
        return  r0 && r1 && r2;
    }
    bool ICHull::CleanEdges()
    {
        // integrate the new faces into the data structure
        CircularListElement<TMMEdge> * e;
        const std::vector<CircularListElement<TMMEdge> *>::iterator itEndUpdate = m_edgesToUpdate.end();
        for(std::vector<CircularListElement<TMMEdge> *>::iterator it = m_edgesToUpdate.begin(); it != itEndUpdate; ++it)
        {
            e = *it;
            if ( e->GetData().m_newFace )
            {
                if ( e->GetData().m_triangles[0]->GetData().m_visible)
                {
                    e->GetData().m_triangles[0] = e->GetData().m_newFace;
                }
                else
                {
                    e->GetData().m_triangles[1] = e->GetData().m_newFace;
                }
                e->GetData().m_newFace = 0;
            }           
        }
        // delete edges maked for deletion
        CircularList<TMMEdge> & edges = m_mesh.GetEdges();
        const std::vector<CircularListElement<TMMEdge> *>::iterator itEndDelete = m_edgesToDelete.end();
        for(std::vector<CircularListElement<TMMEdge> *>::iterator it = m_edgesToDelete.begin(); it != itEndDelete; ++it)
        {
            edges.Delete(*it);         
        }
		m_edgesToDelete.clear();
        m_edgesToUpdate.clear();
        return true;
    }
    bool ICHull::CleanTriangles()
    {
        CircularList<TMMTriangle> & triangles = m_mesh.GetTriangles();
        const std::vector<CircularListElement<TMMTriangle> *>::iterator itEndDelete = m_trianglesToDelete.end();
        for(std::vector<CircularListElement<TMMTriangle> *>::iterator it = m_trianglesToDelete.begin(); it != itEndDelete; ++it)
        {
			if (m_distPoints)
			{
				if (m_isFlat)
				{
					// to be updated
				}
				else
				{
					std::set<long>::const_iterator itPEnd((*it)->GetData().m_incidentPoints.end());
					std::set<long>::const_iterator itP((*it)->GetData().m_incidentPoints.begin());
					std::map<long, DPoint>::iterator itPoint;
					for(; itP != itPEnd; ++itP) 
					{
						itPoint = m_distPoints->find(*itP);
						if (itPoint != m_distPoints->end())
						{
							itPoint->second.m_computed = false;
						}
					}
				}
			}
            triangles.Delete(*it);         
        }
		m_trianglesToDelete.clear();
        return true;
    }
    bool ICHull::CleanVertices(unsigned long & addedPoints)
    {
        // mark all vertices incident to some undeleted edge as on the hull
        CircularList<TMMEdge> & edges = m_mesh.GetEdges();
        CircularListElement<TMMEdge> * e = edges.GetHead();
        size_t nE = edges.GetSize();
        for(size_t i = 0; i < nE; i++)
        {
            e->GetData().m_vertices[0]->GetData().m_onHull = true;
            e->GetData().m_vertices[1]->GetData().m_onHull = true;
            e = e->GetNext();
        }
        // delete all the vertices that have been processed but are not on the hull
        CircularList<TMMVertex> & vertices = m_mesh.GetVertices();
        CircularListElement<TMMVertex> * vHead = vertices.GetHead();
        CircularListElement<TMMVertex> * v = vHead;
        v = v->GetPrev();
        do 
        {
            if (v->GetData().m_tag && !v->GetData().m_onHull)
            {
                CircularListElement<TMMVertex> * tmp = v->GetPrev();
                vertices.Delete(v);
                v = tmp;
                addedPoints--;
            }
            else
            {
                v->GetData().m_duplicate = 0;
                v->GetData().m_onHull = false;
                v = v->GetPrev();
            }
        } 
        while (v->GetData().m_tag && v != vHead);
        return true;
    }
	void ICHull::Clear()
	{	
		m_mesh.Clear();
		m_edgesToDelete = std::vector<CircularListElement<TMMEdge> *>();
		m_edgesToUpdate = std::vector<CircularListElement<TMMEdge> *>();
		m_trianglesToDelete= std::vector<CircularListElement<TMMTriangle> *>();
		m_isFlat = false;
	}
    const ICHull & ICHull::operator=(ICHull & rhs)
    {
        if (&rhs != this)
        {
            m_mesh.Copy(rhs.m_mesh);
            m_edgesToDelete = rhs.m_edgesToDelete;
            m_edgesToUpdate = rhs.m_edgesToUpdate;
            m_trianglesToDelete = rhs.m_trianglesToDelete;
			m_isFlat = rhs.m_isFlat;
        }
        return (*this);
    }   
    double ICHull::ComputeVolume()
    {
        size_t nV = m_mesh.m_vertices.GetSize();
		if (nV == 0 || m_isFlat)
		{
			return 0.0;
		}       
        Vec3<double> bary(0.0, 0.0, 0.0);

        for(size_t v = 0; v < nV; v++)
        {
			bary +=  m_mesh.m_vertices.GetHead()->GetData().m_pos;
			m_mesh.m_vertices.Next();
        }
        bary /= nV;
        
        size_t nT = m_mesh.m_triangles.GetSize();
        Vec3<double> ver0, ver1, ver2;
        double totalVolume = 0.0;
        for(size_t t = 0; t < nT; t++)
        {
            ver0 = m_mesh.m_triangles.GetHead()->GetData().m_vertices[0]->GetData().m_pos;
			ver1 = m_mesh.m_triangles.GetHead()->GetData().m_vertices[1]->GetData().m_pos;
			ver2 = m_mesh.m_triangles.GetHead()->GetData().m_vertices[2]->GetData().m_pos;
			totalVolume += Volume(ver0, ver1, ver2, bary);
			m_mesh.m_triangles.Next();
        }
        return totalVolume;
    }
    bool ICHull::IsInside(const Vec3<double> pt)
    {
		if (m_isFlat)
		{
			// to be fixed
			return false;
		}
		else
		{
			size_t nT = m_mesh.m_triangles.GetSize();
			Vec3<double> ver0, ver1, ver2;
			for(size_t t = 0; t < nT; t++)
			{
				ver0 = m_mesh.m_triangles.GetHead()->GetData().m_vertices[0]->GetData().m_pos;
				ver1 = m_mesh.m_triangles.GetHead()->GetData().m_vertices[1]->GetData().m_pos;
				ver2 = m_mesh.m_triangles.GetHead()->GetData().m_vertices[2]->GetData().m_pos;
				if (Volume(ver0, ver1, ver2, pt) < 0.0)
				{
					return false;
				}
				m_mesh.m_triangles.Next();
			}
			return true;
		}
    }
	double ICHull::ComputeDistance(long name, const Vec3<double> & pt, const Vec3<double> & normal, bool & insideHull, bool updateIncidentPoints, bool debug)
	{
		if (m_isFlat)
		{
			double distance = 0.0;
			Vec3<double> ptNormal = normal;
			ptNormal -= (ptNormal * m_normal) * m_normal;
			if (ptNormal.GetNorm() > 0.0)
			{
				ptNormal.Normalize();
				long nameVE1;
				long nameVE2;
				Vec3<double> pa, pb, d0, d1, d2, d3;
				Vec3<double> p0 = pt;
				Vec3<double> p1 = p0 + ptNormal;
				Vec3<double> p2, p3;
				double mua, mub, s;
				const double EPS = 0.00000000001;
				size_t nE = m_mesh.GetNEdges();
				for(size_t e = 0; e < nE; e++)
				{
					TMMEdge & currentEdge = m_mesh.m_edges.GetHead()->GetData();
                    nameVE1 = currentEdge.m_vertices[0]->GetData().m_name;
                    nameVE2 = currentEdge.m_vertices[1]->GetData().m_name;
                    if (currentEdge.m_triangles[0] == 0 || currentEdge.m_triangles[1] == 0)
                    {
                        if ( nameVE1==name || nameVE2==name )
                        {
                            return 0.0;
                        }
                        /*
                        if (debug) std::cout << "V" << name 
                                             << " E "  << nameVE1 << " " << nameVE2 << std::endl;
                         */

                        p2 = currentEdge.m_vertices[0]->GetData().m_pos;
                        p3 = currentEdge.m_vertices[1]->GetData().m_pos;
                        d0 = p3 - p2;
                        if (d0.GetNorm() > 0.0)
                        {
                            if (IntersectLineLine(p0, p1, p2, p3, pa, pb, mua, mub))
                            {
                                d1 = pa - p2;
                                d2 = pa - pb;
                                d3 = pa - p0;
                                mua = d1.GetNorm()/d0.GetNorm();
                                mub = d1*d0;
                                s = d3*ptNormal;
                                if (d2.GetNorm() < EPS &&  mua <= 1.0 && mub>=0.0 && s>0.0)
                                {
                                    distance = std::max<double>(distance, d3.GetNorm());
                                }
                            }
                        }
                    }
					m_mesh.m_edges.Next();
				}
			}
			return distance;
		}
		else
		{
			Vec3<double> impact;
			Vec3<double> vec;
			long nhit;
			double dist;
			double distance = 0.0; //std::numeric_limits<double>::max();
			size_t nT = m_mesh.GetNTriangles();
			insideHull = false;
			CircularListElement<TMMTriangle> * face = 0;
			for(size_t f = 0; f < nT; f++)
			{
				TMMTriangle & currentTriangle = m_mesh.m_triangles.GetHead()->GetData();
	/*
				if (debug) std::cout << "T " << currentTriangle.m_vertices[0]->GetData().m_name << " "
														  << currentTriangle.m_vertices[1]->GetData().m_name << " "
														  << currentTriangle.m_vertices[2]->GetData().m_name << std::endl;
	*/
				if (currentTriangle.m_vertices[0]->GetData().m_name == name ||
					currentTriangle.m_vertices[1]->GetData().m_name == name ||
					currentTriangle.m_vertices[2]->GetData().m_name == name )
				{
					distance = 0.0;
					insideHull = true;
					face = m_mesh.m_triangles.GetHead();				  
					break;
				}
				else
				{	
					for(int k = 0; k < 3; k++)
					{
						
						nhit = IntersectRayTriangle(pt, normal, 
														currentTriangle.m_vertices[k]->GetData().m_pos, 
														currentTriangle.m_vertices[(k+1)%3]->GetData().m_pos, 
														currentTriangle.m_vertices[(k+2)%3]->GetData().m_pos, dist);


						if (nhit == 1 /*&& dist > 0.0*/ && distance < dist )
						{
							distance = dist;
							insideHull = true;
							face = m_mesh.m_triangles.GetHead();				  
						}
					}
				}
				m_mesh.m_triangles.Next();
			}
			if (updateIncidentPoints && face && m_distPoints)
			{
				(*m_distPoints)[name].m_dist = distance;
				face->GetData().m_incidentPoints.insert(name);
			}
			return distance;
		}
	}
}

