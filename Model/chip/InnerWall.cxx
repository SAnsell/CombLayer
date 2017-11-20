/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chip/InnerWall.cxx
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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "generateSurf.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "InnerWall.h"

namespace hutchSystem
{

InnerWall::InnerWall(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,2),
  innerIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(innerIndex+1),nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

InnerWall::InnerWall(const InnerWall& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),  
  innerIndex(A.innerIndex),cellIndex(A.cellIndex),
  height(A.height),width(A.width),depth(A.depth),defMat(A.defMat),
  nLayers(A.nLayers),cFrac(A.cFrac),cMat(A.cMat),
  CDivideList(A.CDivideList)
  /*!
    Copy constructor
    \param A :: InnerWall to copy
  */
{}

InnerWall&
InnerWall::operator=(const InnerWall& A)
  /*!
    Assignment operator
    \param A :: InnerWall to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      height=A.height;
      width=A.width;
      depth=A.depth;
      defMat=A.defMat;
      nLayers=A.nLayers;
      cFrac=A.cFrac;
      cMat=A.cMat;
      CDivideList=A.CDivideList;
    }
  return *this;
}


InnerWall::~InnerWall() 
  /*!
    Destructor
  */
{}

void
InnerWall::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Database to use
  */
{
  ELog::RegMethod RegA("InnerWall","populate");

  FixedOffset::populate(Control);
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  defMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  // Layers
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  ModelSupport::populateDivideLen(Control,nLayers,
				  keyName+"Frac_",depth,
				  cFrac);
  ModelSupport::populateDivide(Control,nLayers,
			       keyName+"Mat_",defMat,cMat);


  return;
}

void
InnerWall::createUnitVector(const attachSystem::FixedComp& FC,
			    const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: LinearComponent to attach to
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("InnerWall","createUnitVector");
  // Origin is in the wrong place as it is at the EXIT:
  FixedComp::createUnitVector(FC,sideIndex);
  yStep+=depth/2.0;
  applyOffset();
  
  // This should not be needed:
  //  Origin=FC.getExit();
  //  Origin+=X*xStep+Y*yStep+Z*zStep;
  // Move centre before rotation
  //  Centre=Origin+Y*(depth/2.0);
   
  //  setExit(Origin+Y*depth,Y);
  return;
}

void
InnerWall::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("InnerWall","createSurface");
  // INNER PLANES

  // Front
  ModelSupport::buildPlane(SMap,innerIndex+1,Origin-Y*depth/2.0,Y);
  addLinkSurf(0,SMap.realSurf(innerIndex+1));
  // Back
  ModelSupport::buildPlane(SMap,innerIndex+2,Origin+Y*depth/2.0,Y);

  setBridgeSurf(1,SMap.realSurf(innerIndex+2));
  addLinkSurf(1,-SMap.realSurf(innerIndex+2));

  // Hole Inner:
  ModelSupport::buildPlane(SMap,innerIndex+3,Origin-X*width,X);
  ModelSupport::buildPlane(SMap,innerIndex+4,Origin+X*width,X);
  ModelSupport::buildPlane(SMap,innerIndex+5,Origin-Z*height,Z);
  ModelSupport::buildPlane(SMap,innerIndex+6,Origin+Z*height,Z);
  return;
}

void
InnerWall::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("InnerWall","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,innerIndex,"1 -2");
  addOuterSurf(Out);

  // HOLE:
  Out=ModelSupport::getComposite(SMap,innerIndex,"1 -2 3 -4 5 -6");
  Out+=" "+getContainer();
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  // Main wall
  Out=ModelSupport::getComposite(SMap,innerIndex,"1 -2 (-3:4:-5:6)");
  Out+=" "+getContainer();
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  CDivideList.push_back(cellIndex-1);

  return;
}

void 
InnerWall::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    This has to deal with the three layers that invade cells:
    
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("InnerWall","layerProcess");  

  if (nLayers<1) return;

  ModelSupport::surfDivide DA;
  // Generic
  size_t i;
  for(i=0;i<static_cast<size_t>(nLayers-1);i++)
    {
      DA.addFrac(cFrac[i]);
      DA.addMaterial(cMat[i]);
    }
  DA.addMaterial(cMat[i]);

  const int CSize=static_cast<int>(CDivideList.size());
  for(int i=0;i<CSize;i++)
    {
      DA.init();
      // Cell Specific:
      DA.setCellN(CDivideList[static_cast<size_t>(i)]);
      DA.setOutNum(cellIndex,innerIndex+201+100*i);
      DA.makePair<Geometry::Plane>(SMap.realSurf(innerIndex+1),
				   SMap.realSurf(innerIndex+2));
      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
    }
  return;
}

int
InnerWall::exitWindow(const double Dist,
		    std::vector<int>& window,
		    Geometry::Vec3D& Pt) const
  /*!
    Outputs a window
    \param Dist :: Dist from exit point
    \param window :: window vector of paired planes
    \param Pt :: Output point for tally
    \return Master Plane
  */
{
  window.clear();
  window.push_back(SMap.realSurf(innerIndex+3));
  window.push_back(SMap.realSurf(innerIndex+4));
  window.push_back(SMap.realSurf(innerIndex+5));
  window.push_back(SMap.realSurf(innerIndex+6));
  Pt=Origin+Y*(Dist-depth/2.0);  
  return SMap.realSurf(innerIndex+2);
}
  

void
InnerWall::createOnlyObjects(Simulation& System)

  /*!
    Generic function to create objects
    \param System :: Simulation item
  */
{
  ELog::RegMethod RegA("InnerWall","createAll");

  createObjects(System);
  layerProcess(System);
  //  createLinks();
  insertObjects(System);

  
  return;
}

void
InnerWall::createOnlySurfaces(Simulation& System,
			      const attachSystem::FixedComp& FC,
			      const long int sideIndex)
/*!
  Generic function to create surfaces
    \param System :: Simulation item
    \param FC :: Linear component to set axis etc
    \param sideIndex :: link point
*/
{
  ELog::RegMethod RegA("InnerWall","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  
  return;
}

void
InnerWall::createAll(Simulation& System,
		     const attachSystem::FixedComp& FC,
		     const long int sideIndex)
  /*!
    Generic function to create surfaces
    \param System :: Simulation item
    \param FC :: Linear component to set axis etc
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("InnerWall","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  layerProcess(System);
  //  createLinks();
  insertObjects(System);

  
  return;
}

}  // NAMESPACE hutchSystem
