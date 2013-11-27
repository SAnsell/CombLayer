/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   moderator/CoolPad.cxx
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
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
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
#include "LinearComp.h"
#include "ContainedComp.h"
#include "boxValues.h"
#include "boxUnit.h"
#include "BoxLine.h"
#include "CoolPad.h"

namespace moderatorSystem
{

CoolPad::CoolPad(const std::string& Key,const int Index) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,1),
  ID(Index),padIndex(ModelSupport::objectRegister::Instance().
		     cell(StrFunc::makeString(Key,Index))),
  cellIndex(padIndex+1),populated(0),fixIndex(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CoolPad::CoolPad(const CoolPad& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  ID(A.ID),padIndex(A.padIndex),cellIndex(A.cellIndex),
  populated(A.populated),fixIndex(A.fixIndex),xStep(A.xStep),
  zStep(A.zStep),thick(A.thick),height(A.height),
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
      cellIndex=A.cellIndex;
      populated=A.populated;
      fixIndex=A.fixIndex;
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
CoolPad::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("CoolPad","populate");
  const FuncDataBase& Control=System.getDataBase();

  // Two keys : one with a number and the default
  const std::string keyIndex(StrFunc::makeString(keyName,ID));
  try
    {
      const int FI=Control.EvalPair<int>(keyIndex,keyName,"FixIndex");
      if (FI<0)
	throw ColErr::IndexError<int>(FI,0,RegA.getFull());
      fixIndex=static_cast<size_t>(FI);
      xStep=Control.EvalPair<double>(keyIndex,keyName,"XStep");
      zStep=Control.EvalPair<double>(keyIndex,keyName,"ZStep");
      thick=Control.EvalPair<double>(keyIndex,keyName,"Thick");
      width=Control.EvalPair<double>(keyIndex,keyName,"Width");
      height=Control.EvalPair<double>(keyIndex,keyName,"Height");
      Mat=ModelSupport::EvalMat<int>(Control,keyIndex+"Mat",keyName+"Mat");
      const int nZ=Control.EvalPair<int>(keyIndex,keyName,"NZigZag");
      CPts.clear();
      for(int i=0;i<nZ;i++)
	{
	  const std::string tagIndex=
	    StrFunc::makeString(std::string("Cent"),i+1);
	  CPts.push_back(Control.EvalPair<Geometry::Vec3D>
			 (keyIndex,keyName,tagIndex));
	}

      IWidth=Control.EvalPair<double>(keyIndex,keyName,"IWidth");
      IDepth=Control.EvalPair<double>(keyIndex,keyName,"IDepth");
      IMat=Control.EvalPair<int>(keyIndex,keyName,"IMat");
      populated |= 1;
    }
  // Exit and don't report if we are not using this scatter plate
  catch (ColErr::InContainerError<std::string>& EType)
    {
      ELog::EM<<keyIndex<<" not in use Vars:"
	      <<EType.getItem()<<ELog::endErr;
      populated=0;   
    }
  
  return;
}
  
void
CoolPad::createUnitVector(const attachSystem::FixedComp& CUnit)
  /*!
    Create the unit vectors
    - Y Points down Target
    - X Across the target
    - Z up 
    \param CUnit :: Fixed unit that it is connected to 

  */
{
  ELog::RegMethod RegA("CoolPad","createUnitVector");

  Origin=CUnit.getLinkPt(fixIndex);
  Y=CUnit.getLinkAxis(fixIndex);
  Z=CUnit.getZ();
  X=Z*Y;
  
  Origin += X*xStep+Z*zStep;
  // Ugly loop to put relative coordinates into absolute
  for(size_t i=0;i<CPts.size();i++)
    CPts[i]=X*CPts[i].X()+Y*CPts[i].Y()+
      Z*CPts[i].Z()+Origin;

  return;
}

void
CoolPad::createSurfaces(const attachSystem::FixedComp& CUnit)
  /*!
    Create All the surfaces
    \param dirIndex :: direction index of FixedComp
    \param CUnit :: FixedComp (reflector) to attach pad to.
  */
{
  ELog::RegMethod RegA("CoolPad","createSurface");

  SMap.addMatch(padIndex+1,CUnit.getLinkSurf(fixIndex));
  ModelSupport::buildPlane(SMap,padIndex+2,Origin+Y*thick,Y);
  ModelSupport::buildPlane(SMap,padIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,padIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,padIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,padIndex+6,Origin+Z*(height/2.0),Z);

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
  Out=ModelSupport::getComposite(SMap,padIndex,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  
  System.addCell(MonteCarlo::Qhull(cellIndex++,Mat,0.0,Out));


  return;
}

void
CoolPad::createWaterTrack(Simulation& System)
  /*!
    Build the cooling pad pipe line
  */
{  
  ELog::RegMethod RegA("CoolPad","createWaterTrack");

  ModelSupport::BoxLine WaterTrack
    (StrFunc::makeString(std::string("PadWater"),ID));
  WaterTrack.setPoints(CPts);
  WaterTrack.addSection(IWidth,IDepth,IMat,0.0);
  WaterTrack.setInitZAxis(Y);

  WaterTrack.createAll(System);
  // NoInsert();
  //  WaterTrack.insertPipeToCell(System,cellIndex-1);
  return;
}
  
void
CoolPad::createAll(Simulation& System,
		   const attachSystem::FixedComp& FUnit)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
  */
{
  ELog::RegMethod RegA("CoolPad","createAll");
  populate(System);

  createUnitVector(FUnit);
  createSurfaces(FUnit);
  createObjects(System);
  insertObjects(System);       
  createWaterTrack(System);
  return;
}
  
}  // NAMESPACE moderatorSystem
