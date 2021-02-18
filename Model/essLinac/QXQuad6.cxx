/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/QXQuad6.cxx
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
#include "BaseMap.h"
#include "CellMap.h"
#include "QXQuad6.h"

#include <iostream>

namespace essSystem
{

QXQuad6::QXQuad6(const std::string& Base,
		 const std::string& Key,const size_t Index) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Base+Key+std::to_string(Index),6),
  attachSystem::CellMap(),
  baseName(Base),
  extraName(Base+Key),voidMat(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

QXQuad6::QXQuad6(const QXQuad6& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  baseName(A.baseName),
  extraName(A.extraName),
  length(A.length),
  thickness(A.thickness),
  pipeRad(A.pipeRad),
  voidMat(A.voidMat),
  pipeMat(A.pipeMat),
  yokeMat(A.yokeMat),
  coilMat(A.coilMat)
  /*!
    Copy constructor
    \param A :: QXQuad6 to copy
  */
{}

QXQuad6&
QXQuad6::operator=(const QXQuad6& A)
  /*!
    Assignment operator
    \param A :: QXQuad6 to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      length=A.length;
      pipeMat=A.pipeMat;
      yokeMat=A.yokeMat;
      coilMat=A.coilMat;
      voidMat=A.voidMat;
      thickness=A.thickness;
      pipeRad=A.pipeRad;
    }
  return *this;
}

QXQuad6*
QXQuad6::clone() const
/*!
  Clone self
  \return new (this)
 */
{
  return new QXQuad6(*this);
}

QXQuad6::~QXQuad6()
  /*!
    Destructor
  */
{}

void
QXQuad6::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("QXQuad6","populate");

  FixedOffset::populate(Control);

  length=Control.EvalTail<double>(keyName,extraName,"Length");
  //  voidMat=Control.EvalTail<int>(keyName,extraName,"VoidMat");
  thickness=Control.EvalTail<double>(keyName,extraName,"Thickness");
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
  yokeMat=ModelSupport::EvalMat<int>(Control,keyName+"YokeMat");
  coilMat=ModelSupport::EvalMat<int>(Control,keyName+"CoilMat");
  pipeRad=Control.EvalTail<double>(keyName,extraName,"PipeRad");

  //waterMat=ModelSupport::EvalMat<int>(Control,baseName+"WaterMat");

  return;
}

void
QXQuad6::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("QXQuad6","createSurfaces");

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


  // Revised for Q6 geometry ---------- First two 29.55 -> 23.5 to match geometry model
  double x[22] ={32.55,32.55,23.5,10.6,10.6,0.0,0.0,6.19848,9.78078,2.24115,2.0,2.0,2.44591,3.9598,6.4107,7.35,8.25,9.15,19.9177,23.5,23.5,32.55};
  double z[22] ={0.0,10.6,10.6,23.5,32.55,32.55,23.5,23.5,19.9177,9.15,8.25,7.35,6.4107,3.9598,2.44591,2.0,2.0,2.24115,9.78078,6.19848,0.0,0.0};

  //Q6 geometry coils
  double x_A[5]={4.94393,0.595387,0.231638,1.60371,9.21013};
  double z_A[5]={22.9873,18.6526,13.5692,12.4284,20.0106};

  //Q6 Geometry coils
  double x_B[5]={20.0106,12.4284,13.5692,18.6526,22.9873};
  double z_B[5]={9.21013,1.60371,0.231638,0.595387,4.94393};

  //Q6 Geometry  added yoke, for the correction above to x,z
  double x_C[4] = {10.6,10.6,23.5,29.55};
  double z_C[4] = {29.55,23.5,10.6,10.6};


  
 
  //Yoke - From Fermilab model                                                                       //--------------------------------------------------//
  //double x[22]={30.5,30.5   ,24.5   ,16.5122,16.5122,  0.0, 0.0, 8.0269,10.9456,2.2412,2.00,2.00,2.3763,3.9598,6.5985,7.35,8.25,9.15  ,21.5813,24.5  , 24.5,30.5};
  //double z[22]={0.  ,16.5122,16.5122,24.5   ,30.5   , 30.5,24.5,24.5   ,21.5813,9.15   ,8.25,7.35,6.5986,3.9598,2.3763,2.00,2.0,2.2412,10.9456,8.0269,  0.0,0.0};

  
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

  // Coil A
  //Fermilab model
  //double x_A[5]={6.33156,0.131697,0.128245,1.53278,10.0692};
  //double z_A[5]={23.7463,14.97810,9.87756 ,8.93784,21.1291};



  Geometry::Plane* PA_1 = ModelSupport::buildPlane(SMap,buildIndex+24,Origin+Y*(length*0.5)+Geometry::Vec3D(x_A[0],0,z_A[0]),Geometry::Vec3D(fabs(z_A[1]-z_A[0]),0,(x_A[1]-x_A[0])));
  Geometry::Plane* PA_2 = ModelSupport::buildPlane(SMap,buildIndex+25,Origin+Y*(length*0.5)+Geometry::Vec3D(x_A[1],0,z_A[1]),Geometry::Vec3D(fabs(z_A[2]-z_A[1]),0,(x_A[2]-x_A[1])));
  Geometry::Plane* PA_3 = ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Y*(length*0.5)+Geometry::Vec3D(x_A[2],0,z_A[2]),Geometry::Vec3D(fabs(z_A[3]-z_A[2]),0,fabs(x_A[3]-x_A[2])));
  Geometry::Plane* PA_4 = ModelSupport::buildPlane(SMap,buildIndex+27,Origin+Y*(length*0.5)+Geometry::Vec3D(x_A[3],0,z_A[3]),Geometry::Vec3D((z_A[4]-z_A[3]),0,-(x_A[4]-x_A[3])));
  Geometry::Plane* PA_5 = ModelSupport::buildPlane(SMap,buildIndex+28,Origin+Y*(length*0.5)+Geometry::Vec3D(x_A[4],0,z_A[4]),Geometry::Vec3D(fabs(z_A[0]-z_A[4]),0,fabs(x_A[0]-x_A[4])));

 // Coil B
  //Fermilab model
  //double x_B[5]={21.1291,8.93784,9.87756, 14.9781,23.7463};
  //double z_B[5]={10.0692,1.53278,0.128245,0.131697,6.33156};

  Geometry::Plane* PB_1 = ModelSupport::buildPlane(SMap,buildIndex+29,Origin+Y*(length*0.5)+Geometry::Vec3D(x_B[0],0,z_B[0]),Geometry::Vec3D((z_B[1]-z_B[0]),0,-(x_B[1]-x_B[0])));
  Geometry::Plane* PB_2 = ModelSupport::buildPlane(SMap,buildIndex+30,Origin+Y*(length*0.5)+Geometry::Vec3D(x_B[1],0,z_B[1]),Geometry::Vec3D(fabs(z_B[2]-z_B[1]),0,fabs(x_B[2]-x_B[1])));
  Geometry::Plane* PB_3 = ModelSupport::buildPlane(SMap,buildIndex+31,Origin+Y*(length*0.5)+Geometry::Vec3D(x_B[2],0,z_B[2]),Geometry::Vec3D((z_B[3]-z_B[2]),0,-(x_B[3]-x_B[2])));
  Geometry::Plane* PB_4 = ModelSupport::buildPlane(SMap,buildIndex+32,Origin+Y*(length*0.5)+Geometry::Vec3D(x_B[3],0,z_B[3]),Geometry::Vec3D((z_B[4]-z_B[3]),0,-(x_B[4]-x_B[3])));
  Geometry::Plane* PB_5 = ModelSupport::buildPlane(SMap,buildIndex+33,Origin+Y*(length*0.5)+Geometry::Vec3D(x_B[4],0,z_B[4]),Geometry::Vec3D(fabs(z_B[0]-z_B[4]),0,fabs(x_B[0]-x_B[4])));


  //----------extra yoke pieces
  
  Geometry::Plane* PC_1 = ModelSupport::buildPlane(SMap,buildIndex+34,Origin+Y*(length*0.5)+Geometry::Vec3D(x_C[0],0,z_C[0]),Geometry::Vec3D((z_C[1]-z_C[0]),0,-(x_C[1]-x_C[0])));
  Geometry::Plane* PC_2 = ModelSupport::buildPlane(SMap,buildIndex+35,Origin+Y*(length*0.5)+Geometry::Vec3D(x_C[1],0,z_C[1]),Geometry::Vec3D(fabs(z_C[2]-z_C[1]),0,fabs(x_C[2]-x_C[1])));
  Geometry::Plane* PC_3 = ModelSupport::buildPlane(SMap,buildIndex+36,Origin+Y*(length*0.5)+Geometry::Vec3D(x_C[2],0,z_C[2]),Geometry::Vec3D((z_C[3]-z_C[2]),0,-(x_C[3]-x_C[2])));
  Geometry::Plane* PC_4 = ModelSupport::buildPlane(SMap,buildIndex+37,Origin+Y*(length*0.5)+Geometry::Vec3D(x_C[3],0,z_C[3]),Geometry::Vec3D(fabs(z_C[0]-z_C[3]),0,fabs(x_C[0]-x_C[3])));


  SI = buildIndex;
  for(int i=0;i<3;i++){
   
    ModelSupport::buildRotatedPlane(SMap,SI+924,PA_1,theta[i],Y,Geometry::Vec3D(0,0,0));
    ModelSupport::buildRotatedPlane(SMap,SI+925,PA_2,theta[i],Y,Geometry::Vec3D(0,0,0));
    ModelSupport::buildRotatedPlane(SMap,SI+926,PA_3,theta[i],Y,Geometry::Vec3D(0,0,0));
    ModelSupport::buildRotatedPlane(SMap,SI+927,PA_4,theta[i],Y,Geometry::Vec3D(0,0,0));
    ModelSupport::buildRotatedPlane(SMap,SI+928,PA_5,theta[i],Y,Geometry::Vec3D(0,0,0));
    
    ModelSupport::buildRotatedPlane(SMap,SI+929,PB_1,theta[i],Y,Geometry::Vec3D(0,0,0));
    ModelSupport::buildRotatedPlane(SMap,SI+930,PB_2,theta[i],Y,Geometry::Vec3D(0,0,0));
    ModelSupport::buildRotatedPlane(SMap,SI+931,PB_3,theta[i],Y,Geometry::Vec3D(0,0,0));
    ModelSupport::buildRotatedPlane(SMap,SI+932,PB_4,theta[i],Y,Geometry::Vec3D(0,0,0));
    ModelSupport::buildRotatedPlane(SMap,SI+933,PB_5,theta[i],Y,Geometry::Vec3D(0,0,0));

    ModelSupport::buildRotatedPlane(SMap,SI+934,PC_1,theta[i],Y,Geometry::Vec3D(0,0,0));
    ModelSupport::buildRotatedPlane(SMap,SI+935,PC_2,theta[i],Y,Geometry::Vec3D(0,0,0));
    ModelSupport::buildRotatedPlane(SMap,SI+936,PC_3,theta[i],Y,Geometry::Vec3D(0,0,0));
    ModelSupport::buildRotatedPlane(SMap,SI+937,PC_4,theta[i],Y,Geometry::Vec3D(0,0,0));
    
    SI= SI+100;
  }

    
  return;
}

void
QXQuad6::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("QXQuad6","createObjects");

  HeadRule HR;

  // Pipe
  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 -92 "); 
  makeCell("Pipe",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," 1 -2 92 -93"); 
  makeCell("Pipe",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"1 -2 -93"); 
  addOuterSurf(HR);


  // Yokes
  
  int SI(buildIndex);
  for(int i=0;i<4;i++)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,SI,"1 -2 23M -3M 22M -4M"); 
      makeCell("Yoke",System,cellIndex++,yokeMat,0.0,HR);
      addOuterUnionSurf(HR);
      
      HR=ModelSupport::getHeadRule(SMap,buildIndex,SI,"1 -2 -6M -7M 8M 9M");
      makeCell("Yoke",System,cellIndex++,yokeMat,0.0,HR);
      addOuterUnionSurf(HR);
      
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,SI,"1 -2 -22M -5M -9M 10M");
      makeCell("Yoke",System,cellIndex++,yokeMat,0.0,HR);
      addOuterUnionSurf(HR);
      
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,SI,
	 "1 -2 -10M -20M 11M -19M 12M 18M 13M 17M 14M 16M 15M");
      makeCell("Yoke",System,cellIndex++,yokeMat,0.0,HR);
      addOuterUnionSurf(HR);

    SI = SI +100;
  }

  
  int offset[4] = {0,900,1000,1100};
  
  for(int i=0;i<4;i++)
    {
      SI = buildIndex +offset[i];
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,SI,"1 -2 24M 25M 26M -27M -28M");
    makeCell("Coil",System,cellIndex++,coilMat,0.0,HR);
    addOuterUnionSurf(HR);
    
    HR=ModelSupport::getHeadRule
      (SMap,buildIndex,SI,"1 -2 -29M 30M -31M -32M -33M");
    makeCell("Coil",System,cellIndex++,coilMat,0.0,HR);
    addOuterUnionSurf(HR);

    if(i==0)
      {
	HR=ModelSupport::getHeadRule
	  (SMap,buildIndex,SI,"1 -2 -34M 35M -36M -37M");
	makeCell("Coil",System,cellIndex++,yokeMat,0.0,HR);
	addOuterUnionSurf(HR);
      }
    else
      {
	HR=ModelSupport::getHeadRule
	  (SMap,buildIndex,SI,"1 -2 34M 35M 36M -37M");
	makeCell("Coil",System,cellIndex++,yokeMat,0.0,HR);
	addOuterUnionSurf(HR);
      }
    }

  return;
}


void
QXQuad6::createLinks()

  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("QXQuad6","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));

  return;
}




void
QXQuad6::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("QXQuad6","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
