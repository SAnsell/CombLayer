/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sinbadBuild/sinbadShield.cxx
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
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"

#include "sinbadShield.h"

namespace sinbadSystem
{

sinbadShield::sinbadShield(const std::string& Key) : 
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}


sinbadShield::~sinbadShield() 
  /*!
    Destructor
  */
{}


void
sinbadShield::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("sinbadShield","populate");

  const FuncDataBase& Control=System.getDataBase();

  offSetX=Control.EvalVar<double>(keyName+"OffSetX");
  offSetY=Control.EvalVar<double>(keyName+"OffSetY");
  offSetZ=Control.EvalVar<double>(keyName+"OffSetZ");

  heightSlab=Control.EvalVar<double>(keyName+"HeightSlab");
  lengthSlab=Control.EvalVar<double>(keyName+"LengthSlab");
  temperatureSlab=Control.EvalVar<double>(keyName+"TemperatureSlab");

  nSlab=Control.EvalVar<size_t>(keyName+"FrontSlabN");
 
  double W;
  double L;
  // double LM(0);
  double H;
  // double HM(0);
  int M;
  LM=0;
  HM=0;
  for(size_t i=0;i<nSlab;i++)
    {
      W=Control.EvalVar<double>(keyName+"WidthSlab"+std::to_string(i));
      width.push_back(W);
      L=Control.EvalDefVar<double>
	(keyName+"LengthSlab"+std::to_string(i),lengthSlab);
      length.push_back(L);
      if(LM<L) 
	LM=L;
      
      H=Control.EvalDefVar<double>
	(keyName+"HeightSlab"+std::to_string(i),heightSlab);
      height.push_back(H);
      if(HM<H) 
	HM=H;

      M=ModelSupport::EvalMat<int>
	(Control,keyName+"MaterialSlab"+std::to_string(i));
      mat.push_back(M);   
    }

  //detector to insert
  int detN;
  detN=Control.EvalVar<int>("49DetectorPositionN");
  double DY;

  for(int i=0;i<detN;i++)
    {
      DY=Control.EvalVar<double>("49DetectorOffSetY"+std::to_string(i));
      //   PY.push_back(DY);
      // this for 49
    PY.push_back(DY+1.45);

    
    }
  return;
}


void
sinbadShield::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: FixedComponent for origin
  */
{
  ELog::RegMethod RegA("sinbadShield","createUnitVector");

  FixedComp::createUnitVector(FC);

  // setting origin at lower left vertex of the first slab
  // Origin+=X*offSetX+Y*offSetY+Z*offSetZ;

 return;
}


void
sinbadShield::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("sinbadShield","createSurface");
  //  
 
  double cumThick=offSetY;

  //  ModelSupport::buildPlane(SMap,buildIndex+3,Origin,Y );
  int SI1(buildIndex);
  int SI2(buildIndex+1000);

  for(size_t i=0;i<=nSlab;i++)
   {

     ModelSupport::buildPlane(SMap,SI1+1,Origin-X*(length[i]/2.0),X);
     ModelSupport::buildPlane(SMap,SI1+2,Origin+X*(length[i]/2.0),X);

     ModelSupport::buildPlane(SMap,SI1+3,Origin+Y*cumThick,Y);
      cumThick+=width[i];  

     ModelSupport::buildPlane(SMap,SI1+5,Origin-Z*(height[i]/2.0),Z);
     ModelSupport::buildPlane(SMap,SI1+6,Origin+Z*(height[i]/2.0),Z);

    SI1+=10;
   }

     ModelSupport::buildPlane(SMap,SI2+1,Origin-X*(LM/2.0),X);
     ModelSupport::buildPlane(SMap,SI2+2,Origin+X*(LM/2.0),X);

     ModelSupport::buildPlane(SMap,SI2+5,Origin-Z*(HM/2.0),Z);
     ModelSupport::buildPlane(SMap,SI2+6,Origin+Z*(HM/2.0),Z);

   return;
}


void
sinbadShield::createObjects(Simulation& System)
  /*!
    Create all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("sinbadShield","createObjects");
  std::string Out;
  std::string OutTot;

  int SI2(buildIndex);

  for(size_t i=0;i<nSlab;i++)
   {
     Out=ModelSupport::getComposite(SMap,buildIndex,SI2,"1M -2M 3M -13M 5M -6M");
     System.addCell(MonteCarlo::Object(cellIndex++,mat[i],temperatureSlab,Out));
     SI2+=10; 
     // 49
     if (i==3)shieldCell=cellIndex-1;

   }

   OutTot=ModelSupport::getComposite(SMap,buildIndex,SI2,
				     "(1001 -1002 3 -3M 1005 -1006)");
      

  addOuterSurf(OutTot);

  return;
}


void
sinbadShield::createAll(Simulation& System,
			const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Component for origin
  */
{
  ELog::RegMethod RegA("sinbadShield","createAll");

  populate(System);
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  insertObjects(System);

  return;
}
  
} 
