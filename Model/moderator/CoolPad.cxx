/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/CoolPad.cxx
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedUnit.h"
#include "ContainedComp.h"
#include "boxValues.h"
#include "boxUnit.h"
#include "BoxLine.h"
#include "CoolPad.h"

namespace moderatorSystem
{

CoolPad::CoolPad(const std::string& key,const size_t Index) :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(key+std::to_string(Index),1),
  ID(Index),baseName(key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param Index :: index number
  */
{}

CoolPad::CoolPad(const CoolPad& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  ID(A.ID),baseName(A.baseName),
  xStep(A.xStep),zStep(A.zStep),thick(A.thick),height(A.height),
  width(A.width),Mat(A.Mat)
  /*!
    Copy constructor
    \param A :: CoolPad to copy
  */
{}

CoolPad&
CoolPad::operator=(const CoolPad& A)
  /*!
    Assignment operator
    \param A :: CoolPad to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      xStep=A.xStep;
      zStep=A.zStep;
      thick=A.thick;
      height=A.height;
      width=A.width;
      Mat=A.Mat;
    }
  return *this;
}


CoolPad::~CoolPad() 
/*!
  Destructor
*/
{}
  
void
CoolPad::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Database for variables
  */
{
  ELog::RegMethod RegA("CoolPad","populate");

  // Two keys : one with a number and the default
  //  fixIndex=Control.EvalTail<size_t>(keyName,keyName,"FixIndex");
  xStep=Control.EvalTail<double>(keyName,baseName,"XStep");
  zStep=Control.EvalTail<double>(keyName,baseName,"ZStep");
  thick=Control.EvalTail<double>(keyName,baseName,"Thick");
  width=Control.EvalTail<double>(keyName,baseName,"Width");
  height=Control.EvalTail<double>(keyName,baseName,"Height");
  Mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat",baseName+"Mat");
  const size_t nZ=Control.EvalTail<size_t>(keyName,baseName,"NZigZag");
  CPts.clear();
  for(size_t i=0;i<nZ;i++)
    {
      const std::string tagIndex="Cent"+std::to_string(i+1);
      CPts.push_back(Control.EvalTail<Geometry::Vec3D>
		     (keyName,baseName,tagIndex));
    }

  IWidth=Control.EvalTail<double>(keyName,baseName,"IWidth");
  IDepth=Control.EvalTail<double>(keyName,baseName,"IDepth");

  IMat=ModelSupport::EvalMat<int>(Control,keyName+"IMat",baseName+"IMat");
  
  return;
}
  
void
CoolPad::createUnitVector(const attachSystem::FixedComp& CUnit,
			  const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Points down Target
    - X Across the target
    - Z up 
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex :: Side to connect to

  */
{
  ELog::RegMethod RegA("CoolPad","createUnitVector");
  FixedComp::createUnitVector(CUnit,sideIndex);
  applyShift(xStep,0.0,zStep);

  // Ugly loop to put relative coordinates into absolute
  for(size_t i=0;i<CPts.size();i++)
    {
      CPts[i]=-X*CPts[i].X()+Y*CPts[i].Y()+
	Z*CPts[i].Z()+Origin;
    }
  return;
}

void
CoolPad::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CoolPad","createSurface");

  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*thick,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(height/2.0),Z);

  return;
}

void
CoolPad::createObjects(Simulation& System)
  /*!
    Adds the Cylinder
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CoolPad","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,buildIndex," -2 3 -4 5 -6 ");
  Out+=hotSurf.display();
  
  System.addCell(MonteCarlo::Object(cellIndex++,Mat,0.0,Out));
  addOuterSurf(Out);

  return;
}

void
CoolPad::createWaterTrack(Simulation& System)
  /*!
    Build the cooling pad pipe line
    \param System :: Simulation to place item
  */
{  
  ELog::RegMethod RegA("CoolPad","createWaterTrack");
  ModelSupport::BoxLine WaterTrack("PadWater"+std::to_string(ID));

  WaterTrack.setPoints(CPts);
  WaterTrack.addSection(IWidth,IDepth,IMat,0.0);

  WaterTrack.setInitZAxis(Y);
  WaterTrack.build(System);
  // NoInsert();
  //  WaterTrack.insertPipeToCell(System,cellIndex-1);
  return;
}
  
void
CoolPad::createAll(Simulation& System,
		   const attachSystem::FixedComp& FUnit,
		   const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
    \param sideIndex :: Link point [signed]
  */
{
  ELog::RegMethod RegA("CoolPad","createAll");
  populate(System.getDataBase());
  
  createUnitVector(FUnit,sideIndex);
  createSurfaces();
  hotSurf=FUnit.getFullRule(sideIndex);
  createObjects(System);
  insertObjects(System);       
  createWaterTrack(System);
  return;
}
  
}  // NAMESPACE moderatorSystem
