/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   zoom/ZoomRoof.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <boost/bind.hpp>
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
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Rules.h"
#include "surfFunctors.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "SimProcess.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "shutterBlock.h"
#include "LinkUnit.h"  
#include "FixedComp.h" 
#include "SecondTrack.h"
#include "TwinComp.h"
#include "InsertComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "GeneralShutter.h"
#include "BulkShield.h"
#include "bendSection.h"
#include "varBlock.h"
#include "ZoomBend.h"
#include "ZoomRoof.h"

namespace zoomSystem
{

ZoomRoof::ZoomRoof(const std::string& Key) : 
  attachSystem::FixedComp(Key,0),attachSystem::ContainedComp(),
  roofIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(roofIndex+1),populated(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

ZoomRoof::ZoomRoof(const ZoomRoof& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  roofIndex(A.roofIndex),cellIndex(A.cellIndex),
  populated(A.populated),nSteel(A.nSteel),nExtra(A.nExtra),
  Steel(A.Steel),Extra(A.Extra),steelMat(A.steelMat),extraMat(A.extraMat),
  monoWallSurf(A.monoWallSurf)
  /*!
    Copy constructor
    \param A :: ZoomRoof to copy
  */
{}

ZoomRoof&
ZoomRoof::operator=(const ZoomRoof& A)
  /*!
    Assignment operator
    \param A :: ZoomRoof to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      populated=A.populated;
      nSteel=A.nSteel;
      nExtra=A.nExtra;
      Steel=A.Steel;
      Extra=A.Extra;
      steelMat=A.steelMat;
      extraMat=A.extraMat;
      monoWallSurf=A.monoWallSurf;
    }
  return *this;
}

ZoomRoof::~ZoomRoof() 
  /*!
    Destructor
  */
{}


void
ZoomRoof::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("ZoomRoof","populate");

  const FuncDataBase& Control=System.getDataBase();

  nSteel=Control.EvalVar<size_t>(keyName+"NSteel");
  nExtra=Control.EvalVar<size_t>(keyName+"NExtra");
  
  const size_t DSize(2);
  const std::string sndKey[DSize]=
    {"Height","Step"}; 
  
  for(size_t i=0;i<nSteel;i++)
    {
      // note : zero int item 
      Steel.push_back(varBlock(DSize,sndKey,0,0));
      for(size_t j=0;j<DSize;j++)
	{
	  std::ostringstream cx;
	  cx<<keyName<<sndKey[j]<<i+1;
	  if (Control.hasVariable(cx.str()))
	    Steel[i].setVar(sndKey[j],Control.EvalVar<double>(cx.str()));
	  else if (!i)
	    {
	      ELog::EM<<"Failed to connect on first component:: "
		      <<sndKey[j]<<ELog::endErr;
	    }
	  else
	    Steel[i].setVar(sndKey[j],Steel[i-1]);
	}  
    }

  for(size_t i=0;i<nExtra;i++)
    {
      Extra.push_back(varBlock(DSize,sndKey,0,sndKey+DSize));
      for(size_t j=0;j<DSize;j++)
	{
	  std::ostringstream cx;
	  cx<<keyName<<"E"<<sndKey[j]<<i+1;
	  if (Control.hasVariable(cx.str()))
	    Extra[i].setVar(sndKey[j],Control.EvalVar<double>(cx.str()));
	  else if (!i)
	    {
	      ELog::EM<<"Failed to connect on first component:: "
		      <<"E"<<sndKey[j]<<ELog::endErr;
	    }
	  else
	    Extra[i].setVar(sndKey[j],Steel[i-1]);
	}  
    }

  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  extraMat=ModelSupport::EvalMat<int>(Control,keyName+"ExtraMat");
  populated |= 1;
  return;
}

void
ZoomRoof::createUnitVector(const attachSystem::FixedComp& ZChopper)
  /*!
    Create the unit vectors
    \param ZChopper :: Zoom Chopper unit [for origin]
  */
{
  ELog::RegMethod RegA("ZoomRoof","createUnitVector");
  attachSystem::FixedComp::createUnitVector(ZChopper.getLinkPt(5),
					    ZChopper.getY(),
					    ZChopper.getZ());
  return;
}

void
ZoomRoof::createSurfaces(const shutterSystem::GeneralShutter& GS,
			 const attachSystem::FixedComp& ZChopper,
			 const attachSystem::FixedComp& ZColl)
  /*!
    Create All the surfaces
    \param GS :: GeneralShutter [for divide]
    \param ZChopper :: ZoomChopper
    \param ZColl :: Collimator Unit
  */
{
  ELog::RegMethod RegA("ZoomRoof","createSurface");
  

  SMap.addMatch(roofIndex+100,GS.getDivideSurf());
  SMap.addMatch(roofIndex+1,monoWallSurf);

  SMap.addMatch(roofIndex+3,ZChopper.getLinkSurf(2));  // side
  SMap.addMatch(roofIndex+4,ZChopper.getLinkSurf(3));
  SMap.addMatch(roofIndex+5,ZChopper.getLinkSurf(5));
  SMap.addMatch(roofIndex+2,ZChopper.getLinkSurf(1));
  // Stuff from the collimator
  SMap.addMatch(roofIndex+13,ZColl.getLinkSurf(2));  // side
  SMap.addMatch(roofIndex+14,ZColl.getLinkSurf(3));
  SMap.addMatch(roofIndex+12,ZColl.getLinkSurf(1));
  
  int sOff(roofIndex+500);
  for(size_t i=0;i<nSteel;i++)
    {
      ModelSupport::buildPlane(SMap,sOff+2,Origin+
			       Y*Steel[i].getVar<double>("Step"),Y);
      ModelSupport::buildPlane(SMap,sOff+6,Origin+
			       Z*Steel[i].getVar<double>("Height"),Z);
      sOff+=100;
    }

  sOff=roofIndex+501;
  for(size_t i=0;i<nExtra;i++)
    {
      ModelSupport::buildPlane(SMap,sOff+2,Origin+
			       Y*Extra[i].getVar<double>("Step"),Y);
      ModelSupport::buildPlane(SMap,sOff+6,Origin+
			       Z*Extra[i].getVar<double>("Height"),Z);
      sOff+=100;
    }

  return;
}

void
ZoomRoof::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("ZoomRoof","createObjects");

  std::string CompUnit;
  std::string Out;

  // Outer steel
  int sOff(roofIndex+400);              // NOTE :: needed for previous unit
  for(size_t i=0;i<nSteel-1;i++)
    {
      if (i)           
	Out+=ModelSupport::getComposite(SMap,sOff,"(-106:102:-2)");
      else
	Out+=ModelSupport::getComposite(SMap,sOff,"(-106:102)");
      sOff+=100;
    }
  if (nSteel>1)
    Out+=ModelSupport::getComposite(SMap,sOff," (-106:-2) -102 ");
  else if (nSteel)
    Out+=ModelSupport::getComposite(SMap,sOff," -106 -102 ");
      
  Out+=ModelSupport::getComposite(SMap,roofIndex," 5 ((100 1 -2 3 -4) : (2 13 -14)) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,steelMat,0.0,Out));
  
  if (nExtra)
    {
      sOff=roofIndex+400;
      int eOff=roofIndex+401;
      Out="";
      for(size_t i=0;i<nExtra;i++)
	{
	  // Standard capping units:
	  if (i==nExtra-1)
	    CompUnit+=ModelSupport::getComposite(SMap,eOff," (-106:-2) -102 ");
	  else if (!i)
	    CompUnit+=ModelSupport::getComposite(SMap,eOff,"(-106:102)");	
	  else 
	    CompUnit+=ModelSupport::getComposite(SMap,eOff,"(-106:102:-2)");
	  eOff+=100;
	}
      
      for(size_t j=0;j<nSteel;j++)
	{
	  if (j)           
	    Out+=ModelSupport::getComposite(SMap,sOff,"(106:102:-2)");
	  else
	    Out+=ModelSupport::getComposite(SMap,sOff,"(106:102)");
	  sOff+=100;
	}

      CompUnit+=ModelSupport::getComposite(SMap,roofIndex," 5 ((100 1 -2 3 -4) : (2 13 -14)) ");
      Out+=CompUnit;
      System.addCell(MonteCarlo::Qhull(cellIndex++,extraMat,0.0,Out));
    }
  if (nExtra)
    addOuterSurf(CompUnit);        
  else
    addOuterSurf(Out);        
  return;
}

void
ZoomRoof::createAll(Simulation& System,
		    const shutterSystem::GeneralShutter& GShutter,
		    const attachSystem::FixedComp& ZChopper,
		    const attachSystem::FixedComp& ZColl)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param GShutter :: Shutter Unit
    \param ZChopper :: Zoom Chopper
    \param ZColl :: Zoom Collimator
  */
{
  ELog::RegMethod RegA("ZoomRoof","createAll");
  
  populate(System);
  createUnitVector(ZChopper);
  createSurfaces(GShutter,ZChopper,ZColl);
  createObjects(System);
   
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE shutterSystem
