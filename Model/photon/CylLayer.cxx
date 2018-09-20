/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   photon/CylLayer.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
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
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BoundOuter.h"
#include "CylLayer.h"

namespace photonSystem
{
  //LINFO Stuff:
void
LInfo::resize(const size_t N)
  /*!
    Resize the vectors for mat/radii.
    \param N :: Index size
  */
{
  ELog::RegMethod RegA("LInfo","resize");
  if (!N)
    throw ColErr::IndexError<size_t>(N,0,"N size cant be zero");
  Radii.resize(N-1);
  Mat.resize(N);
  Temp.resize(N);
  return;
}
      
CylLayer::CylLayer(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,6),
  attachSystem::BoundOuter(),
  layerIndex(ModelSupport::objectRegister::Instance().cell(Key)), 
  cellIndex(layerIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

CylLayer::~CylLayer()
  /*!
    Destructor
  */
{}

CylLayer::CylLayer(const CylLayer& A) :
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::BoundOuter(A),
  layerIndex(A.layerIndex),cellIndex(A.cellIndex),
  outerRadius(A.outerRadius),
  nLayers(A.nLayers),LVec(A.LVec)
  /*!
    Copy constructor
    \param A :: CylLayer to copy
  */
{}

CylLayer&
CylLayer::operator=(const CylLayer& A)
  /*!
    Assignment operator
    \param A :: CylLayer to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::BoundOuter::operator=(A);
      cellIndex=A.cellIndex;
      outerRadius=A.outerRadius;
      nLayers=A.nLayers;
      LVec=A.LVec;
    }
  return *this;
}

CylLayer*
CylLayer::clone() const
  /*!
    Clone copy constructor
    \return copy of this
  */
{
  return new CylLayer(*this);
}

void
CylLayer::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("CylLayer","populate");

  FixedOffset::populate(Control);

  // Master values
  outerRadius=Control.EvalVar<double>(keyName+"OuterRadius");
  
  nLayers=Control.EvalVar<size_t>(keyName+"NLayers");
  if (nLayers<1)
    throw ColErr::IndexError<size_t>(nLayers,1,"NLayers not zero");

  for(size_t i=0;i<nLayers;i++)
    {
      const std::string KN=keyName+StrFunc::makeString(i);
      const size_t copyNum=
	Control.EvalDefVar<size_t>(KN+"CopyCell",i);
      if (copyNum==i)
	{
	  LInfo LI;
	  Control.EvalPair<double>(KN+"Thick",keyName+"Thick");
	  LI.thick=Control.EvalPair<double>(KN+"Thick",keyName+"Thick");
	  LI.nDisk=Control.EvalPair<size_t>(KN+"NUnits",keyName+"NUnits");
	  LI.resize(LI.nDisk);
	  for(size_t j=0;j<LI.nDisk;j++)
	    {
	      const std::string subNum=StrFunc::makeString(j);
	      if (j+1!=LI.nDisk)
		LI.Radii[j]=Control.EvalPair<double>(KN+"Radius"+subNum,
						     keyName+"Radius"+subNum);
	      
	      LI.Mat[j]=ModelSupport::EvalMat<int>(Control,KN+"Mat"+subNum,
						   keyName+"Mat"+subNum);
	      LI.Temp[j]=Control.EvalDefPair<double>
		(KN+"Temp"+subNum,keyName+"Temp"+subNum,0.0);
	    }
	  LVec.push_back(LI);
	}
      else
	LVec.push_back(LVec[copyNum]);
    }
  return;
}

void
CylLayer::createUnitVector(const attachSystem::FixedComp& FC,
			   const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("CylLayer","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
CylLayer::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
{
  ELog::RegMethod RegA("CylLayer","createSurfaces");

  // Outer surface:
  if (!(BoundOuter::setFlag & 2))
    {
      ModelSupport::buildCylinder(SMap,layerIndex+8,
				  Origin,Y,outerRadius);
      outerStruct.procSurfNum(-SMap.realSurf(layerIndex+8));
    }
	  
  // Divide plane
  int index(layerIndex);
  Geometry::Vec3D OR(Origin);
  for(const LInfo& LI : LVec)
    {
      ModelSupport::buildPlane(SMap,index+1,OR,Y);
      int subIndex(0);
      for(const double& R : LI.Radii)
	{
	  ModelSupport::buildCylinder(SMap,index+subIndex+7,
				      OR,Y,R);
	  subIndex+=10;
	}
      OR+=Y*LI.thick;
      index+=100;
    }
  // Add last plane
  ModelSupport::buildPlane(SMap,index+1,OR,Y);  
  return; 
}

void
CylLayer::createObjects(Simulation& System)
  /*!
    Create the vaned moderator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("CylLayer","createObjects");

  std::string Out;
  int index(layerIndex);
  for(const LInfo& LI : LVec)
    {
      int subIndex(index);
      const std::string layOut=
	ModelSupport::getComposite(SMap,index," 1 -101 ");

      // Inner cell:
      if (LI.nDisk==1)
	Out=outerStruct.display();
      //	Out=ModelSupport::getComposite(SMap,layerIndex," -8 ");
      else 
	Out=ModelSupport::getComposite(SMap,subIndex," -7 ");
	    
      System.addCell(MonteCarlo::Qhull(cellIndex++,LI.Mat[0],LI.Temp[0],
				       Out+layOut));
      // Inner bound cells:
      for(size_t i=1;i<LI.nDisk-1;i++)
	{
	  Out=ModelSupport::getComposite(SMap,subIndex," 7 -17 ");
	  System.addCell(MonteCarlo::Qhull
			 (cellIndex++,LI.Mat[i],LI.Temp[i],Out+layOut));
	  subIndex+=10;
	}
      // Outer cell [if required]:
      if (LI.nDisk>1)
	{
	  Out=ModelSupport::getComposite(SMap,subIndex," 7 ");
	  Out+=outerStruct.display();
	  System.addCell(MonteCarlo::Qhull
			 (cellIndex++,LI.Mat.back(),LI.Temp.back(),
			  Out+layOut));
	}
      index+=100;
    }
  Out=ModelSupport::getComposite(SMap,layerIndex,index," 1 -1M");
  addOuterSurf(Out+outerStruct.display());
  return; 
}

void
CylLayer::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("CylLayer","createLinks");

  double tThick(0.0);
  for(const LInfo& LI : LVec)
    tThick+=LI.thick;
  const int NL(layerIndex+static_cast<int>(nLayers-1)*100);

  
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(layerIndex+1));

  FixedComp::setConnect(1,Origin+Y*tThick,Y);
  FixedComp::setLinkSurf(1,-SMap.realSurf(NL+101));

  FixedComp::setConnect(2,Origin+Y*(tThick/2.0)-X*outerRadius,-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(layerIndex+8));
  
  FixedComp::setConnect(3,Origin+Y*(tThick/2.0)+X*outerRadius,X);
  FixedComp::setLinkSurf(3,SMap.realSurf(layerIndex+8));
  
  FixedComp::setConnect(4,Origin+Y*(tThick/2.0)-Z*outerRadius,-Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(layerIndex+8));

  FixedComp::setConnect(5,Origin+Y*(tThick/2.0)+Z*outerRadius,Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(layerIndex+8));

  return;
}

void
CylLayer::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: surface offset
   */
{
  ELog::RegMethod RegA("CylLayer","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE photonSystem
