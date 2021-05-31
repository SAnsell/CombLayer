/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   lensModel/siModerator.cxx
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "siModerator.h"

namespace lensSystem
{

siModerator::siModerator(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  populated(0)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

siModerator::siModerator(const siModerator& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  populated(A.populated),modLength(A.modLength),
  modWidth(A.modWidth),modHeight(A.modHeight),topThick(A.topThick),
  baseThick(A.baseThick),sideThick(A.sideThick),
  siThick(A.siThick),polyThick(A.polyThick),temp(A.temp),
  siMat(A.siMat),polyMat(A.polyMat),surroundMat(A.surroundMat)
  /*!
    Copy constructor
    \param A :: siModerator to copy
  */
{}

siModerator&
siModerator::operator=(const siModerator& A)
  /*!
    Assignment operator
    \param A :: siModerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      populated=A.populated;
      modLength=A.modLength;
      modWidth=A.modWidth;
      modHeight=A.modHeight;
      topThick=A.topThick;
      baseThick=A.baseThick;
      sideThick=A.sideThick;
      siThick=A.siThick;
      polyThick=A.polyThick;
      temp=A.temp;
      siMat=A.siMat;
      polyMat=A.polyMat;
      surroundMat=A.surroundMat;
    }
  return *this;
}

void
siModerator::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  modHeight=Control.EvalVar<double>(keyName+"Height");
  modWidth=Control.EvalVar<double>(keyName+"Width");
  modLength=Control.EvalVar<double>(keyName+"Length");
  siThick=Control.EvalVar<double>(keyName+"SiThick");
  polyThick=Control.EvalVar<double>(keyName+"PolyThick");
  baseThick=Control.EvalVar<double>(keyName+"BaseAlThick");
  topThick=Control.EvalVar<double>(keyName+"TopAlThick");
  sideThick=Control.EvalVar<double>(keyName+"SideAlThick");
  temp=Control.EvalVar<double>(keyName+"Temp"); 
  siMat=ModelSupport::EvalMat<int>(Control,keyName+"SiMat");
  polyMat=ModelSupport::EvalMat<int>(Control,keyName+"PolyMat"); 
  surroundMat=ModelSupport::EvalMat<int>(Control,keyName+"SurroundMat"); 

  return;
}

void
siModerator::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("siModerator","createSurfaces");

  // Across sides
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-X*modWidth/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+X*modWidth/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-Y*modLength/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+Y*modLength/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*modHeight/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*modHeight/2.0,Z);

  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-X*(modWidth/2.0+sideThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+X*(modWidth/2.0+sideThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(modHeight/2.0+baseThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(modHeight/2.0+topThick),Z);

  // Now create Z plane silicon/poly layers:
  double Zpoint=-modHeight/2.0+polyThick;
  int nextType(0);          // 0 ==> next to add is poly / 1 next to add is poly
  int planeIndex(buildIndex+101);  // Plane to add
  while(Zpoint<modHeight/2.0-siThick)
    {
      ModelSupport::buildPlane(SMap,planeIndex,Origin+Z*Zpoint,Z);
      planeIndex++;
      nextType=1-nextType;
      Zpoint+=(nextType) ? siThick : polyThick;      
    }
  return; 
}

void
siModerator::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
   */
{
  ELog::RegMethod RegA("siModerator","createObjects");

  std::string Out;
  // Virtual box:
  Out=ModelSupport::getComposite(buildIndex,"11 -12 3 -4 15 -16");
  addOuterSurf(Out);

  //  System.makeVirtual(cellIndex-1);         
  // Now create Z plane silicon/poly layers:
  const std::string XYsides("1 -2 3 -4 ");

  std::ostringstream cx;
  double Zpoint = -modHeight/2.0+polyThick;
  int nextType(0);          // 0 ==> next to add is poly / 1 next to add is poly
  int planeIndex(101);      // Plane to add
  int prevIndex(5);
  int mat[2]={polyMat,siMat};
  while(Zpoint<modHeight/2.0-siThick)
    {
      cx.str("");
      cx<<prevIndex<<" "<<(-planeIndex);
      Out=ModelSupport::getComposite(buildIndex,XYsides+cx.str());
      System.addCell(MonteCarlo::Object(cellIndex++,mat[nextType],temp,Out));
      prevIndex=planeIndex;
      planeIndex++;
      nextType=1-nextType;
      Zpoint+=(nextType) ? siThick : polyThick;      
    }
  // Last cell that hits the outside limit:
  cx.str("");
  cx<<prevIndex<<" -6";
  Out=ModelSupport::getComposite(buildIndex,XYsides+cx.str());
  System.addCell(MonteCarlo::Object(cellIndex++,mat[nextType],temp,Out));

  // Add AL surrounds:
  Out=ModelSupport::getComposite(buildIndex,
				 "11 -12 3 -4 15 -16 ( -1 : 2 : -5 : 6 )");
  System.addCell(MonteCarlo::Object(cellIndex++,surroundMat,temp,Out));
  return; 
}

void
siModerator::createLinks()
  /*!
    Creates a full attachment set
  */
{
  // back front 
  FixedComp::setConnect(0,-Y*modLength/2.0,-Y);
  FixedComp::setConnect(1,Y*modLength/2.0,Y);
  FixedComp::setConnect(2,-X*(modWidth/2.0+sideThick),-X);
  FixedComp::setConnect(3,X*(modWidth/2.0+sideThick),X);
  FixedComp::setConnect(4,-Z*(modLength/2.0+baseThick),-Z);
  FixedComp::setConnect(5,Z*(modLength/2.0+topThick),Z);

  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+11));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+12));
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));

  return;
}


void
siModerator::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
   */
{
  ELog::RegMethod RegA("siModerator","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE LensModel
