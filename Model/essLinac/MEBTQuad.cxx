/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essLinac/MEBTQuad.cxx
 *
 * Copyright (c) 2017-2021 by Konstantin Batkov
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
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "inputParam.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "MEBTQuad.h"

namespace essSystem
{

MEBTQuad::MEBTQuad(const std::string& Base,
		   const std::string& Key,const size_t Index) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Base+Key+std::to_string(Index),6),
  attachSystem::CellMap(),
  baseName(Base),
  extraName(Base+Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

MEBTQuad::MEBTQuad(const MEBTQuad& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  baseName(A.baseName),
  extraName(A.extraName),
  engActive(A.engActive),
  length(A.length),
  pipeRad(A.pipeRad),
  thickness(A.thickness),
  voidMat(A.voidMat),
  pipeMat(A.pipeMat),
  yokeMat(A.yokeMat),
  coilMat(A.coilMat)

  /*!
    Copy constructor
    \param A :: MEBTQuad to copy
  */
{}

MEBTQuad&
MEBTQuad::operator=(const MEBTQuad& A)
  /*!
    Assignment operator
    \param A :: MEBTQuad to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      engActive=A.engActive;
      length=A.length;
      pipeRad=A.pipeRad;
      pipeMat=A.pipeMat;
      yokeMat=A.yokeMat;
      coilMat=A.coilMat;
      voidMat=A.voidMat;
      thickness=A.thickness;
    }
  return *this;
}

MEBTQuad*
MEBTQuad::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new MEBTQuad(*this);
}

MEBTQuad::~MEBTQuad()
  /*!
    Destructor
  */
{}

void
MEBTQuad::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("MEBTQuad","populate");

  FixedOffset::populate(Control);
  engActive=Control.EvalTriple<int>(keyName,baseName,"","EngineeringActive");

  length=Control.EvalTail<double>(keyName,extraName,"Length");
  voidMat=Control.EvalTail<int>(keyName,extraName,"VoidMat");
  thickness=Control.EvalTail<double>(keyName,extraName,"Thickness");
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
  yokeMat=ModelSupport::EvalMat<int>(Control,keyName+"YokeMat");
  coilMat=ModelSupport::EvalMat<int>(Control,keyName+"CoilMat");
  pipeRad=Control.EvalTail<double>(keyName,extraName,"PipeRad");

  //waterMat=ModelSupport::EvalMat<int>(Control,baseName+"WaterMat");

  return;
}

void
MEBTQuad::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("MEBTQuad","createSurfaces");

  // Dividers
  //ModelSupport::buildPlane(SMap,buildIndex+3,Origin,X);
  //ModelSupport::buildPlane(SMap,buildIndex+5,Origin,Z);

  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(length),Y);

  ModelSupport::buildPlane(SMap,buildIndex+90,Origin,X);
  ModelSupport::buildPlane(SMap,buildIndex+91,Origin,Z);

  //double pipeRad = 2.801; //otherwise particle will start on surface
  
  ModelSupport::buildCylinder(SMap,buildIndex+92,Origin,Y,pipeRad);
  ModelSupport::buildCylinder(SMap,buildIndex+93,Origin,Y,pipeRad+thickness);
 
  //Yoke                                                                        //--------------------------------------------------//
  //double x[22]={30.5,30.5   ,24.5   ,16.5122,16.5122,  0.0, 0.0, 8.0269,10.9456,2.2412,2.00,2.00,2.3763,3.9598,6.5985,7.35,8.25,9.15  ,21.5813,24.5  , 24.5,30.5};
  //double z[22]={0.  ,16.5122,16.5122,24.5   ,30.5   , 30.5,24.5,24.5   ,21.5813,9.15   ,8.25,7.35,6.5986,3.9598,2.3763,2.00,2.0,2.2412,10.9456,8.0269,  0.0,0.0};


  //Yoke MEBT                                                                        //--------------------------------------------------//
  double x[22]={22.967,22.967,8.8528,0,0,5.86899,7.70747,3.09006,0.961807,0.776121,0.776121,0.994829,1.45,2.10824,2.53031,2.82631,3.13984,10.1611,14.7785,16.617,16.617,22.967};
  double z[22]={0,8.8528,22.967,22.967,16.617,16.617,14.7785,10.1611,3.13984,2.82631,2.53031,2.10824,1.45,0.994829,0.77612,0.77612,0.961806,3.09006,7.70746,5.86899,0,0};

  Geometry::Plane* P[21];
  
  int SI(buildIndex);
  for(int i=0;i<21;i++){
    if(i==8 || i==9){ 
      P[i] = ModelSupport::buildPlane(SMap,SI+3,Origin+Y*(length*0.5)+Geometry::Vec3D(x[i],0,z[i]),Geometry::Vec3D(fabs(z[i+1]-z[i]),0,(x[i+1]-x[i])));
    } else if(i==17 || i==16){
      P[i] = ModelSupport::buildPlane(SMap,SI+3,Origin+Y*(length*0.5)+Geometry::Vec3D(x[i],0,z[i]),Geometry::Vec3D((z[i+1]-z[i]),0,-(x[i+1]-x[i])));
    } else{
      P[i] = ModelSupport::buildPlane(SMap,SI+3,Origin+Y*(length*0.5)+Geometry::Vec3D(x[i],0,z[i]),Geometry::Vec3D(fabs(z[i+1]-z[i]),0,fabs(x[i+1]-x[i])));
    }
    SI++;
  }

  double theta[3] = {-90.0,-180.0,-270.0};
  int counter = 103;
  for(int j=0;j<3;j++){
    SI = buildIndex;
    for(int i=0;i<21;i++){
      if(i==8 || i==9){ 
	ModelSupport::buildRotatedPlane(SMap,SI+counter,P[i],theta[j],Y,Geometry::Vec3D(0,0,0));
      } else if(i==17 || i==16){
	ModelSupport::buildRotatedPlane(SMap,SI+counter,P[i],theta[j],Y,Geometry::Vec3D(0,0,0));
      } else{
	ModelSupport::buildRotatedPlane(SMap,SI+counter,P[i],theta[j],Y,Geometry::Vec3D(0,0,0));
      }
      SI++;
    }
    counter = counter+100;
  }
  
  return;
}

void
MEBTQuad::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("MEBTQuad","createObjects");

  HeadRule HR;
  // Pipe
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 -92 "); 
  makeCell("Pipe",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 92 -93 "); 
  makeCell("Pipe",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 -93 "); 
  addOuterUnionSurf(HR);

  // Yokes
  
  int SI = buildIndex;
  for(int i=0;i<4;i++)
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,SI,"1 -2 23M -3M 22M -4M"); 
      makeCell("Yoke",System,cellIndex++,yokeMat,0.0,HR);
      addOuterUnionSurf(HR);

      HR=ModelSupport::getHeadRule(SMap,buildIndex,SI,"1 -2 -6M -7M 8M 9M");
      makeCell("Yoke",System,cellIndex++,yokeMat,0.0,HR);
      addOuterUnionSurf(HR);
      
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,SI,"1 -2 -22M -5M 8M 6M -4M");
      makeCell("Yoke",System,cellIndex++,yokeMat,0.0,HR);
      addOuterUnionSurf(HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,SI,"1 -2 -8M 6M 7M");
      makeCell("Yoke",System,cellIndex++,yokeMat,0.0,HR);
      addOuterUnionSurf(HR);

      
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,SI,
	 "1 -2 -10M -20M 11M -19M 12M 18M 13M 17M 14M 16M 15M");
      makeCell("Yoke",System,cellIndex++,yokeMat,0.0,HR);
      addOuterUnionSurf(HR);
      
      SI = SI +100;
    }
  return;
}


void
MEBTQuad::createLinks()

  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("MEBTQuad","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));

  return;
}




void
MEBTQuad::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("MEBTQuad","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
