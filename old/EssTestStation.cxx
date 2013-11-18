/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/EssTestStation.cxx
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "surfDivide.h"
#include "surfDIter.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Object.h"
#include "Qhull.h"
#include "KGroup.h"
#include "SrcData.h"
#include "SrcItem.h"
#include "Source.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "BeamWindow.h"
#include "EssTestStation.h"

namespace essSystem
{

EssTestStation::EssTestStation(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6),
  TSTIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(TSTIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

EssTestStation::EssTestStation(const EssTestStation& A) :
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  TSTIndex(A.TSTIndex), cellIndex(A.cellIndex), OffsetX(A.OffsetX),
  OffsetY(A.OffsetY), OffsetZ(A.OffsetX),AngleXY(A.AngleXY),AngleZ(A.AngleZ),
  SampleBlockLength(A.SampleBlockLength),SampleBlockWidth(A.SampleBlockWidth),
  SampleBlockHeight(A.SampleBlockHeight),SampleBlockColumns(A.SampleBlockColumns),
  SampleBlockRows(A.SampleBlockRows),SampleBlockMaterial(A.SampleBlockMaterial),
  SampleBlockTemperature(A.SampleBlockTemperature),
  SampleCladdingMaterial(A.SampleCladdingMaterial),
  SampleCladdingThickness(A.SampleCladdingThickness),
  SampleCladdingTemperature(A.SampleCladdingTemperature),
  BaffleMaterial(A.BaffleMaterial),
  BaffleThickness(A.BaffleThickness),
  BaffleTemperature(A.BaffleTemperature),
  BaffleCladdingMaterial(A.BaffleCladdingMaterial),
  BaffleCladdingThickness(A.BaffleCladdingThickness),
  BaffleCladdingTemperature(A.BaffleCladdingTemperature),
  CoolingMaterial(A.CoolingMaterial),
  CoolingThickness(A.CoolingThickness),
  CoolingTemperature(A.CoolingTemperature),
  CoolingCladdingMaterial(A.CoolingCladdingMaterial),
  CoolingCladdingThickness(A.CoolingCladdingThickness),
  CoolingCladdingTemperature(A.CoolingCladdingTemperature),
  mainDetCell(A.mainDetCell)
  /*!
    Constructor BUT ALL variable are left unpopulated.
  */
{}

EssTestStation&
EssTestStation::operator=(const EssTestStation& A) 
{
  if (&A!=this)
    {
     attachSystem::ContainedComp::operator=(A);
     attachSystem::FixedComp::operator=(A);
     cellIndex=A.cellIndex;
     OffsetX=A.OffsetX;
     OffsetY=A.OffsetY;
     OffsetZ=A.OffsetZ;
     AngleXY=A.AngleXY;
     AngleZ=A.AngleZ;

     SampleBlockLength=A.SampleBlockLength;
     SampleBlockWidth=A.SampleBlockWidth;
     SampleBlockHeight=A.SampleBlockHeight;
     SampleBlockColumns=A.SampleBlockColumns;
     SampleBlockRows=A.SampleBlockRows;
     SampleBlockMaterial=A.SampleBlockMaterial;
     SampleBlockTemperature=A.SampleBlockTemperature;

     SampleCladdingMaterial=A.SampleCladdingMaterial;
     SampleCladdingThickness=A.SampleCladdingThickness;
     SampleCladdingTemperature=A.SampleCladdingTemperature;

     BaffleMaterial=A.BaffleMaterial;
     BaffleThickness=A.BaffleThickness;
     BaffleTemperature=A.BaffleTemperature;

     BaffleCladdingMaterial=A.BaffleCladdingMaterial;
     BaffleCladdingThickness=A.BaffleCladdingThickness;
     BaffleCladdingTemperature=A.BaffleCladdingTemperature;

     CoolingMaterial=A.CoolingMaterial;
     CoolingThickness=A.CoolingThickness;
     CoolingTemperature=A.CoolingTemperature;

     CoolingCladdingMaterial=A.CoolingCladdingMaterial;
     CoolingCladdingThickness=A.CoolingCladdingThickness;
     CoolingCladdingTemperature=A.CoolingCladdingTemperature;

    }
  return *this;
}

EssTestStation*
EssTestStation::clone() const
  /*!
    Clone funciton
    \return new(this)
  */
{
  return new EssTestStation(*this);
}

EssTestStation::~EssTestStation() 
  /*!
    Destructor
  */
{}

void
EssTestStation::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("EssTestStation","populate");

  const FuncDataBase& Control=System.getDataBase();

  OffsetX=Control.EvalVar<double>(keyName+"OffsetX");
  OffsetY=Control.EvalVar<double>(keyName+"OffsetY");
  OffsetZ=Control.EvalVar<double>(keyName+"OffsetZ");

  AngleXY=Control.EvalVar<double>(keyName+"AngleXY");
  AngleZ=Control.EvalVar<double>(keyName+"AngleZ");

  SampleBlockLength=Control.EvalVar<double>(keyName+"SampleBlockLength");
  SampleBlockWidth=Control.EvalVar<double>(keyName+"SampleBlockWidth");
  SampleBlockHeight=Control.EvalVar<double>(keyName+"SampleBlockHeight");
  SampleBlockColumns=Control.EvalVar<int>(keyName+"SampleBlockColumns");
  SampleBlockRows=Control.EvalVar<int>(keyName+"SampleBlockRows");
  SampleBlockMaterial=Control.EvalVar<int>(keyName+"SampleBlockMaterial");
  SampleBlockTemperature=Control.EvalVar<double>(keyName+"SampleBlockTemperature");

  SampleCladdingMaterial=Control.EvalVar<int>(keyName+"SampleCladdingMaterial");
  SampleCladdingThickness=Control.EvalVar<double>(keyName+"SampleCladdingThickness");
  SampleCladdingTemperature=Control.EvalVar<double>(keyName+"SampleCladdingTemperature");

  BaffleMaterial=Control.EvalVar<int>(keyName+"BaffleMaterial");
  BaffleThickness=Control.EvalVar<double>(keyName+"BaffleThickness");
  BaffleTemperature=Control.EvalVar<double>(keyName+"BaffleTemperature");

  BaffleCladdingMaterial=Control.EvalVar<int>(keyName+"BaffleCladdingMaterial");
  BaffleCladdingThickness=Control.EvalVar<double>(keyName+"BaffleCladdingThickness");
  BaffleCladdingTemperature=Control.EvalVar<double>(keyName+"BaffleCladdingTemperature");

  CoolingMaterial=Control.EvalVar<int>(keyName+"CoolingMaterial");
  CoolingThickness=Control.EvalVar<double>(keyName+"CoolingThickness");
  CoolingTemperature=Control.EvalVar<double>(keyName+"CoolingTemperature");

  CoolingCladdingMaterial=Control.EvalVar<int>(keyName+"CoolingCladdingMaterial");
  CoolingCladdingThickness=Control.EvalVar<double>(keyName+"CoolingCladdingThickness");
  CoolingCladdingTemperature=Control.EvalVar<double>(keyName+"CoolingCladdingTemperature");
 
  return;
}

void
EssTestStation::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: FixedComponent for origin
  */
{
  ELog::RegMethod RegA("EssTestStation","createUnitVector");

  FixedComp::createUnitVector(FC);
  Origin+=X*OffsetX+Y*OffsetY+Z*OffsetZ;

  const Geometry::Quaternion Qz=
    Geometry::Quaternion::calcQRotDeg(AngleZ,X);
  const Geometry::Quaternion Qxy=
    Geometry::Quaternion::calcQRotDeg(AngleXY,Z);
  Qz.rotate(Y);
  Qz.rotate(Z);
  Qxy.rotate(Y);
  Qxy.rotate(X);
  Qxy.rotate(Z);

 return;
}


void
EssTestStation::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("EssTestStation","createSurface");
  
  
  const double XStep(SampleBlockLength/SampleBlockRows);
  const double YStep(SampleBlockWidth/SampleBlockColumns);
  Geometry::Vec3D XPt(Origin-X*(XStep*(SampleBlockRows/2.0)));
  Geometry::Vec3D YPt(Origin-Y*(YStep*(SampleBlockColumns/2.0)));

  int TI(TSTIndex+3);
  for(size_t i=0;i<SampleBlockRows+1;i++)  
     {
       ModelSupport::buildPlane(SMap,TI,XPt-X*(XStep/2.0),X);
       XPtr+=X*XStep;
       TI+=10;
     }

  TI=TSTIndex+1;
  for(size_t i=0;i<SampleBlockColumns+1;i++)  
     {
       ModelSupport::buildPlane(SMap,TI,YPt-Y*(YStep/2.0),Y);
       YPtr+=Y*YStep;
       TI+=10;
     }
  ModelSupport::buildPlane(SMap,TSTIndex+5,
			   Origin-Z*(SampleBlockHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,TSTIndex+6,
			   Origin+Z*(SampleBlockHeight/2.0),Z);

   return;
}

void
EssTestStation::createObjects(Simulation& System)
  /*!
    Create all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("EssTestStation","createObjects");
  std::string Out;

  const std::string gapStr=
    ModelSupport::getComposite(SMap,TSTIndex," 5 -6 ");

  int TIx(TSTIndex);
  int TIy(TSTIndex);
  for(size_t i=0;i<SampleBlockRows;i++)  
    {
      TIx+=10;
      TIy=TSTIndex;
      for(size_t j=0;j<SampleBlockColumns;j++)  
	{
	  Out=ModelSupport::getComposite(SMap,TIx,TIy," 3 -13 1M -11M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,SampleBlockMat,
					   Out+gapStr));
	  TIy+=10;
	}
    }
  
  Out=ModelSupport::getComposite(SMap,TSTIndex," 3 1 5 -6 ");
  OUt+=ModelSupport::getComposite(SMap,TIx,TIy," -3 -1 ");
  addOutSurf(Out);
  return
}

void
EssTestStation::createLinks()
  /*!
    Create all the links
  */
{

  return;
}

void
EssTestStation::createAll(Simulation& System,
		 const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Component for origin
  */
{
  ELog::RegMethod RegA("EssTestStation","createAll");

  populate(System);
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}
  
}  // NAMESPACE TMRsystem
