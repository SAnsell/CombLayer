/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chip/ScatterPlate.cxx
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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedGroup.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "ScatterPlate.h"

namespace hutchSystem
{

ScatterPlate::ScatterPlate(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,2),
  populated(0),nLayers(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ScatterPlate::ScatterPlate(const ScatterPlate& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  populated(A.populated),
  width(A.width),height(A.height),
  depth(A.depth),defMat(A.defMat),cutSurf(A.cutSurf),nLayers(A.nLayers),
  cFrac(A.cFrac),cMat(A.cMat),CDivideList(A.CDivideList)
  /*!
    Copy constructor
    \param A :: ScatterPlate to copy
  */
{}

ScatterPlate&
ScatterPlate::operator=(const ScatterPlate& A)
  /*!
    Assignment operator
    \param A :: ScatterPlate to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      populated=A.populated;
      width=A.width;
      height=A.height;
      depth=A.depth;
      defMat=A.defMat;
      cutSurf=A.cutSurf;
      nLayers=A.nLayers;
      cFrac=A.cFrac;
      cMat=A.cMat;
      CDivideList=A.CDivideList;
    }
  return *this;
}

ScatterPlate::~ScatterPlate() 
  /*!
    Destructor
  */
{}

void
ScatterPlate::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Function database
  */
{
  ELog::RegMethod RegA("ScatterPlate","populate");
  
  FixedOffset::populate(Control);
  try
    {      
      width=Control.EvalVar<double>(keyName+"Width");
      height=Control.EvalVar<double>(keyName+"Height");
      depth=Control.EvalVar<double>(keyName+"Depth");
      defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");
      
      // Layers
      nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
      ModelSupport::populateDivide(Control,nLayers,
				   keyName+"Frac_",cFrac);
      ModelSupport::populateDivide(Control,nLayers,
				   keyName+"Mat_",defMat,cMat);
      
      populated = 1;
    }
  // Exit and don't report if we are not using this scatter plate
  catch (ColErr::InContainerError<std::string>& EType)
    {
      ELog::EM<<"ScatterPlate "<<keyName<<" not in use Var:"
	      <<EType.getItem()<<ELog::endWarn;
      populated=0;   
    }
  return;
}


void
ScatterPlate::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("ScatterPlate","createSurface");
  // INNER PLANES
  
  // Front
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*depth,Y);
  
  if (height>0.0)   // Only create if not using register edges
    {
      ModelSupport::buildPlane(SMap,buildIndex+6,
			       Origin+Z*(height/2.0),Z);
      // down
      ModelSupport::buildPlane(SMap,buildIndex+5,
			       Origin-Z*height/2.0,Z);
    }
  if (width>0.0)
    {
      // left [Dept. on Axis]
      ModelSupport::buildPlane(SMap,buildIndex+3,
			       Origin-X*(width/2.0),X);
      // right [Dept. on Axis]
      ModelSupport::buildPlane(SMap,buildIndex+3,
			       Origin+X*(width/2.0),X);
    }

  return;
}

void
ScatterPlate::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ScatterPlate","createObjects");
  
  // Master box:
  std::string Out;
  Out=ModelSupport::getSetComposite(SMap,buildIndex," 1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  Out+=getContainer();
  System.addCell(MonteCarlo::Object(cellIndex++,defMat,0.0,Out));

  return;
}

void 
ScatterPlate::layerProcess(Simulation& System)
  /*!
    Processes the splitting of the surfaces into a multilayer system
    This has to deal with the three layers that invade cells:
    
    \param System :: Simulation to work on
  */
{
  ELog::RegMethod RegA("ScatterPlate","LayerProcess");
  
  if (!nLayers) return;
  ModelSupport::surfDivide DA;
  // Generic
  for(size_t i=0;i<nLayers-1;i++)
    {
      DA.addFrac(cFrac[i]);
      DA.addMaterial(cMat[i]);
    }
  DA.addMaterial(cMat[nLayers-1]);

  const size_t CSize=CDivideList.size();
  for(size_t i=0;i<CSize;i++)
    {
      DA.init();
      // Cell Specific:
      DA.setCellN(CDivideList[i]);
      DA.setOutNum(cellIndex,buildIndex+201+100*static_cast<int>(i));

      DA.makePair<Geometry::Plane>(SMap.realSurf(buildIndex+1),
				   -SMap.realSurf(buildIndex+2));
      DA.activeDivide(System);
      cellIndex=DA.getCellNum();
    }
  return;
}

int
ScatterPlate::exitWindow(const double Dist,
		    std::vector<int>& window,
		    Geometry::Vec3D& Pt) const
  /*!
    Outputs a window
    \param Dist :: Dist from exit point
    \param window :: window vector of paired planes
    \param Pt :: Output point for tally
    \todo add 
    \return Master Plane
  */
{  
  ELog::RegMethod RegA("ScatterPlate","exitWindow");
  window.clear();
  if (width>0.0)
    {
      window.push_back(SMap.realSurf(buildIndex+3));
      window.push_back(SMap.realSurf(buildIndex+4));
    }
  if (depth>0.0)
    {
      window.push_back(SMap.realSurf(buildIndex+5));
      window.push_back(SMap.realSurf(buildIndex+6));
    }
  Pt=Origin+Y*(depth+Dist);  

  return SMap.realSurf(buildIndex+2);
}
  
void
ScatterPlate::createAll(Simulation& System,
			const attachSystem::FixedComp& LC,
			const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param LC :: Linear component to set axis etc
    \param sideIndex :: link pont 
  */
{
  ELog::RegMethod RegA("ScatterPlate","createAll");
  populate(System.getDataBase());
  if (populated)
    {
      createUnitVector(LC,sideIndex);
      createSurfaces();
      createObjects(System);
      layerProcess(System);
      insertObjects(System);
    }
  return;
}
  
}  // NAMESPACE shutterSystem
