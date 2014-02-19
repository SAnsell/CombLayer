/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/Torpedo.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Tensor.h"
#include "Vec3D.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "Vertex.h"
#include "Face.h"
#include "Convex.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "shutterBlock.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "InsertComp.h"
#include "ContainedComp.h"
#include "LinearComp.h"
#include "GeneralShutter.h"
#include "Torpedo.h"

#include "Debug.h"

namespace shutterSystem
{

Torpedo::Torpedo(const size_t ID,const std::string& Key) : 
  FixedComp(Key,6),shutterNumber(ID),
  surfIndex(ModelSupport::objectRegister::Instance().
	    cell(Key+StrFunc::makeString(ID))),
  cellIndex(surfIndex+1),populated(0)
  /*!\
    Constructor BUT ALL variable are left unpopulated.
    \param ID :: Shutter number
    \param Key :: Variable keyword 
  */
{}

Torpedo::Torpedo(const Torpedo& A) : 
  attachSystem::FixedComp(A),attachSystem::InsertComp(A),
  shutterNumber(A.shutterNumber),surfIndex(A.surfIndex),
  cellIndex(A.cellIndex),vBox(A.vBox),
  populated(A.populated),voidXoffset(A.voidXoffset),
  xyAngle(A.xyAngle),innerRadius(A.innerRadius),zOffset(A.zOffset),
  Height(A.Height),Width(A.Width),innerSurf(A.innerSurf),
  voidCell(A.voidCell)
  /*!
    Copy constructor
    \param A :: Torpedo to copy
  */
{}

Torpedo&
Torpedo::operator=(const Torpedo& A)
  /*!
    Assignment operator
    \param A :: Torpedo to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::InsertComp::operator=(A);
      cellIndex=A.cellIndex;
      vBox=A.vBox;
      populated=A.populated;
      voidXoffset=A.voidXoffset;
      xyAngle=A.xyAngle;
      innerRadius=A.innerRadius;
      zOffset=A.zOffset;
      Height=A.Height;
      Width=A.Width;
      innerSurf=A.innerSurf;
      voidCell=A.voidCell;
    }
  return *this;
}

Torpedo::~Torpedo() 
  /*!
    Destructor
  */
{}
				
void
Torpedo::populate(const Simulation& System,
		     const shutterSystem::GeneralShutter& GS)
  /*!
    Populate all the variables
    \param System :: Simulation to use
    \param GS :: General shutter that aligns with this torpedo
  */
{
  ELog::RegMethod RegA("Torpedo","populate");
  const FuncDataBase& Control=System.getDataBase();

  // Global from shutter size:

  voidXoffset=Control.EvalVar<double>("voidXoffset");
  innerRadius=Control.EvalVar<double>("bulkShutterRadius");
  xyAngle=GS.getAngle();
  
  zOffset=SimProcess::getIndexVar<double>
    (Control,keyName,"ZOffset",shutterNumber+1);
  Width=SimProcess::getIndexVar<double>
    (Control,keyName,"Width",shutterNumber+1);
  Height=SimProcess::getIndexVar<double>
    (Control,keyName,"Height",shutterNumber+1);
  
  populated|=1;
  return;
}

void
Torpedo::createUnitVector(const shutterSystem::GeneralShutter& GS)
  /*!
    Create unit vectors for shutter along shutter direction
    - Z is gravity 
    - Y is beam XY-axis
    - X is perpendicular to XY-Axis
    \param GS :: General shutter
  */
{
  ELog::RegMethod RegA("Torpedo","createUnitVector");
  
  attachSystem::FixedComp::createUnitVector(GS.getOrigin(),
					    GS.getXYAxis(),
					    GS.getZ());
  setExit(GS.getOrigin()+GS.getXYAxis()*innerRadius,GS.getXYAxis());
  return;
}

void
Torpedo::calcVoidIntercept(const attachSystem::ContainedComp& CC)
  /*!
    Determine what surfaces need to be on the inside edge of the 
    torpedo tube
    \param CC :: Contained object 
   */
{
  ELog::RegMethod RegA("Torpedo","calcVoidIntercept");
  // Clear set
  innerSurf.erase(innerSurf.begin(),innerSurf.end());
  
  // Create 4 lines: 
  for(int i=0;i<4;i++)
    {
      const double zScale( (i % 2) ? -Height/2.0 : Height/2.0);
      const double xScale( (i / 2) ? -Width/2.0 : Width/2.0);
      // Advance origin from voidVessel middle to end point
      // and then look back
      const Geometry::Vec3D OP=Origin+Y*1000.0+Z*zScale+X*xScale;
      const Geometry::Line LA(OP,-Y);
      const int surfN=CC.surfOuterIntersect(LA);
      if (surfN)
	innerSurf.insert(-surfN);
    }
  return;
}


void
Torpedo::setExternal(const int rInner)
  /*!
    Set the external surfaces
    \param rInner :: inner cylinder surface
  */
{
  SMap.addMatch(surfIndex+7,rInner);
  // FixedComp::setExitSurf(SMap.realSurf(surfIndex+7));
  return;
}

void
Torpedo::calcConvex(Simulation& System)
  /*!
    Calc convex objects for the torpedo
    \param System :: System object
  */
{
  ELog::RegMethod RegA("Torpedo","calcConvex");

  MonteCarlo::Qhull* VC=System.findQhull(voidCell);
  if (VC)
    {
      VC->calcVertex();
      vBox.setPoints(VC->getVertex());
      vBox.createAll(1);
    }
  return;
}

void
Torpedo::createSurfaces()
  /*!
    Create all the surfaces that are normally created 
  */
{
  ELog::RegMethod RegA("Torpedo","createSurfaces");

  ModelSupport::buildPlane(SMap,surfIndex+3,Origin-X*(Width/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+4,Origin+X*(Width/2.0),X);
  ModelSupport::buildPlane(SMap,surfIndex+5,Origin+Z*(zOffset-Height/2.0),Z);
  ModelSupport::buildPlane(SMap,surfIndex+6,
			   Origin+Z*(zOffset+Height/2.0),Z);

  return;
}
  
std::string
Torpedo::getInnerSurf() const
  /*!
    Get the interal string
    \return string of surfaces
  */
{
  std::ostringstream cx;

  // Large number of objects
  std::set<int>::const_iterator vc;
  for(vc=innerSurf.begin();vc!=innerSurf.end();vc++)
    cx<<" "<<*vc;
  cx<<" ";

  return cx.str();
}

void
Torpedo::createObjects(Simulation& System)
  /*!
    Construction of the main shutter
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("Torpedo","constructObjects");

  std::string Out,dSurf;

  // Calculate Cut WITHOUT inner cylinder:
  dSurf=getInnerSurf();
  Out=ModelSupport::getComposite(SMap,surfIndex,"3 -4 5 -6 -7 ");
  // ADD INNER SURF HERE:
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+dSurf));
  voidCell=cellIndex-1;

  Out=ModelSupport::getComposite(SMap,surfIndex,"3 -4 5 -6 ")+dSurf;  
  setInterSurf(Out);

  return;
}

std::string
Torpedo::getSurfString(const std::string& SList) const
  /*!
    Way to get a composite string (for exclude)
    \param SList :: String list
    \return surface string
  */
{
  return ModelSupport::getComposite(SMap,surfIndex,SList);
}

void
Torpedo::createLinks()
  /*!
    Creates the links for the torpedo objects
  */
{
  ELog::RegMethod RegA("Torpedo","createLinks");


  FixedComp::setLinkSurf(1,SMap.realSurf(surfIndex+7));
  std::set<int>::const_iterator vc;
  for(vc=innerSurf.begin();vc!=innerSurf.end();vc++)
    {
      FixedComp::setLinkSurf(0,-*vc);
      if (vc==innerSurf.begin())
	FixedComp::addLinkSurf(1,-*vc);
    }

  FixedComp::setLinkSurf(2,SMap.realSurf(surfIndex+3));
  FixedComp::setLinkSurf(3,-SMap.realSurf(surfIndex+4));
  FixedComp::setLinkSurf(4,SMap.realSurf(surfIndex+5));
  FixedComp::setLinkSurf(5,-SMap.realSurf(surfIndex+6));

  // set Links [WRONG Zero POINT ]:
  FixedComp::setConnect(0,Origin,Y);
  FixedComp::setConnect(1,Origin+Y*innerRadius+Z*zOffset,-Y); 
  FixedComp::setConnect(2,Origin-X*(Width/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(Width/2.0),X);
  FixedComp::setConnect(4,Origin+Z*(zOffset-Height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(zOffset+Height/2.0),Z);
  return;
}

void
Torpedo::createAll(Simulation& System,
		   const shutterSystem::GeneralShutter& GS,
		   const attachSystem::ContainedComp& CC)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param GS :: GeneralShutter to use
    \param CC :: Void Vessel containment
   */
{
  ELog::RegMethod RegA("Torpedo","createAll");

  populate(System,GS);
  createUnitVector(GS);
  createSurfaces();
  calcVoidIntercept(CC);
  createObjects(System);
  calcConvex(System);
  createLinks();
  return;
}

void 
Torpedo::addCrossingIntersect(Simulation& System,const Torpedo& Other)
  /*!
    Check and add cross in surface if an intersect exists:
    \param System :: Simulation system
    \param Other :: Ohter Object to intersect
  */
{
  ELog::RegMethod RegA("Torpedo","addCrossingIntersect");

  std::vector<const Geometry::Face*> OFaces;
  if (vBox.intersectHull(Other.vBox,OFaces) &&
      vBox.getNonMatch(OFaces))
    {
      std::vector<const Geometry::Face*>::const_iterator fc;
      // Construct from OFaces:
      std::ostringstream cx;
      cx<<" ";
      for(fc=OFaces.begin();fc!=OFaces.end();fc++)
	{
	  const int index=Other.findPlane(**fc);
	  if (index)
	    cx<<index<<" ";
	}
      MonteCarlo::Qhull* VC=System.findQhull(voidCell);
      if (VC)
	VC->addSurfString(cx.str());
    }
  return;
}

int
Torpedo::findPlane(const Geometry::Face& FC) const
  /*!
    Find a plane that the face is equivilent to with a 
    sign for the normal
    \param FC :: Face to determine equivilent plane
    \return surface number 
   */
{
  const Geometry::Vec3D& A=FC.getVertex(0)->getV();
  const Geometry::Vec3D& B=FC.getVertex(1)->getV();
  const Geometry::Vec3D& C=FC.getVertex(2)->getV();
  const Geometry::Plane* Ptr=SMap.findPtr<const Geometry::Plane>(A,B,C);
  if (Ptr)
    {
      if (fabs(Ptr->getNormal().dotProd(Z))<0.9)
	return (Ptr->getNormal().dotProd(FC.getNormal())<0) ? 
	  Ptr->getName() : -Ptr->getName();
    }
  return 0;
}

}  // NAMESPACE shutterSystem


