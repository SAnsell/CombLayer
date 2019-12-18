/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chip/ColBox.cxx
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
#include "generateSurf.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "ColBox.h"

namespace hutchSystem
{

ColBox::ColBox(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,2),
  populated(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ColBox::ColBox(const ColBox& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  populated(A.populated),insertCell(A.insertCell),
  width(A.width),depth(A.depth),height(A.height),
  roofThick(A.roofThick),floorThick(A.floorThick),
  frontThick(A.frontThick),backThick(A.backThick),
  viewX(A.viewX),viewZ(A.viewZ),outMat(A.outMat),
  defMat(A.defMat)
  /*!
    Copy constructor
    \param A :: ColBox to copy
  */
{}

ColBox&
ColBox::operator=(const ColBox& A)
  /*!
    Assignment operator
    \param A :: ColBox to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      populated=A.populated;
      insertCell=A.insertCell;
      width=A.width;
      depth=A.depth;
      height=A.height;
      roofThick=A.roofThick;
      floorThick=A.floorThick;
      frontThick=A.frontThick;
      backThick=A.backThick;
      viewX=A.viewX;
      viewZ=A.viewZ;
      outMat=A.outMat;
      defMat=A.defMat;
    }
  return *this;
}

ColBox::~ColBox() 
  /*!
    Destructor
  */
{}

void
ColBox::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DattaBase to use
  */
{
  ELog::RegMethod RegA("ColBox","populate");
  
  FixedOffset::populate(Control);

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");

  roofThick=Control.EvalVar<double>(keyName+"RoofThick");
  floorThick=Control.EvalVar<double>(keyName+"FloorThick");
  frontThick=Control.EvalVar<double>(keyName+"FrontThick");
  backThick=Control.EvalVar<double>(keyName+"BackThick");

  viewX=Control.EvalVar<double>(keyName+"ViewX");
  viewZ=Control.EvalVar<double>(keyName+"ViewZ");

  outMat=ModelSupport::EvalMat<int>(Control,keyName+"OutMat");
  defMat=ModelSupport::EvalMat<int>(Control,keyName+"DefMat");

  return;
}

void
ColBox::createUnitVector(const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Create the unit vectors
    \param TC :: TwinComponent to attach to
  */
{
  ELog::RegMethod RegA("ColBox","createUnitVector");
  const attachSystem::FixedGroup* FGPtr=
    dynamic_cast<const attachSystem::FixedGroup*>(&FC);
  if (FGPtr)
    FixedComp::createUnitVector(FGPtr->getKey("Beam"),sideIndex);
  else
    FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
ColBox::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("ColBox","createSurface");
  // Back

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*depth,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(depth/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(depth/2.0),Z);

  // Front / Roof and Back planes
  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin+Y*backThick,Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(depth-frontThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(depth/2.0-floorThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(depth/2.0-roofThick),Z);

  // View Ports:
  ModelSupport::buildPlane(SMap,buildIndex+103,
			   Origin-X*(viewX/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+104,
			   Origin+X*(viewX/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+105,
			   Origin-Z*(viewZ/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+106,
			   Origin-Z*(viewZ/2.0),Z);
  

  return;
}

void
ColBox::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ColBox","createObjects");

  std::string Out;
  // LEFT box: [virtual]  
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6");
  addOuterSurf(Out);
  
  // Front plate
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -11 3 -4 5 -6 ");
  System.addCell(MonteCarlo::Object(cellIndex++,outMat,0.0,Out));
  
  
  return;
}

void 
ColBox::setMidFace(const Geometry::Vec3D& fC)
  /*!
    Sets the centre
    \param fC :: Centre point (Mid Face)
  */
{
  populated |= 8;
  populated &= (~4);
  Origin=fC;
  return;
}

int
ColBox::exitWindow(const double Dist,
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
  ELog::RegMethod RegA("ColBox","exitWindow");

  window.clear();
  window.push_back(SMap.realSurf(buildIndex+3));
  window.push_back(SMap.realSurf(buildIndex+4));
  window.push_back(SMap.realSurf(buildIndex+5));
  window.push_back(SMap.realSurf(buildIndex+6));
  Pt=Origin+Y*(depth+Dist);  
  return SMap.realSurf(buildIndex+2);
}

void
ColBox::createPartial(Simulation& System,
		      const attachSystem::FixedComp& TC,
		      const long int sideIndex)
  /*!
    Generic function to create just sufficient to get datum points
    \param System :: Simulation item
    \param TC :: Twin component to set axis etc
  */
{
  ELog::RegMethod RegA("ColBox","createPartial");

  populate(System.getDataBase());
  if (populated!=9)
    {
      ELog::EM<<"ERROR ColBox not populated:"<<populated<<ELog::endErr;
      throw ColErr::ExitAbort(ELog::RegMethod::getFull());
    }
  
  ColBox::createUnitVector(TC,sideIndex);  
  return;
}
  
void
ColBox::createAll(Simulation& System,
		  const attachSystem::FixedComp& TC,
		  const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param TC :: Linear component to set axis etc
  */
{
  ELog::RegMethod RegA("ColBox","createAll");

  createPartial(System,TC,sideIndex);
  createSurfaces();
  createObjects(System);
  insertObjects(System);
  
  return;
}
  
}  // NAMESPACE hutchSystem
