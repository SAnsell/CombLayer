/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Face.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h" 
#include "OutputLog.h"
#include "BaseVisit.h"
#include "MatrixBase.h"
#include "BaseModVisit.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "Binary.h"
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"

namespace Geometry
{

std::ostream&
operator<<(std::ostream& OX,const Face& A)
  /*!
    Standard output stream
    \param OX :: Output stream
    \param A :: Face to write
    \return Stream State
   */
{
  A.write(OX);
  return OX;
}

Face::Face():
  visible(0)
  /*!
    Default constructor
  */
{
  for(int i=0;i<3;i++)
    {
      FPts[i]=0;
      ELines[i]=0;
    }
}

Face::Face(Vertex* VA,Vertex* VB,Vertex* VC,
	   Edge* EA,Edge* EB,Edge* EC) :
  visible(0)
  /*!
    Setting Constructor
    \param VA :: Vertex 1.
    \param VB :: Vertex 2.
    \param VC :: Vertex 3.
    \param EA :: Edge 1:
    \param EB :: Edge 2:
    \param EC :: Edge 3:
  */
{
  setVertex(VA,VB,VC,EA,EB,EC);
}


Face::Face(const Face& A) :
  visible(A.visible)
  /*!
    Copy constructor
    \param A :: Face Object to copy
  */
{
  for(int i=0;i<3;i++)
    {
      FPts[i]=A.FPts[i];
      ELines[i]=A.ELines[i];
    }
}

Face&
Face::operator=(const Face& A) 
  /*!
    Assignment operator
    \param A :: Face Object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      visible=A.visible;
      for(int i=0;i<3;i++)
       {
	 FPts[i]=A.FPts[i];
	 ELines[i]=A.ELines[i];
       }
    }
  return *this;
}

Face::~Face()
  /*!
    Destructor
  */
{}

void
Face::setVertex(Vertex* A,Vertex* B,Vertex* C,
		Edge* EA,Edge* EB,Edge* EC)
  /*!
    Sets the vertex points BUT not the other face
    Points are in counter clockwize order
    \param A :: First point
    \param B :: Second point
    \param C :: Third point
    \param EA :: First Edge
    \param EB :: Second Edge
    \param EC :: Third Edge
  */
{
  FPts[0]=A;
  FPts[1]=B;
  FPts[2]=C;

  ELines[0]=EA;
  ELines[1]=EB;
  ELines[2]=EC;
  
  // This sets the edge vertexes:
  if (A && B && C)
    {
      EA->setEndPts(A,B);
      EB->setEndPts(B,C);
      EC->setEndPts(C,A);
    }
  return;
}

Edge*
Face::getEdge(const size_t Index) 
  /*!
    Access an edge			      
    \param Index :: Index value of edge [0-2]
    \return Edge Ptr
  */
{
  if (Index>2)
    throw ColErr::IndexError<size_t>(Index,2,"Face::getEdge");
  return ELines[Index];
}

Vertex*
Face::getVertex(const size_t Index)
  /*!
    Gets the vertex associated with the index
    \param Index :: Inex of face point to access
    \return Face Point [Index]
   */
{
  if (Index>2)
    throw ColErr::IndexError<size_t>(Index,2,"Face::getVertex");
  return FPts[Index];
}

const Vertex*
Face::getVertex(const size_t Index) const
  /*!
    Gets the vertex associated with the index
    \param Index :: Inex of face point to access
    \return Face Point [Index]
   */
{
  if (Index>2)
    throw ColErr::IndexError<size_t>(Index,2,"Face::getVertex const");
  return FPts[Index];
}

Vec3D
Face::getNormal() const
  /*!
    Gets the normal associated with the Face assuming
    the points are counter-clockwize. 
    The normal points outwards, e.g. to a viewable direction
    \return Normal 
  */
{
  const Geometry::Vec3D& Orig=FPts[0]->getV();
  const Geometry::Vec3D PtA=FPts[1]->getV()-Orig;
  const Geometry::Vec3D PtB=FPts[2]->getV()-Orig;
  return (PtB*PtA).unit();
}

int
Face::volumeSign(const Geometry::Vertex& V) const
  /*!
    Calculates the volume on the face relative 
    to the point. Counter-clockwize direction
    This is the same as the orientation code
    \param V :: Vertex to calculate volume realtive
    \return -1/0/1 
  */
{
  return volumeSign(V.getV());
}

int
Face::volumeSign(const Geometry::Vec3D& V) const
  /*!
    Calculates the volume on the face relative 
    to the point. Counter-clockwize direction
    \param V :: Point to calculate volume realtive
    \return -1/0/1 
  */
{
  const Geometry::Vec3D PtA=FPts[0]->getV()-V;
  const Geometry::Vec3D PtB=FPts[1]->getV()-V;
  const Geometry::Vec3D PtC=FPts[2]->getV()-V;
  const double Volume=PtA.dotProd(PtB*PtC);
  if (Volume>Geometry::shiftTol) return 1;
  if (Volume<-Geometry::shiftTol) return -1;
  return 0;
}

void
Face::makeCCW(Edge* EPtr,Vertex* Point)
  /*!  
    Make the face's vertexes counter clockwise
    relative to the point of view
    \param Point :: Point to view face from
    \param EPtr :: Edge to face (defined first point)
  */
{
  ELog::RegMethod RegA("Face","makeCCW");
  Face* visFace(EPtr->getVisibleFace());

  // This ensures that we get a face with the same sense as the visible face:
  size_t index;
  for(index=0;index<3 && 
	visFace->getVertex(index) != EPtr->getEndPt(0);index++) ;

  if ( visFace->getVertex((index+1) % 3) == EPtr->getEndPt(1))
    {
      FPts[0]=EPtr->getEndPt(0);
      FPts[1]=EPtr->getEndPt(1);
    }
  else
    {
      FPts[1]=EPtr->getEndPt(0);
      FPts[0]=EPtr->getEndPt(1);
    }

   // Obviously make the third vertex the projection point
  FPts[2]=Point;
  return;
}

int
Face::calcTriGrp(const Face& A,TriGrp& GA) const
  /*!
    Calculate the triGrp value if the system is 
    a possible.		
    \param A :: Opposite Face
    \param GA :: TriGrp to populate
    \retval -1 :: No intersection possible
    \retval 0 :: Colinear
    \retval 1 :: Normal
   */
{
  ELog::RegMethod RegA("Face","calcTriGrp");

  const Vec3D& p = FPts[0]->getV();
  const Vec3D& q = FPts[1]->getV();
  const Vec3D& r = FPts[2]->getV();
  const Vec3D& a = A.FPts[0]->getV();
  const Vec3D& b = A.FPts[1]->getV();
  const Vec3D& c = A.FPts[2]->getV();
  
   // Compute distance signs  of p, q and r to the plane of triangle(a,b,c)
  const Vec3D DP=(b-a)*(c-a);
  double pd=DP.dotProd(p-a);
  const int sP=(fabs(pd)<Geometry::zeroTol) ? 0 : ((pd>0) ? 1 : -1);
  pd=DP.dotProd(q-a);
  const int sQ=(fabs(pd)<Geometry::zeroTol) ? 0 : ((pd>0) ? 1 : -1);
  pd=DP.dotProd(r-a);
  const int sR=(fabs(pd)<Geometry::zeroTol) ? 0 : ((pd>0) ? 1 : -1);

  if (sP==1)
    {
      if (sQ==1)
	{
	  if (sR==1) return -1; // NO intersection
	  GA(q,r,r,p);     // bcca
	}
      else   // SQ == 0/-1
	{
	  if (sR==1)
	    GA(p,q,q,r);   // abbc
	  else
	    GA(p,q,r,p);   // abca
	}
    }
  else if (sP==-1)
    {
      if (sQ==-1)
	{
	  if (sR==-1) return -1;     // NO intersection
	  GA(r,p,q,r);               // cabc
	}
      else  // SQ == 1/0
	{
	  if (sR==-1)
	    GA(q,r,p,q);              //bcab
	  else
	    GA(r,p,p,q);              //caab
	}
    }
  else    // COLINEAR in P
    {
      if (sQ==1)
	{
	  if (sR==1)
	    GA(r,p,p,q);  // caab
	  else 
	    GA(q,r,p,q);  //bcab
	}
      else if (sQ==-1)
	{
	  if (sR==-1)
	    GA(p,q,r,p);
	  else 
	    GA(p,q,q,r);
	}
      else        // COLINEAR in Q
	{
	  if (sR==1)
	    GA(r,p,q,r);
	  else if (sR==-1)
	    GA(q,r,r,p);
	  else 
	    return 0;      // COLINEAR
	}
    }
  return 1;          // INTERTANGLED
}
int
Face::calcTriGrpCase(const Face& A,TriGrp& GA) const
  /*!
    Calculate the triGrp value if the system is 
    a possible.		
    \param A :: Opposite Face
    \param GA :: TriGrp to populate
    \retval -1 :: No intersection possible
    \retval 0 :: Colinear
    \retval 1 :: Normal
   */
{
  ELog::RegMethod RegA("Face","calcTriGrp");
  const Vec3D& p = FPts[0]->getV();
  const Vec3D& q = FPts[1]->getV();
  const Vec3D& r = FPts[2]->getV();
  const Vec3D& a = A.FPts[0]->getV();
  const Vec3D& b = A.FPts[1]->getV();
  const Vec3D& c = A.FPts[2]->getV();
  
   // Compute distance signs  of p, q and r to the plane of triangle(a,b,c)
  const Vec3D DP=(b-a)*(c-a);
  double pd=DP.dotProd(p-a);
  const int sP=(fabs(pd)<Geometry::zeroTol) ? 0 : ((pd>0) ? 32 : 16);
  pd=DP.dotProd(q-a);
  const int sQ=(fabs(pd)<Geometry::zeroTol) ? 0 : ((pd>0) ? 8 : 4);
  pd=DP.dotProd(r-a);
  const int sR=(fabs(pd)<Geometry::zeroTol) ? 0 : ((pd>0) ? 2 : 1);

  const int flag=sP+sQ+sR;

  switch (flag)
    {
    case 42:      // +++
      return -1;
    case 41:      // ++-
    case 40:      // ++0
      GA(q,r,r,p);
      return 1;
    case 38:      // +-+
    case 34:      // +0+
      GA(p,q,q,r);
      return 1;
    case 32:      // +00
    case 33:      // +0-
    case 36:      // +-0
    case 37:      // +--
      GA(p,q,r,p);
      return 1;
    // NEGATIVE:
    case 21:      // ---
      return -1; 
    case 20:      // --0
    case 22:      // --+
      GA(r,p,q,r);
      return 1;
    case 17:      // -0-
    case 25:      // -+-
      GA(q,r,p,q);
      return 1;
    case 16:         // -00 
    case 18:         // -0+
    case 24:         // -+0 
    case 26:         // -++
      GA(r,p,p,q);
      return 1;

    // COPLANAR
    case 10:         // 0++
      GA(r,p,p,q);
      return 1;
    case 8:         // 0+0 
    case 9:         // 0+- 
      GA(q,r,p,q);
      return 1;
    case 5:        // 0--
      GA(p,q,r,p);
      return 1;
    case 4:        // 0-0
    case 6:        // 0-+
      GA(p,q,q,r);
      return 1;
    case 2:        // 00+
      GA(r,p,q,r);
      return 1;
    case 1:        // 00-
      GA(q,r,r,p);
    case 0:        // 000     [ ALL COPLANAR]
      return 0;

    }  
  throw ColErr::InContainerError<int>(flag,"Flag");
}

int
Face::orientation(const Vec3D& a,const Vec3D& b,
		  const Vec3D& c,const Vec3D& d)
  /*!
    Function to determine the orientation of a point relative to a triangle
    \param a :: Point of base triangle
    \param b :: Point of base triangle
    \param c :: Point of base triangle
    \param d :: Test point
    \return orientation +/-1 or zero on coplanar
  */
{
  const Vec3D DP=(b-a)*(c-a);
  const double pd=DP.dotProd(d);
  return (fabs(pd)<Geometry::zeroTol) ? 0 : ((pd>0) ? 1 : -1);
}

int
Face::orientation(const Vec3D& a,const Vec3D& b,const Vec3D& c)
  /*!
    Function to determine the orientation of a triangle
    \param a :: Point of base triangle
    \param b :: Point of base triangle
    \param c :: Point of base triangle
    \retval 1 :: Counter clockwize / -1 if clockwize
  */
{
  const Vec3D DP=(b-a)*(c-a);
  if (DP.abs()<Geometry::zeroTol) return 0;
  
  if ( fabs(DP.Z())>Geometry::zeroTol )
    return (DP.Z()>0) ? 1 : -1;
  if ( fabs(DP.Y())>Geometry::zeroTol )
    return (DP.Y()>0) ? 1 : -1;

  return (DP.X()>0) ? 1 : -1;
}

bool
Face::checkAcuteAngle(const Vec3D& A,const Vec3D& O,const Vec3D& B)
  /*!
    Test if an angle is accute
    \param A :: A Point 
    \param O :: Origin Point 
    \param B :: B Point 
    \return 1 if accute
   */
{
  ELog::RegMethod RegA("Face","checkAcuteAngle");

  ELog::EM<<"ACCUTE == "<<A<<":"<<O<<":"<<B<<ELog::endWarn;
  const Geometry::Vec3D nA=(A-O).unit();
  const Geometry::Vec3D nB=(B-O).unit();

  return (nA.dotProd(nB)>0) ? 1 : 0;
}

bool
Face::checkNormAngle(const Vec3D& N,const Vec3D& A,
		     const Vec3D& O,const Vec3D& B)
  /*!
    Check that the two lines AO and OB are on different sides
    of the normal (Point O is on the plane)
    \param N :: Normal 
    \param A :: A Point 
    \param O :: Origin Point 
    \param B :: B Point 
    \return 1 if true / 0 if flase
   */
{
  const Geometry::Vec3D nA=(A-O);
  const Geometry::Vec3D nB=(B-O);
  return (nA.dotProd(N)*nB.dotProd(N)<0) ? 1 : 0;
}

int
Face::intersect(const Face& A) const
  /*!
    Test the intersection of two faces
    \param A :: Other Face to determine if intercept
    \return 1 on intersection
  */
{
  ELog::RegMethod RegA("Face","intersect");

    // Structure for this:
  TriGrp GA;
  TriGrp GB;
  const int flag=calcTriGrpCase(A,GA);
  if (flag<0) return 0;
  if (flag==0) 
    { 
      ELog::EM<<"Colinear"<<ELog::endCrit; 
      return colinearIntersect(A); 
    }
  
  if (A.calcTriGrpCase(*this,GB)<0) return 0;

  const int OA=orientation(*GA.sMin,*GA.tMin,*GB.sMin,*GB.tMin);
  const int OB=orientation(*GA.sMax,*GA.tMax,*GB.tMax,*GB.sMax);
  ELog::EM<<"OA == "<<OA<<" "<<OB<<ELog::endWarn;
  ELog::EM<<"GA == "<<*GA.sMin<<":"<<*GA.sMax<<":"
	  <<*GA.tMin<<":"<<*GA.tMax<<ELog::endTrace;
  ELog::EM<<"GB == "<<*GB.sMin<<":"<<*GB.sMax<<":"
	  <<*GB.tMin<<":"<<*GB.tMax<<ELog::endTrace;
    
  return (orientation(*GA.sMin,*GA.tMin,*GB.sMin,*GB.tMin)!=1  &&
	  orientation(*GA.sMax,*GA.tMax,*GB.tMax,*GB.sMax)!=1 );
}

int
Face::colinearIntersect(const Face& A) const
  /*!
    If This and A and both colinear.
    Determine if they intersect
    \param A :: Other face to test
    \return 1 on intersection
  */
{
  const Vec3D* p = &FPts[0]->getV();
  const Vec3D* q = &FPts[1]->getV();
  const Vec3D* r = &FPts[2]->getV();
  
  const Vec3D* a = &A.FPts[0]->getV();
  const Vec3D* b = &A.FPts[1]->getV();
  const Vec3D* c = &A.FPts[2]->getV();

  if (orientation(*p,*q,*r)<0) 
    std::swap(q,r);
  if (orientation(*a,*b,*c)<0) 
    std::swap(b,c);
      
  if (orientation(*a,*b,*p)!=-1)
    {
      if (orientation(*b,*c,*p)!=-1)
	{
	  if (orientation(*c,*a,*p)!=-1)     // p is inside
	    return 1;
	  return testEdge(*p,*q,*r,*a,*b,*c);  // p sees ac
	}
      if (orientation(*c,*a,*p)!=-1)  // p sees bc
	return testEdge(*p,*q,*r,*c,*a,*b);
      return testVertex(*p,*q,*r,*a,*b,*c);
    }

  if (orientation(*b,*c,*p) != -1) 
    {
      if (orientation(*c,*a,*p) != -1 ) //p sees ab
	return testEdge(*p,*q,*r,*b,*c,*a);
      return testVertex(*p,*q,*r,*b,*c,*a);  // p sees a
    } 
  // p sees b
  return testVertex(*p,*q,*r,*c,*a,*b);
}

int
Face::testVertex(const Vec3D& p,const Vec3D& q,const Vec3D& r,
		 const Vec3D& a,const Vec3D& b,const Vec3D& c)
  /*!
    Test if the point is the wrong side of a point
    \param p :: Triangle 1 first point
    \param q :: Triangle 1 second point
    \param r :: Triangle 1 third point
    \param a :: Triangle 2 first point
    \param b :: Triangle 2 second point
    \param c :: Triangle 2 third point
    \return true if edge validates point
  */
{
  if (orientation(c,a,q) != -1) 
    {
      if (orientation(c,b,q) != 1) 
	{
	  if (orientation(p,a,q) == 1) 
	    return  (orientation(p,b,q) != 1);  
      
	  return orientation(p,a,r) != -1
	    && orientation(q,r,a) != -1;
	}
      
      if (orientation(p,b,q) != 1)
	return (orientation(c,b,r) != 1 &&
		orientation(q,r,b) != -1);
      return 0;
    }

  if (orientation(c,a,r) != -1) //qr straddles (ac) 
    { 
      if (orientation(q,r,c) != -1)
	return (orientation(p,a,r) != -1);
      
      return (orientation(q,r,b) != -1 &&
	      orientation(c,r,b) != -1);
    }

  return 0; // ca separes
}

int
Face::planeIntersect(const Face& B) const
  /*!
    Assume that the planes are parallel and find if the
    system intersects
    \param B :: Other triangle
    \return true if the triangles intersect
  */
{
  ELog::RegMethod RegA("Face","planeIntersect");
  // Check that no point in within the triangle:
  for(int i=0;i<3;i++)
    {
      if (flatValid(B.FPts[i]->getV())==1)
	return 1;

      if (B.flatValid(FPts[i]->getV())==1)
	return 1;
    }

  // The only OTHER condition is that the triangle forms
  // a star.
  // -- In this case TWO of the lines must double intersect:
  // Therefore only test 2 lines:

  // Both lines start from the same origin:
  // solve for beta: (q+ b u = poit)
  const Vec3D& p=FPts[0]->getV();
  const Vec3D& q=B.FPts[0]->getV();
  const Vec3D p_q=p-q;
  for(int i=0;i<2;i++)
    {
      const Vec3D u=FPts[i+1]->getV()-p;
      const Vec3D v=B.FPts[i+1]->getV()-q;
      const double dUV=u.dotProd(v);
      const double dUU=u.dotProd(u);
      const double dVV=v.dotProd(v);
      const double dUPQ=u.dotProd(p_q);
      const double dVPQ=v.dotProd(p_q);
      const double scale(dUU*dVV-dUV*dUV);
      if (scale>Geometry::zeroTol)
	{
	  const double beta=(dUU*dVPQ-dUV*dUPQ)/scale;
	  if (beta<1.0-Geometry::zeroTol && beta>Geometry::zeroTol)
	    {
	      const double alpha=(dUV*dVPQ-dVV*dUPQ)/scale;
	      if (alpha<1.0-Geometry::zeroTol && alpha>Geometry::zeroTol)
		{
		  return 1;
		}
	    }
	}
    }
  return -1;
}

int
Face::Intersect(const Face& B) const
  /*!
    Intersect between two facece
    \param B :: Face 
    \retval 1 Intersect
    \return -1 Does not intercept
  */
{
  ELog::RegMethod RegA("Face","Intersect");

  const Vec3D& a = FPts[0]->getV();
  const Vec3D& b = FPts[1]->getV();
  const Vec3D& c = FPts[2]->getV();

  const Vec3D& p = B.FPts[0]->getV();
  const Vec3D& q = B.FPts[1]->getV();
  const Vec3D& r = B.FPts[2]->getV();

  // Calculate the two planes:
  Vec3D NormA=((b-a)*(c-a)).unit();
  Vec3D NormB=((q-p)*(r-p)).unit();
  const double NAB=NormA.dotProd(NormB);
  const double DA=a.dotProd(NormA);
  const double DB=p.dotProd(NormB);

  // This needs to call colinear
  if (fabs(NAB)>1.0-Geometry::zeroTol &&
      ( (NAB>0.0 && fabs(DA-DB)<Geometry::zeroTol)  ||
	(NAB<0.0 && fabs(DA+DB)<Geometry::zeroTol) ) )
    {
      return planeIntersect(B);
    }

  // Loop over the three lines:
  for(int i=0;i<3;i++)
    {
      const Vec3D& ax=FPts[i]->getV();
      const Vec3D& ab=FPts[(i+1) % 3]->getV()-FPts[i]->getV();
      const double abDot=ab.dotProd(NormB);
      if (fabs(abDot)>Geometry::zeroTol)
	{
	  const double lambda = (DB-NormB.dotProd(ax))/abDot;
	  // Did point hit:
	  if (lambda>Geometry::zeroTol && lambda<1.0-Geometry::zeroTol)
	    {
	      const Vec3D Pt=ax+ab*lambda;
	      if (B.flatValid(Pt)==1)
		return 1;
	    }
	  // SPECAL CASE [1]:
	  else if (lambda>-Geometry::zeroTol && lambda<Geometry::zeroTol 
		   && checkNormAngle(NormB,ax+ab,ax,FPts[(i+2) % 3]->getV()) 
		   && B.flatValid(ax)==1)
	    return 1;
	  else if (lambda>1.0-Geometry::zeroTol && lambda<1.0+Geometry::zeroTol 
		   && checkNormAngle(NormB,ax,ax+ab,FPts[(i+2) % 3]->getV()) 
		   && B.flatValid(ax+ab)==1)
	    return 1; 
	}
    }
  // Loop over the other three lines:
  for(int i=0;i<3;i++)
    {
      const Vec3D& ax=B.FPts[i]->getV();
      const Vec3D& ab=B.FPts[(i+1) % 3]->getV()-B.FPts[i]->getV();
      const double abDot=ab.dotProd(NormA);
      if (fabs(abDot)>Geometry::zeroTol)
	{
	  const double lambda = (DA-NormA.dotProd(ax))/abDot;
	  // ELog::EM<<"lambda["<<i<<"] == "<<lambda<<" :: "
	  // 	  <<ax+ab*lambda<<ELog::endTrace;

	  // Did point hit:
	  if (lambda>Geometry::zeroTol && lambda<1.0-Geometry::zeroTol)
	    {
	      const Vec3D Pt=ax+ab*lambda;

	      if (flatValid(Pt)==1)
		return 1;
	    }
	  else if (lambda>-Geometry::zeroTol && lambda<Geometry::zeroTol 
		   && checkNormAngle(NormA,ax+ab,ax,B.FPts[(i+2) % 3]->getV())
		   && flatValid(ax)==1)
	    return 1;
	  else if (lambda>1.0-Geometry::zeroTol && lambda<1.0+Geometry::zeroTol 
		   && checkNormAngle(NormA,ax,ax+ab,B.FPts[(i+2) % 3]->getV()) 
		   && flatValid(ax+ab)==1)
	    return 1; 

	}
    }
  return -1;
}

int
Face::flatValid(const Geometry::Vec3D& Pt) const
  /*!
    Carrys out an effective point in trianglecalculate.
    Finds the projection of the sides of the triangle 
    within the point.
    \param Pt :: Point
    \retval 1 if inside the triangle
    \retval 0 if on the edge
    \retval -1 if outside the triangle
   */
{
  // // Quick check as to in plane:
  const Vec3D c=Pt-FPts[0]->getV();
  // if (NormV.dotProd(Pt)>Geometry::shiftTol)
  //   return 0;

  const Vec3D a=FPts[2]->getV()-FPts[0]->getV();
  const Vec3D b=FPts[1]->getV()-FPts[0]->getV(); 

  const double daa=a.dotProd(a);
  const double dab=a.dotProd(b);
  const double dac=a.dotProd(c);
  const double dbb=b.dotProd(b);
  const double dbc=b.dotProd(c);
  
  const double nVal = (daa*dbb-dab*dab);         
  if (nVal<Geometry::zeroTol)
    return 0;
  const double u = (dbb*dac - dab*dbc)/nVal;
  const double v = (daa*dbc - dab*dac)/nVal;
  const double uPv(u+v);

  if (u>Geometry::zeroTol && v>Geometry::zeroTol &&
      uPv < 1.0-Geometry::zeroTol)
    return 1;
  // on the line A:

  if (fabs(v)<Geometry::zeroTol && 
      u>-Geometry::zeroTol &&  u<1.0+Geometry::zeroTol)
      return 0;
  // on the line B:
  if (fabs(u)<Geometry::zeroTol && 
      v>-Geometry::zeroTol &&  v<1.0+Geometry::zeroTol)
      return 0;
  // on the line C:
  if (u>-Geometry::zeroTol && v>-Geometry::zeroTol &&
      uPv>1.0-Geometry::zeroTol && uPv<1.0+Geometry::zeroTol)
      return 0;
  return -1;
}

int
Face::testEdge(const Vec3D& p,const Vec3D& q,const Vec3D& r,
	       const Vec3D& a,const Vec3D&,const Vec3D& c)
  /*!
    Test if and edge is the wrong side of a point
    \param p :: Triangle 1 first point
    \param q :: Triangle 1 second point
    \param r :: Triangle 1 third point
    \param a :: Triangle 2 first point
    \param :: Triangle 2 second point
    \param c :: Triangle 2 third point
    \return true if edge validates point
  */
{
  if (orientation(c,a,q) != -1) 
    {  //pq straddles (ac)
      if (orientation(p,a,q) != -1) 
	return (orientation(p,q,c) != -1) ;
      
      return ((orientation(q,r,a) != -1) &&
	      orientation(r,p,a) != -1);
    }
  
  if (orientation(c,a,r) != -1) 
    { 
      // pr and qr straddle line (pr)
      return (orientation(p,a,r) != -1 &&
	      (orientation(p,r,c) != -1 ||
	       orientation(q,r,c) != -1));
    }
  
  return 0;   //ac separates
}

void
Face::write(std::ostream& OX) const
  /*!
    Debug output writing 
    \param OX :: Output stream
   */
{
  for(int i=0;i<3;i++)
    {
      OX<<"V["<<i<<"]=";
      if (FPts[i])
	OX<<FPts[i]->getID()<<" ";
      else
	OX<<"Null";
    }
  for(int i=0;i<3;i++)
    {
      OX<<"E["<<i<<"]=";
      if (ELines[i])
	OX<<ELines[i]->getEndPt(0)->getID()<<":"
	  <<ELines[i]->getEndPt(1)->getID()<<" ";
      else
	OX<<0<<" ";
    }
  return;
}

} // NAMESPACE Geometry
  
