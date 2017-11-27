/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderator/RefBolts.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "localRotate.h"
#include "masterRotate.h"
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
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "SimProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"
#include "RefBolts.h"

#include "Debug.h"

namespace moderatorSystem
{

RefBolts::RefBolts(const std::string& Key)  :
  attachSystem::FixedComp(Key,0),
  boltIndex(ModelSupport::objectRegister::Instance().cell(Key))
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

RefBolts::RefBolts(const RefBolts& A) : 
  attachSystem::FixedComp(A),
  boltIndex(A.boltIndex)
  /*!
    Copy constructor
    \param A :: RefBolts to copy
  */
{}

RefBolts&
RefBolts::operator=(const RefBolts& A)
  /*!
    Assignment operator
    \param A :: RefBolts to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
    }
  return *this;
}


RefBolts::~RefBolts() 
  /*!
    Destructor
  */
{}
  
void
RefBolts::createUnitVector(const attachSystem::FixedComp& CUnit,
			   const long int sideIndex)
/*!
    Create the unit vectors
    - Y Points towards WISH
    - X Across the moderator
    - Z up (towards the target)
    \param CUnit :: Fixed unit that it is connected to 
    \param sideIndex :: Side index for origin
  */
{
  ELog::RegMethod RegA("RefBolts","createUnitVector");

  FixedComp::createUnitVector(CUnit,0);
  Origin=CUnit.getLinkPt(sideIndex);
  return;
}

void 
RefBolts::createBoltGrp(Simulation& System,const std::string& subKey)
  /*!
    Add the boltsgrp : Note the coordinates are entered in 
    the lab frame reference so need to be moved back to current.
    \param System :: Simulation to add pipe to
    \param subKey :: Sub key
  */
{
  ELog::RegMethod RegA("RefBolts","createBoltGrp("+subKey+")");
  const FuncDataBase& Control=System.getDataBase();
  const masterRotate& MR=masterRotate::Instance();
  int bolt(0);
  int flag(1);
  int matN(0);   // default value
  std::vector<double> Radii;   // default values:
  std::ostringstream cx;  
     
  while(flag)
    {
      flag=0;
      cx.str("");
      cx<<keyName<<subKey<<bolt;
      if (Control.hasVariable(cx.str()+"Track0"))
	{
	  //  Get Default/base radii
	  matN=ModelSupport::EvalDefMat<int>(Control,cx.str()+"Mat",matN); 
	  if (Control.hasVariable(cx.str()+"Radius0"))
	    Radii=SimProcess::getVarVec<double>(Control,cx.str()+"Radius");
	  std::vector<Geometry::Vec3D> Track=
	    SimProcess::getVarVec<Geometry::Vec3D>(Control,cx.str()+"Track");
	  // Error test:
	  if (Radii.size()+1!=Track.size() ||
	      Track.size()<2)
	    {
	      throw ColErr::MisMatch<size_t>(Radii.size(),Track.size(),
					     RegA.getFull());
	    }

	  ModelSupport::PipeLine TBolt(cx.str());
	  std::vector<Geometry::Vec3D>::const_iterator vc;
	  for(vc=Track.begin();vc!=Track.end();vc++)
	    TBolt.addPoint(MR.reverseRotate(*vc)+Origin);

	  for(const double& R : Radii)
	    TBolt.addRadius(R,matN,0.0);
	  // Special case for headed bolt:
	  if (Radii.size()==2)
	    {
	      TBolt.setActive(0,3);
	      TBolt.setActive(1,1);
	    }
	  TBolt.createAll(System);
	  flag=1;
	  bolt++;
	} 
    }
  return;
}
  
void
RefBolts::createAll(Simulation& System,
		    const attachSystem::FixedComp& FUnit)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param FUnit :: Fixed Base unit
  */
{
  ELog::RegMethod RegA("RefBolts","createAll");

  createUnitVector(FUnit,7);
  createBoltGrp(System,"Nim");
  createBoltGrp(System,"Steel");
  createBoltGrp(System,"Buck");
  
  return;
}
  
}  // NAMESPACE moderatorSystem
