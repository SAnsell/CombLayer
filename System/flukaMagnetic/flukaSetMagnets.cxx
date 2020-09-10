/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaProcess/flukaSetMagnets.cxx
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>
#include <complex>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <array>

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
#include "inputParam.h"
#include "Quaternion.h"
#include "Surface.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "AttachSupport.h"
#include "LinkSupport.h"
#include "Object.h"
#include "ObjectAddition.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimFLUKA.h"

#include "flukaPhysics.h"
#include "magnetUnit.h"
#include "flukaDefPhysics.h"


namespace flukaSystem
{
  

void
setMagneticExternal(SimFLUKA& System,
		    const mainSystem::inputParam& IParam)
  /*!
    Sets the external magnetic fields in object(s) 
    \param System :: Simulation
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod Rega("flukaDefPhysics[F]","setMagneticExternal");

  if (IParam.flag("MagStep"))
    {
      const size_t nSet=IParam.setCnt("MagStep");
      for(size_t index=0;index<nSet;index++)
	{
	  const std::set<MonteCarlo::Object*> Cells=
	    mainSystem::getNamedObjects
	    (System,IParam,"MagStep",index,0,"MagStep");
	  const double minV=IParam.getValueError<double>("MagStep",index,1,"MinStep not found");
	  const double maxV=IParam.getValueError<double>("MagStep",index,2,"MaxStep not found");
	  for(MonteCarlo::Object* mc : Cells)
	    mc->setMagStep(minV,maxV);
	}
    }

  if (IParam.flag("MagUnit"))
    {
      const size_t nSet=IParam.setCnt("MagUnit");
      for(size_t setIndex=0;setIndex<nSet;setIndex++)
	{
	  Geometry::Vec3D AOrg;
	  Geometry::Vec3D AY;
	  Geometry::Vec3D AZ;
	  
	  // General form is ::  Type : location : Param
	  size_t index(0);
	  ModelSupport::getObjectAxis
	    (System,"MagUnit",IParam,setIndex,index,AOrg,AY,AZ);
	  const Geometry::Vec3D Extent=
	    IParam.getCntVec3D("MagUnit",setIndex,index,"Extent");

	  std::vector<double> KV(4);
	  KV[0]=IParam.getValueError<double>
	    ("MagUnit",setIndex,index,"K Value");
	  for(size_t i=1;i<4;i++)
	    KV[i]=IParam.getDefValue<double>(0.0,"MagUnit",setIndex,index);

	  std::shared_ptr<flukaSystem::magnetUnit>
	    OPtr(new magnetUnit("MagUnit",setIndex));
	  OPtr->createAll(System,AOrg,AY,AZ,Extent,KV);
	  System.addMagnetObject(OPtr);
	}
    }
  return;
}

void
setModelPhysics(SimFLUKA& System,
		const mainSystem::inputParam& IParam)
  /*!
    Set the physics that needs a model
    \param System :: Simulation
    \param IParam :: Input parameters
  */
{
  ELog::RegMethod RegA("flukaDefPhysics","setModelPhysics");
  
  setXrayPhysics(System,IParam);
  setMagneticPhysics(System,IParam);
  setUserFlags(System,IParam);
  
  size_t nSet=IParam.setCnt("wMAT");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processMAT(System,IParam,index);
    }

  nSet=IParam.setCnt("wBIAS");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processBIAS(System,IParam,index);
    }
  
  nSet=IParam.setCnt("wCUT");    
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processCUT(System,IParam,index);
    }
  
  nSet=IParam.setCnt("wEMF");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processEMF(System,IParam,index);
    }
  
  nSet=IParam.setCnt("wEXP");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processEXP(System,IParam,index);
    }

  nSet=IParam.setCnt("wLAM");    
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processLAM(System,IParam,index);
    }

  nSet=IParam.setCnt("wBIAS");
  if (nSet)
    {
      flukaSystem::flukaImpConstructor A;
      for(size_t index=0;index<nSet;index++)
	A.processBIAS(System,IParam,index);
    }

  return; 
}

  
}
