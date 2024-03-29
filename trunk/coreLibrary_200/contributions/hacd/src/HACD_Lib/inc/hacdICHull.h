/* Copyright (c) 2011 Khaled Mamou (kmamou at gmail dot com)
 All rights reserved.
 
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 3. The names of the contributors may not be used to endorse or promote products derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#ifndef HACD_ICHULL_H
#define HACD_ICHULL_H
#include <hacdVersion.h>
#include <hacdManifoldMesh.h>
#include <hacdVector.h>
#include <vector>
#include <map>
namespace HACD
{
	class DPoint;
	//!	Incremental Convex Hull algorithm (cf. http://maven.smith.edu/~orourke/books/ftp.html ).
	enum ICHullError
	{
		ICHullErrorOK = 0,
		ICHullErrorCoplanarPoints,
		ICHullErrorNoVolume,
		ICHullErrorInconsistent,
		ICHullErrorNotEnoughPoints
	};
	class ICHull
	{
		public:
												
			//! 
            std::map<long, DPoint> *							GetDistPoints() const { return m_distPoints;}
			//!
			void												SetDistPoints(std::map<long, DPoint> * distPoints) { m_distPoints = distPoints;}
            //!
            Vec3<double> *                                      GetFacePointsPositions() const { return m_facePointsPositions;}
            //!
            void												SetFacePointsPositions(Vec3<double> * distPointsPositions) { m_facePointsPositions = distPointsPositions;}
            //!
            Vec3<double> *                                      GetFacePointsNormals() const { return m_facePointsNormals;}
            //!
            void												SetFacePointsNormals(Vec3<double> * distPointsNormals) { m_facePointsNormals = distPointsNormals;}
            //!
            Vec3<double> *                                      GetDistPointsPositions() const { return m_distPointsPositions;}
            //!
            void												SetDistPointsPositions(Vec3<double> * distPointsPositions) { m_distPointsPositions = distPointsPositions;}
            //!
            Vec3<double> *                                      GetDistPointsNormals() const { return m_distPointsNormals;}
            //!
            void												SetDistPointsNormals(Vec3<double> * distPointsNormals) { m_distPointsNormals = distPointsNormals;}
			//! Returns the computed mesh
            TMMesh &                                            GetMesh() { return m_mesh;}
			//!	Add one point to the convex-hull    
            bool                                                AddPoint(const Vec3<double> & point) {return AddPoints(&point, 1);}
			//!	Add one point to the convex-hull    
            bool                                                AddPoint(const Vec3<double> & point, long id);
			//!	Add points to the convex-hull
			bool                                                AddPoints(const Vec3<double> * points, size_t nPoints);	
			bool												AddPoints(std::vector< Vec3<double> > points);
			//!	
			ICHullError                                         Process();
            //! 
            ICHullError                                         Process(unsigned long nPointsCH);
            //!
            double                                              ComputeVolume();
            //!
            bool                                                IsInside(const Vec3<double> pt);
			//!
			double												ComputeDistance(long name, const Vec3<double> & pt, const Vec3<double> & normal, bool & insideHull, bool updateIncidentPoints, bool debug = false);
            //!
            const ICHull &                                      operator=(ICHull & rhs);        

			//!	Constructor
																ICHull(void);
			//! Destructor
			virtual                                             ~ICHull(void) {};

		private:
            //!	DoubleTriangle builds the initial double triangle.  It first finds 3 noncollinear points and makes two faces out of them, in opposite order. It then finds a fourth point that is not coplanar with that face.  The vertices are stored in the face structure in counterclockwise order so that the volume between the face and the point is negative. Lastly, the 3 newfaces to the fourth point are constructed and the data structures are cleaned up. 
			ICHullError                                         DoubleTriangle();
            //!	MakeFace creates a new face structure from three vertices (in ccw order).  It returns a pointer to the face.
            CircularListElement<TMMTriangle> *                  MakeFace(CircularListElement<TMMVertex> * v0,  
                                                                         CircularListElement<TMMVertex> * v1,
                                                                         CircularListElement<TMMVertex> * v2,
                                                                         CircularListElement<TMMTriangle> * fold);			
            //!	
            CircularListElement<TMMTriangle> *                  MakeConeFace(CircularListElement<TMMEdge> * e, CircularListElement<TMMVertex> * v);
			//!	
			bool                                                ProcessPoint();
            //!
            bool                                                ComputePointVolume(double &totalVolume, bool markVisibleFaces);
            //!
            bool                                                FindMaxVolumePoint();
            //!	
            bool                                                CleanEdges();
            //!	
            bool                                                CleanVertices(unsigned long & addedPoints);
            //!	
            bool                                                CleanTriangles();
            //!	
            bool                                                CleanUp(unsigned long & addedPoints);
            //!
            bool                                                MakeCCW(CircularListElement<TMMTriangle> * f,
                                                                        CircularListElement<TMMEdge> * e, 
                                                                        CircularListElement<TMMVertex> * v);
			void												Clear(); 
            void                                                UpdateFaceDistances(CircularListElement<TMMTriangle> * face);
		private:
            static const long                                   sc_dummyIndex;
			TMMesh                                              m_mesh;
            std::vector<CircularListElement<TMMEdge> *>         m_edgesToDelete;
            std::vector<CircularListElement<TMMEdge> *>         m_edgesToUpdate;
            std::vector<CircularListElement<TMMTriangle> *>     m_trianglesToDelete; 
			std::map<long, DPoint> *							m_distPoints;
            Vec3<double> *                                      m_distPointsPositions;
            Vec3<double> *                                      m_distPointsNormals;
            Vec3<double> *                                      m_facePointsPositions;
            Vec3<double> *                                      m_facePointsNormals;
			bool												m_isFlat;
			CircularListElement<TMMVertex> *					m_dummyVertex;
			Vec3<double>										m_normal;
        
					                                           
																ICHull(const ICHull & rhs);
        
			friend class HACD;
	};

}
#endif