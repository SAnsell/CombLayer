/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chip/ChipSample.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "Vec3D.h"
#include "Triple.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
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
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ChipSample.h"

namespace hutchSystem
{

ChipSample::ChipSample(const std::string& Key,const int Index) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,2),ID(Index),
  csIndex(ModelSupport::objectRegister::Instance().
	  cell(StrFunc::makeString(Key,Index))),
  cellIndex(csIndex+1),populated(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ChipSample::ChipSample(const ChipSample& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  ID(A.ID),csIndex(A.csIndex),
  cellIndex(A.cellIndex),populated(A.populated),
  tableNum(A.tableNum),zAngle(A.zAngle),xyAngle(A.xyAngle),
  XStep(A.XStep),YStep(A.YStep),ZLift(A.ZLift),
  width(A.width),height(A.height),length(A.length),
  defMat(A.defMat)
  /*!
    Copy constructor
    \param A :: ChipSample to copy
  */
{}

ChipSample&
ChipSample::operator=(const ChipSample& A)
  /*!
    Assignment operator
    \param A :: ChipSample to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      tableNum=A.tableNum;
      zAngle=A.zAngle;
      xyAngle=A.xyAngle;
      XStep=A.XStep;
      YStep=A.YStep;
      ZLift=A.ZLift;
      width=A.width;
      height=A.height;
      length=A.length;
      defMat=A.defMat;
    }
  return *this;
}

ChipSample::~ChipSample() 
  /*!
    Destructor
  */
{}

void
ChipSample::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("ChipSample","populate");

  const FuncDataBase& Control=System.getDataBase();

  // Two keys : one with a number and the default
  const std::string keyIndex(StrFunc::makeString(keyName,ID));
  try
    {
      tableNum=Control.EvalPair<int>(keyIndex,keyName,"TableNum");
      zAngle=Control.EvalPair<double>(keyIndex,keyName,"ZAngle");
      xyAngle=Control.EvalPair<double>(keyIndex,keyName,"XYAngle");

      XStep=Control.EvalPair<double>(keyIndex,keyName,"XStep");
      YStep=Control.EvalPair<double>(keyIndex,keyName,"YStep");
      ZLift=Control.EvalPair<double>(keyIndex,keyName,"ZLift");
      
      width=Control.EvalPair<double>(keyIndex,keyName,"Width");
      height=Control.EvalPair<double>(keyIndex,keyName,"Height");
      length=Control.EvalPair<double>(keyIndex,keyName,"Depth");
      defMat=ModelSupport::EvalMat<int>(Control,keyIndex+"DefMat",
					keyName+"DefMat");
      
      populated = 1;
    }
  // Exit and don't report if we are not using this scatter plate
  catch (ColErr::InContainerError<std::string>& EType)
    {
      ELog::EM<<keyName<<" not in use Vars:"
	      <<EType.getItem()<<ELog::endWarn;
      populated=0;   
    }
  return;
}

void
ChipSample::createUnitVector(const FixedComp& TC)
  /*!
    Create the unit vectors
    \param TC :: Table to attach sample to
  */
{
  ELog::RegMethod RegA("ChipSample","createUnitVector");

  // Origin is in the wrong place as it is at the EXIT:
  FixedComp::createUnitVector(TC);

  Origin=TC.getLinkPt(4);
  Origin+=X*XStep+Y*YStep+Z*ZLift;

  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(zAngle,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(xyAngle,Z);
  
  Qz.rotate(Y);
  Qz.rotate(X);
  Qxy.rotate(X);
  Qxy.rotate(Y);
  Qxy.rotate(Z);

  return;
}

void
ChipSample::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("ChipSample","createSurface");
  // Sides
  ModelSupport::buildPlane(SMap,csIndex+1,Origin-Y*length/2.0,Y);
  ModelSupport::buildPlane(SMap,csIndex+2,Origin+Y*length/2.0,Y);
  ModelSupport::buildPlane(SMap,csIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,csIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,csIndex+5,Origin,Z);
  ModelSupport::buildPlane(SMap,csIndex+6,Origin+Z*height,Z);
  
  return;
}

void
ChipSample::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ChipSample","createObjects");
  
  std::string Out;
  // Master box: 
  Out=ModelSupport::getComposite(SMap,csIndex,"1 -2 3 -4 5 -6");
  addOuterSurf(Out);
  System.addCell(MonteCarlo::Qhull(cellIndex++,defMat,0.0,Out));
  
  return;
}

  
void
ChipSample::createAll(Simulation& System,
		      const attachSystem::FixedComp& TCA,
		      const attachSystem::FixedComp& TCB)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param TCA :: First Table 
    \param TCB :: Second Table 
  */
{
  ELog::RegMethod RegA("ChipSample","createAll");

  populate(System);
  if (populated)
    {
      if (tableNum==1)
	createUnitVector(TCA);
      else
	createUnitVector(TCB);
      createSurfaces();
      createObjects(System);
      insertObjects(System);
    }
  return;
}
  
}  // NAMESPACE shutterSystem
