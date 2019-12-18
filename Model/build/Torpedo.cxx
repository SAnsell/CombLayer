/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   build/Torpedo.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
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
#include "shutterBlock.h"
#include "SimProcess.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "vertexCalc.h"
#include "Torpedo.h"


namespace shutterSystem
{

Torpedo::Torpedo(const size_t ID,const std::string& Key) : 
  attachSystem::FixedOffset(Key+std::to_string(ID),6),
  attachSystem::ContainedComp(),attachSystem::CellMap(),
  attachSystem::ExternalCut(),
  baseName(Key),shutterNumber(ID)
  /*!\
    Constructor BUT ALL variable are left unpopulated.
    \param ID :: Shutter number
    \param Key :: Variable keyword 
  */
{}

Torpedo::Torpedo(const Torpedo& A) : 
  attachSystem::FixedOffset(A),attachSystem::ContainedComp(A),
  attachSystem::CellMap(A),attachSystem::ExternalCut(A),
  baseName(A.baseName),shutterNumber(A.shutterNumber),
  vBox(A.vBox),voidXoffset(A.voidXoffset),
  innerRadius(A.innerRadius),
  Height(A.Height),Width(A.Width),innerSurf(A.innerSurf)
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
      attachSystem::FixedOffset::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      vBox=A.vBox;
      voidXoffset=A.voidXoffset;
      innerRadius=A.innerRadius;
      zOffset=A.zOffset;
      Height=A.Height;
      Width=A.Width;
      innerSurf=A.innerSurf;
    }
  return *this;
}

Torpedo::~Torpedo() 
  /*!
    Destructor
  */
{}
				
void
Torpedo::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: FuncDataBase fot use
  */
{
  ELog::RegMethod RegA("Torpedo","populate");

  // Global from shutter size:
  FixedOffset::populate(Control);
  
  voidXoffset=Control.EvalVar<double>("voidXoffset");
  innerRadius=Control.EvalVar<double>("bulkShutterRadius");

  // why needed?
  zOffset=Control.EvalTail<double>(keyName,baseName,"ZOffset");
  Width=Control.EvalTail<double>(keyName,baseName,"Width");
  Height=Control.EvalTail<double>(keyName,baseName,"Width");
  
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
      const Geometry::Vec3D OP=Origin+Y*1000.0+Z*zScale+
	X*xScale;
      const Geometry::Line LA(OP,-Y);
      const int surfN=CC.surfOuterIntersect(LA);
      if (surfN)
	innerSurf.insert(-surfN);
    }
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

  MonteCarlo::Object* VC=System.findObject(getCell("Void"));
  if (VC)
    {
      const std::vector<Geometry::Vec3D> PT=
	ModelSupport::calcVertexPoints(*VC);
      vBox.setPoints(PT);
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

  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(Width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(Width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin+Z*(Height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,
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
  Out=ModelSupport::getComposite(SMap,buildIndex,"3 -4 5 -6 ");
  Out+=ExternalCut::getRuleStr("Outer");
  // ADD INNER SURF HERE:
  CellMap::makeCell("Void",System,cellIndex++,0,0.0,Out+dSurf);
  

  Out=ModelSupport::getComposite(SMap,buildIndex,"3 -4 5 -6 ")+dSurf;  
  addOuterSurf(Out);

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
  return ModelSupport::getComposite(SMap,buildIndex,SList);
}

void
Torpedo::createLinks()
  /*!
    Creates the links for the torpedo objects
  */
{
  ELog::RegMethod RegA("Torpedo","createLinks");

  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+7));
  std::set<int>::const_iterator vc;
  for(vc=innerSurf.begin();vc!=innerSurf.end();vc++)
    {
      FixedComp::setLinkSurf(0,-*vc);
      if (vc==innerSurf.begin())
	FixedComp::addLinkSurf(1,-*vc);
    }

  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,-SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,-SMap.realSurf(buildIndex+6));

  // set Links
  // First point is center line intersect
  const Geometry::Vec3D OP=Origin+Y*innerRadius+Z*zOffset;
  MonteCarlo::LineIntersectVisit LI(OP,Y);
  
  //  ELog::EM<<"Inner Surf "<<getInnerSurf()<<ELog::endDiag;
  //  ELog::EM<<"INNER POINT == "<<to<<LI.getPoint(getInnerSurf(),OP)<<ELog::endDiag;
  FixedComp::setConnect(0,LI.getPoint(getInnerSurf(),OP),-Y);
  FixedComp::setConnect(1,LI.getPoint(SMap.realSurfPtr(buildIndex+7),OP),Y);
  FixedComp::setConnect(2,Origin-X*(Width/2.0),-X);
  FixedComp::setConnect(3,Origin+X*(Width/2.0),X);
  FixedComp::setConnect(4,Origin+Z*(zOffset-Height/2.0),-Z);
  FixedComp::setConnect(5,Origin+Z*(zOffset+Height/2.0),Z);
  return;
}

void 
Torpedo::addCrossingIntersect(Simulation& System,
			      const Torpedo& Other)
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
      
      MonteCarlo::Object* VC=System.findObject(getCell("Void"));
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
  ELog::RegMethod RegA("Torpedo","findPlane");
  
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



void
Torpedo::createAll(Simulation& System,
		   const attachSystem::FixedComp& GS,
		   const long int sideIndex)
  /*!
    Create the shutter
    \param System :: Simulation to process
    \param GS :: GeneralShutter to use
    \param CC :: Void Vessel containment
   */
{
  ELog::RegMethod RegA("Torpedo","createAll");

  populate(System.getDataBase());
  createUnitVector(GS,sideIndex);
  createSurfaces();
  //  calcVoidIntercept(CC);  //FIGURE OUT CC
  createObjects(System);
  calcConvex(System);
  createLinks();
  return;
}


}  // NAMESPACE shutterSystem


