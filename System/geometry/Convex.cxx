/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   geometry/Convex.cxx
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
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <map>
#include <string> 
#include <algorithm>
#include <iterator>
#include <memory>
#include <functional>

#include "Exception.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Vertex.h"
#include "Edge.h"
#include "Face.h"
#include "Convex.h"


namespace Geometry
{

std::ostream&
operator<<(std::ostream& OX,const Convex& A)
  /*!
    Standard output stream
    \param OX :: Output stream
    \param A :: Convex to write
    \return Stream state
  */
{
  A.write(OX);
  return OX;
}

Convex::Convex() 
  /*! 
    Constructor
  */
{}

Convex::Convex(const std::vector<Geometry::Vec3D>& PVec) 
  /*!
    Constructor 
    \param PVec :: Points to add
  */
{
  setPoints(PVec);
}

Convex::Convex(const Convex& A) :
  Pts(A.Pts)
  /*!
    Copy Constructor - Does not copy 
    \param A :: Convex object 
  */
{
  // Set the vertex:
  std::vector<Vec3D>::const_iterator vc;
  for(vc=Pts.begin();vc!=Pts.end();vc++)
    {
      Vertex* NV=new Vertex(static_cast<int>(VList.size()),*vc);
      VList.push_back(NV);
    }

  ETYPE::const_iterator ec;
  for(ec=A.EList.begin();ec!=A.EList.end();ec++)
    {
      const Edge& AE= **ec;
      const long int EVa=(AE.getEndPt(0)) ? AE.getEndPt(0)->getID() : -1;
      const long int EVb=(AE.getEndPt(1)) ? AE.getEndPt(1)->getID() : -1;
      if (EVa>=static_cast<long int>(VList.size()) ||
	  EVb>=static_cast<long int>(VList.size()))
	{
	  for(ec=EList.begin();ec!=EList.end();ec++)
	    delete *ec;
	  EList.clear();
	  createAll(1);
	  return;
	}
      EList.push_back(new Edge);
      Vertex* VA=(EVa<0) ? 0 : VList[static_cast<size_t>(EVa)];
      Vertex* VB=(EVa<0) ? 0 : VList[static_cast<size_t>(EVa)];
      EList.back()->setEndPts(VA,VB);
    }
  
}

Convex&
Convex::operator=(const Convex& A) 
  /*! 
    Assignment constructor
    \param A :: Convex object 
    \return *this
  */
{
  ELog::RegMethod RegItem("Convex","operator=");
  
  if (this!=&A)
    {
      Pts=A.Pts;
      deleteAll();
    }
  return *this;
}

Convex::~Convex()
///< Deletion operator
{
  deleteAll();
}

void 
Convex::deleteAll()
  /*!
    Delete everything
  */
{  
  VTYPE::iterator vc;
  for(vc=VList.begin();vc!=VList.end();vc++)
    delete *vc;
  ETYPE::iterator ec;
  for(ec=EList.begin();ec!=EList.end();ec++)
    delete *ec;
  FTYPE::iterator fc;
  for(fc=FList.begin();fc!=FList.end();fc++)
    delete *fc;
  // Clear all:
  VList.clear();
  EList.clear();
  FList.clear();
  SurfList.clear();
  return;
}


void 
Convex::setPoints(const std::vector<Geometry::Vec3D>& PVec)
  /*!
    Sets the points
    \param PVec :: Vector of Points
   */
{  
  ELog::RegMethod RegItem("Convex","setPoints");
  Pts=PVec;
  deleteAll();

  for(const Vec3D& corner : Pts)
    {
      Vertex* NV=new Vertex(static_cast<int>(VList.size()),corner);
      VList.push_back(NV);
    }
  return;
}

void 
Convex::addPoints(const std::vector<Geometry::Vec3D>& PVec)
  /*!
    Adds to the set of points
    \param PVec :: Points
   */
{  
  ELog::RegMethod RegItem("Convex","addPoints");
  copy(PVec.begin(),PVec.end(),
       std::back_insert_iterator<std::vector<Geometry::Vec3D> >(Pts));
  setPoints(Pts);

  return;
}

void 
Convex::addPoint(const Geometry::Vec3D& PItem)
  /*!
    Add a point to the system
    \param PItem :: Point to add
   */
{  
  ELog::RegMethod RegItem("Convex","addPoint");

  Vertex* NV=new Vertex(static_cast<int>(VList.size()),PItem);
  if (Pts.empty() || Pts.back()!=PItem)
    {
      Pts.push_back(PItem);
      VList.push_back(NV);
    }
  else
    {
      ELog::EM<<"Adding Identical point"<<PItem<<ELog::endWarn;
      delete NV;
    }
  return;
}

void
Convex::createAll(const int Index)
  /*!
    Carry out the whole process from 
    \param Index :: Number to start surfaces from
  */
{
  if (!calcDTriangle())
    {
      constructHull();
      createSurfaces(Index);
    }
  return;
}

void
Convex::reduceSurfaces()
  /*!
    Remove like surfaces
  */
{
  ELog::RegMethod RegA("Convex","reduceSurface");

  if (!SurfList.empty())
    {
      // IF we had a way to sort surfaces this could be quicker:
      int SNum=SurfList.front().getName();
      
      std::vector<Plane>::iterator vc;
      for(size_t i=SurfList.size()-1;i>0;i--)
	{
	  const long int li(static_cast<long int>(i));
	  const Geometry::Plane& PL=SurfList[i];
	  vc=find_if(SurfList.begin(),SurfList.begin()+li,
		     std::bind(std::equal_to<Geometry::Plane>(),
			       std::placeholders::_1,PL));

	  if (vc != SurfList.begin()+li)
	    SurfList.erase(SurfList.begin()+li);
	}
      for(vc=SurfList.begin();vc!=SurfList.end();vc++)
	vc->setName(SNum++);
    }
  return;
  
}

void
Convex::constructHull()
  /*!
    Construct hull onto a completed Hull.
    Adds point by point
  */
{
  ELog::RegMethod RegItem("Convex","constructHull");

  for(;;)
    {
      Vertex* VPtr(0);
      for(Vertex* vc : VList)
	{
	  if (!vc->isDone())
	    {
	      VPtr=vc;
	      break;
	    }
	}

      if (!VPtr) return;
      VPtr->Done();
      addOne(VPtr);
      cleanUp();
    }
  return;
}

int
Convex::calcDTriangle()
 /*!
   Calculate Degulgry triangle
   \retval -ve on error 
   \retval Index of last point on success
  */
{
  ELog::RegMethod RegItem("Convex","calcDTriangle");

  if (Pts.size()<4)
    {
      ELog::EM<<"Insufficient points for convex hull:"
	      <<Pts.size()<<ELog::endCrit;
      return -1;
    }
  const size_t vMax(Pts.size());
  size_t vN(0);         // Vertex index

  // First find three non-colinear points  
  while(VList[vN % vMax]->getV().coLinear(VList[(vN+1) % vMax]->getV(),
					  VList[(vN+2) % vMax]->getV()))
    {
      vN++;
      if (vN==vMax)
        {
	  ELog::EM<<"All points conlinear:"<<ELog::endCrit;
	  return -2;
	}
    }
  for(int i=0;i<3;i++)
    EList.push_back(new Edge);
  Vertex* v0= VList[vN];
  Vertex* v1= VList[(vN+1) % vMax];
  Vertex* v2= VList[(vN+2) % vMax];
  v0->Done();
  v1->Done();
  v2->Done();
  
  // FIRST Face:
  FList.push_back(new Face(v0,v1,v2,EList[0],EList[1],EList[2]));
  // OPPOSITE Face :
  FList.push_back(new Face(v2,v1,v0,EList[2],EList[1],EList[0]));
  // Set Faces
  for(size_t i=0;i<3;i++)
    {
      Edge* EPtr=FList[0]->getEdge(i);
      EPtr->setAdjacentFaces(FList[0],FList[1]);
      //      EPtr=FList[1]->getEdge(i);
      //      EPtr->setAdjacentFaces(FList[1].get(),FList[0].get());
    }
  
  // FIND Fourth non-coplanar
  vN+=3;
  vN %= vMax;
  while(!FList[0]->volumeSign(*VList[vN]))
    {
      if (++vN==vMax)
        {
	  ELog::EM<<"All points coplanar with zero volume"
		  <<ELog::endCrit;
	  return -3;
	}
    }
  VList[vN]->Done();
  addOne(VList[vN]);
  cleanUp();  
  return 0;
}

int
Convex::addOne(Vertex* VPtr) 
  /*! 
    Object to add a vertex to the list (or at least try)
    - Loops through the face list to find 
      the faces that are visible.
    
    \param VPtr :: object 
    \retval 0 on failure to add
    \retval 1 :: success
  */
{
  ELog::RegMethod RegA("Convex","addOne");
  int visFlag(0);        // Check that one face is visible

  // Clear all cone edges
  for(Vertex* vc : VList)
    vc->setConeEdge(static_cast<Edge*>(0));

  for(Face* FPtr : FList)
    {
      if (FPtr->volumeSign(*VPtr)<0)
        {
	  FPtr->setVisible(1);
	  visFlag++;
	}
      else
	FPtr->setVisible(0);
    }
  
  // No visible faces therefore point must be in the hull
  if (!visFlag)
    {
      VPtr->setOnHull(0);
      return 0;
    }
  // Now remove excessive edges and faces 
  // Each edge shares two faces if both visible delete
  // else if one visible create new face based on that edge and the 
  // cone point
  const size_t ELsize=EList.size();
  for(size_t i=0;i<ELsize;i++)            // EList CHANGES SIZE in makeCone
    {
      Edge* EPtr=EList[i];
      if (EPtr->bothVisible())
	EPtr->markForDeletion();
      else if (EPtr->oneVisible())
        EPtr->setNewFace(makeCone(VPtr,EPtr));
    }
  return 1;
}

void
Convex::cleanUp()
  /*!
    Re-adjust the list of vertex/edge/faces
  */
{
  ELog::RegMethod RegItem("Convex","cleanUp");  

  cleanEdges();
  cleanFaces();
  cleanVertex();

  return;
}

void
Convex::cleanEdges()
  /*!
    Remove waste edges
  */
{
  ELog::RegMethod RegItem("Convex","cleanEdges");
  // Find edges that have a newFace
  ETYPE::iterator ec;
  for(ec=EList.begin();ec!=EList.end();ec++)
    {
      if ((*ec)->hasNewFace())        /// This test is redundent
	(*ec)->setVisibleFace();
    }

  ec=EList.begin();
  while(ec!=EList.end())
    {
      if ((*ec)->reqDel())
	{
	  delete *ec;
	  ec=EList.erase(ec);
	}
      else
	ec++;
    }
  
  return;
}

void
Convex::cleanFaces()
  /*!
    Remove waste faces
  */
{
  ELog::RegMethod RegItem("Convex","cleanFaces");

  // Delete and remove
  FTYPE::iterator fc=FList.begin();
  while(fc!=FList.end())
    {
      if ((*fc)->isVisible())
	{
	  delete *fc;
	  fc=FList.erase(fc);
	}
      else
	fc++;
    }
  return;
}

void
Convex::cleanVertex()
  /*!
    Remove waste faces
  */
{
  ELog::RegMethod RegItem("Convex","cleanVertex");
  // Set all
  for(Edge* ec : EList)
    ec->setOnHull();

  VTYPE::iterator vc=VList.begin();
  while(vc!=VList.end())
    {
      if ((*vc)->isNotOnHull())
	{
	  delete *vc;
	  vc=VList.erase(vc);
	}
      else
	vc++;
    }
	  
  return;
}
 

Face*
Convex::makeCone(Vertex* VPtr,Edge* EPtr)
  /*!
    Create a cone face. Note that this adds 
    a pointer to the EList. This invalidates any
    iterator. 
    
    \param VPtr :: Top of cone
    \param EPtr :: One line of the cone 
    \return Face created
  */
{
  ELog::RegMethod RegA("Convex","makeCone");

  Edge* newEdge[2];    // Array to store new edges for a bit (1 or 2 will be new)
  for(int i=0;i<2;i++)
    {
      // If we have already created the points?
      // and the cone edge 
      Vertex* EdgePoint=EPtr->getEndPt(i);
      newEdge[i]=EdgePoint->getConeEdge();       // sets 0 / ConeEdge
      
      if (!newEdge[i])
        {
	  // make new edge and connect between the end point and cone vertex.

	  Edge* NewEdge=new Edge;
	  EList.push_back(NewEdge);	  
	  NewEdge->setEndPts(EdgePoint,VPtr);
	  newEdge[i]=NewEdge;
	  EPtr->getEndPt(i)->setConeEdge(NewEdge);
	}
    }		     

  // Create the new face. (remember edges from above!)
  Face* NewFace=new Face(0,0,0,EPtr,newEdge[0],newEdge[1]);
  
  // Generate Vertex list (in counter-clockwise order)
  NewFace->makeCCW(EPtr,VPtr);
//  Set the adjacent face pointers. 
  for(int i=0;i<2;i++)
    for(int j=0;j<2;j++)
      {
	// Only one NULL link should be set per new_face. 
        if (!newEdge[i]->getAdjFace(j))
	  {
	    newEdge[i]->setAdjFace(j,NewFace);
	    break;
	  }
      }
  FList.push_back(NewFace);
  return NewFace;
}

void
Convex::createSurfaces(int startNumber) 
  /*!
    Create some surface based on the surfaces
    \param startNumber :: surface offset number
  */
{
  SurfList.clear();
  if (startNumber<1) 
    startNumber=1;
  FTYPE::const_iterator fc;
  for(fc=FList.begin();fc!=FList.end();fc++)
    {
      SurfList.push_back(Plane(startNumber++,0));
      const Geometry::Vec3D& PtA=(*fc)->getVertex(0)->getV();
      const Geometry::Vec3D& PtB=(*fc)->getVertex(1)->getV();
      const Geometry::Vec3D& PtC=(*fc)->getVertex(2)->getV();
      SurfList.back().setPlane(PtA,PtB,PtC);
    }
  // Remove overlaps here
  reduceSurfaces();
  return;
}

int
Convex::isConvex() const
 /*!
   Test if the cell is  convex
   \retval 0 :: Failure
   \retval 1 :: success
  */
{

  ELog::RegMethod RegA("Convex","isConvex");

  for(const FTYPE::value_type& FacePtr : FList)
    for(const VTYPE::value_type& VPtr : VList)
      {
        if (VPtr->isDone() && FacePtr->volumeSign(*VPtr)<0)
          {
	    ELog::EM<<"Not convex : VolSign="
                    <<FacePtr->volumeSign(*VPtr)<<"\n"
                    <<"  at "<<*VPtr<<"\n"
                    "Face == "<<*FacePtr<<ELog::endErr;
            return 0;
          }
      }
  // check edges:
  for(const ETYPE::value_type& EdgePtr : EList)
    if (!EdgePtr->getAdjFace(0) || !EdgePtr->getAdjFace(1))
      {
	ELog::EM<<"Failed on Edge "<<*EdgePtr<<ELog::endErr;
	return 0;
      }
  return 1;
}

int
Convex::inHull(const Geometry::Vec3D& Pt) const
 /*!
   Test if the cell is within the hull
   \param Pt :: test Point
   \retval 0 :: Failure
   \retval 1 :: success
  */
{
  FTYPE::const_iterator fc;
  for(fc=FList.begin();fc!=FList.end();fc++)
    if ( (*fc)->volumeSign(Pt)<0)
      return 0;

  return 1;
}

int
Convex::getNonMatch(std::vector<const Face*>& OFaces) const
  /*!
    Determines those faces that don't correspond to an
    existing surface. Also removes self similar faces
    \param OFaces :: Other Faces
    \return number of non-unique faces
   */
{
  ELog::RegMethod RegA("Convex","getNonMatch");

  std::vector<const Face*>::iterator fc=OFaces.begin();
  std::vector<Plane>::const_iterator vc;
  Plane AP(1,0);
  std::vector<Plane> foundSet;
  while(fc!=OFaces.end())
    {
      const Geometry::Vec3D& PtA=(*fc)->getVertex(0)->getV();
      const Geometry::Vec3D& PtB=(*fc)->getVertex(1)->getV();
      const Geometry::Vec3D& PtC=(*fc)->getVertex(2)->getV();
      AP.setPlane(PtA,PtB,PtC);
      vc=find_if(foundSet.begin(),foundSet.end(),
		 std::bind(&Geometry::Plane::isEqual,
			   std::placeholders::_1,AP));
      if (vc!=foundSet.end())
	fc=OFaces.erase(fc);
      else
	{
	  vc=find_if(SurfList.begin(),SurfList.end(),
		     std::bind(&Geometry::Plane::isEqual,
			       std::placeholders::_1,AP));
	  if (vc!=SurfList.end())
	    fc=OFaces.erase(fc);
	  else
	    {
	      foundSet.push_back(AP);
	      fc++;
	    }
	}
    }
  return static_cast<int>(OFaces.size());
}


int
Convex::intersectHull(const Convex& A) const
 /*!
   Test if the two constructed hull intersect
   Currently only test to see if the points are within
   the hull. The better test is to construct all the 
   planes and test all intersections:
   \param A :: Convex Hull to test
   \retval 0 :: Failure
   \retval Face number+1 :: success
  */
{
  ELog::RegMethod RegA("Convex","intersectHull");
  FTYPE::const_iterator vc;
  FTYPE::const_iterator ac;
  for(vc=FList.begin();vc!=FList.end();vc++)
    for(ac=A.FList.begin();ac!=A.FList.end();ac++)
      {
	if ((*vc)->Intersect(**ac))
	  return 1+static_cast<int>(distance(A.FList.begin(),ac));
      }
  return 0;
}

int
Convex::intersectHull(const Convex& A,
		      std::vector<const Face*>& FSet) const
 /*!
   Test if the two constructed hull intersect
   Currently only test to see if the points are within
   the hull. The better test is to construct all the 
   planes and test all intersections:
   \param A :: Convex Hull to test
   \param FSet :: Faces that intersect
   \retval 0 :: Failure
   \retval Face number+1 :: success
  */
{
  ELog::RegMethod RegA("Convex","intersectHull");
  FTYPE::const_iterator vc;
  FTYPE::const_iterator ac;

  for(vc=FList.begin();vc!=FList.end();vc++)
    for(ac=A.FList.begin();ac!=A.FList.end();ac++)
      {
	if ((*vc)->Intersect(**ac)==1)
	  FSet.push_back(*ac);
      }

  return (FSet.empty()) ? 0 : 1;
}

int
Convex::inSurfHull(const Geometry::Vec3D& Pt) const
 /*!
   Test if the cell is within the hull using surface
   \param Pt :: test Point
   \retval 0 :: Failure
   \retval 1 :: success
  */
{
  std::vector<Plane>::const_iterator pc;
  for(pc=SurfList.begin();pc!=SurfList.end();pc++)
    if (pc->side(Pt)<0)
      return 0;
  // Success
  return 1;
}

int
Convex::validEdge() const
  /*!
    Checks to see if we have valid edges [after cleanup]
    \retval 0 :: Valid
    \retval Index + 1 of EList in error
   */
{
  ETYPE::const_iterator ec;
  for(ec=EList.begin();ec!=EList.end();ec++)
    if (!(*ec)->getAdjFace(0) || 
	!(*ec)->getAdjFace(1) )
      return static_cast<int>(distance(EList.begin(),ec))+1000;
  return 0;
}

void
Convex::writeFaces(std::ostream& OX) const
  /*!
    Write out all the faces
    \param OX :: Output stream
  */
{
  FTYPE::const_iterator fc;
  for(fc=FList.begin();fc!=FList.end();fc++)
    OX<<**fc<<std::endl;
  return;
}

void
Convex::write(std::ostream& OX) const
  /*!
    Write out the convex hull
    \param OX :: Output stream
   */
{
  OX<<"----------   POINTS: ----------------"<<std::endl;
  VTYPE::const_iterator vc;
  for(vc=VList.begin();vc!=VList.end();vc++)
    {
      (*vc)->write(OX);
      OX<<std::endl;
    }
  OX<<"----------   FACES: ----------------"<<std::endl;
  FTYPE::const_iterator fc;
  for(fc=FList.begin();fc!=FList.end();fc++)
    {
      (*fc)->write(OX);
      OX<<std::endl;
    }
  
  return;
}


} // NAMESPACE Geometry
  
