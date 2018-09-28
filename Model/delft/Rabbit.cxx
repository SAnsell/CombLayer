/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   delft/Rabbit.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <numeric>
#include <memory>
#include <boost/multi_array.hpp>

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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "FuelLoad.h"
#include "ReactorGrid.h"
#include "Rabbit.h"

namespace delftSystem
{

Rabbit::Rabbit(const std::string& Key,const int index)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key+std::to_string(index),3),
  baseName(Key),innerVoid(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param index :: ID number
  */
{}

Rabbit::Rabbit(const Rabbit& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  baseName(A.baseName),
  nLayer(A.nLayer),
  Radii(A.Radii),Mat(A.Mat),length(A.length),capThick(A.capThick),
  capMat(A.capMat),innerVoid(A.innerVoid)
  /*!
    Copy constructor
    \param A :: Rabbit to copy
  */
{}

Rabbit&
Rabbit::operator=(const Rabbit& A)
  /*!
    Assignment operator
    \param A :: Rabbit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      nLayer=A.nLayer;
      Radii=A.Radii;
      Mat=A.Mat;
      length=A.length;
      capThick=A.capThick;
      capMat=A.capMat;
      innerVoid=A.innerVoid;
    }
  return *this;
}


Rabbit::~Rabbit() 
 /*!
   Destructor
 */
{}

void
Rabbit::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase
 */
{
  ELog::RegMethod RegA("Rabbit","populate");

  if (Control.hasVariable(keyName+"GridKey"))
    {
      FixedOffset::populate(Control);
  
      objName=Control.EvalVar<std::string>(keyName+"GridKey");
      // First get inner widths:
      length=Control.EvalPair<double>(keyName,baseName,"Length");
      capThick=Control.EvalPair<double>(keyName,baseName,"CapThick");
      capMat=ModelSupport::EvalMat<int>(Control,keyName+"CapMat",
					baseName+"CapMat");
      
      nLayer=Control.EvalPair<size_t>(keyName,baseName,"NLayer");
      double D;
      int M;
      for(size_t i=0;i<nLayer;i++)
	{ 
	  const std::string mName="Mat"+StrFunc::makeString(i);
	  D=Control.EvalPair<double>(keyName,baseName,
				     "Radius"+StrFunc::makeString(i));
	  M=ModelSupport::EvalMat<int>(Control,keyName+mName,baseName+mName);
	  
	  Radii.push_back(D);
	  Mat.push_back(M);
	}
      
      sampleRadius=Control.EvalPair<double>(keyName,baseName,"SampleRadius");
      sampleMat=ModelSupport::EvalMat<int>(Control,keyName+"SampleMat",
					   baseName+"SampleMat");
      capsuleRadius=Control.EvalPair<double>(keyName,baseName,"CapsuleRadius");
      capsuleWall=Control.EvalPair<double>(keyName,baseName,"CapsuleWall");
      capsuleLen=Control.EvalPair<double>(keyName,baseName,"CapsuleLen");
      capsuleMat=ModelSupport::EvalMat<int>(Control,keyName+"CapsuleMat",
					    baseName+"CapsuleMat");
    }
  return;
}
  

void
Rabbit::createUnitVector(const ReactorGrid& RG)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param RG :: Reactor Grid
  */
{
  ELog::RegMethod RegA("Rabbit","createUnitVector");
  
  // PROCESS Origin of a point
  attachSystem::FixedComp::createUnitVector(RG);
  // NOW GET ORIGN:

  const std::pair<size_t,size_t> GPt=RG.getElementNumber(objName);
  Origin=RG.getCellOrigin(GPt.first,GPt.second);
  FixedOffset::applyOffset();

  return;
}

void
Rabbit::createUnitVector(const attachSystem::FixedComp& FC,
			 const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Points towards the beamline
    - X Across the Face
    - Z up (towards the target)
    \param FC :: A Contained FixedComp to use as basis set
    \param sideIndex :: link point [signed]
  */
{
  ELog::RegMethod RegA("Rabbit","createUnitVector");

  // PROCESS Origin of a point
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  FixedOffset::applyOffset();
		
  return;
}

void
Rabbit::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("Rabbit","createSurfaces");

  // Outer layers

  ModelSupport::buildPlane(SMap,buildIndex+1,
			   Origin,Z);
  ModelSupport::buildPlane(SMap,buildIndex+2,
			   Origin+Z*length,Z);
  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Z*capThick,Z);

  int RI(buildIndex);
  for(size_t i=0;i<nLayer;i++)
    {
      ModelSupport::buildCylinder(SMap,RI+7,
				  Origin,Z,Radii[i]);
      RI+=10;
    }

  // Create sample capulse [500]
  ModelSupport::buildCylinder(SMap,buildIndex+507,
			      Origin,Z,capsuleRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+517,
			      Origin,Z,capsuleRadius+capsuleWall); 
  ModelSupport::buildPlane(SMap,buildIndex+501,Origin+
			   Z*(capsuleZShift-capsuleLen/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+502,Origin+
			   Z*(capsuleZShift+capsuleLen/2.0),Z);
  ModelSupport::buildSphere(SMap,buildIndex+508,
			    Origin+Z*(capsuleZShift-capsuleLen/2.0),
			    capsuleRadius);
  ModelSupport::buildSphere(SMap,buildIndex+509,
			    Origin+Z*(capsuleZShift+capsuleLen/2.0),
			    capsuleRadius);
  ModelSupport::buildSphere(SMap,buildIndex+518,
			    Origin+Z*(capsuleZShift-capsuleLen/2.0),
			    capsuleRadius+capsuleWall);
  ModelSupport::buildSphere(SMap,buildIndex+519,
			    Origin+Z*(capsuleZShift+capsuleLen/2.0),
			    capsuleRadius+capsuleWall);
  // Create sample 
  ModelSupport::buildSphere(SMap,buildIndex+1007,
			    Origin+Z*capsuleZShift,sampleRadius);

  return;
}


void
Rabbit::createObjects(Simulation& System)
  /*!
    Adds the BeamLne components
    \param System :: Simulation to add beamline to
  */
{
  ELog::RegMethod RegA("Rabbit","createObjects");
  

  std::string Out;

  // Add sample
  Out=ModelSupport::getComposite(SMap,buildIndex," -1007 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,sampleMat,0.0,Out));

  // Capsule
  Out=ModelSupport::getComposite(SMap,buildIndex
,				 "-507 (-508:501) (-509:-502) 1007 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  Out=ModelSupport::getComposite(SMap,buildIndex,"501 -502 507 -517 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,capsuleMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex,"-501 508 -518");
  System.addCell(MonteCarlo::Qhull(cellIndex++,capsuleMat,0.0,Out));
  Out=ModelSupport::getComposite(SMap,buildIndex,"502 509 -519");
  System.addCell(MonteCarlo::Qhull(cellIndex++,capsuleMat,0.0,Out));

  const std::string capExclude=
    ModelSupport::getComposite(SMap,buildIndex," (507:508:509) ");

  int RI=buildIndex-10;
  for(size_t i=0;i<nLayer;i++)
    {
      if (i)
	Out=ModelSupport::getComposite(SMap,buildIndex,RI," 1 -2 7M -17M");
      else
	Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 -7M ")+
	  capExclude;
      System.addCell(MonteCarlo::Qhull(cellIndex++,Mat[i],0.0,Out));
      RI+=10;
    }
  Out=ModelSupport::getComposite(SMap,buildIndex,RI," -1 11 -7M");
  System.addCell(MonteCarlo::Qhull(cellIndex++,capMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,RI," -2 11 -7M");
  addOuterSurf(Out);

  return;
}

void
Rabbit::createLinks()
  /*!
    Create All the links:
    - 0 : First surface
    - 1 : Exit surface
    - 2 : Inner face
  */
{
  ELog::RegMethod RegA("Rabbit","createLinks");

  FixedComp::setConnect(0,Origin-Z*capThick,-Z); 
  FixedComp::setConnect(1,Origin+Z*length,Z);
  FixedComp::setConnect(2,Origin+Y*Radii.back(),Y);

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+11));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7+
					 10*static_cast<int>(nLayer-1)));

  return;
}



int
Rabbit::createAll(Simulation& System,
		  const ReactorGrid& RG)
  /*!
    Global creation of the vac-vessel
    \param System :: Simulation to add vessel to
    \param RG :: Reactor Grid
    \return success / failure
  */
{
  ELog::RegMethod RegA("Rabbit","createAll");
  populate(System.getDataBase());
  if (!objName.empty())
    {
      createUnitVector(RG);
      createSurfaces();
      createObjects(System);
      createLinks();
      insertObjects(System);
      return 1;
    }
  return 0;
      

}


} // NAMESPACE delftSystem

