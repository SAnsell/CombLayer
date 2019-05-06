/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuild/QXQuad.cxx
 *
 * Copyright (c) 2017-2018 by Konstantin Batkov
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
#include "stringCombine.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
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
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "FixedOffset.h"
#include "surfDBase.h"
#include "surfDIter.h"
#include "surfDivide.h"
#include "SurInter.h"
#include "mergeTemplate.h"
#include "CellMap.h"
#include "QXQuad.h"

#include <iostream>

namespace essSystem
{

  QXQuad::QXQuad(const std::string& Base,const std::string& Key,const size_t Index) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Base+Key+StrFunc::makeString(Index),6),
  attachSystem::CellMap(),
  baseName(Base),
  extraName(Base+Key),
  surfIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(surfIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

QXQuad::QXQuad(const QXQuad& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedOffset(A),attachSystem::CellMap(A),
  baseName(A.baseName),
  extraName(A.extraName),
  surfIndex(A.surfIndex),cellIndex(A.cellIndex),
  engActive(A.engActive),
  length(A.length),
  pipeMat(A.pipeMat),
  yokeMat(A.yokeMat),
  voidMat(A.voidMat),
  thickness(A.thickness)

  /*!
    Copy constructor
    \param A :: QXQuad to copy
  */
{}

QXQuad&
QXQuad::operator=(const QXQuad& A)
  /*!
    Assignment operator
    \param A :: QXQuad to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      engActive=A.engActive;
      length=A.length;
      pipeMat=A.pipeMat;
      yokeMat=A.yokeMat;
      coilMat=A.coilMat;
      voidMat=A.voidMat;
      thickness=A.thickness;
    }
  return *this;
}

QXQuad*
QXQuad::clone() const
/*!
  Clone self
  \return new (this)
 */
{
    return new QXQuad(*this);
}

QXQuad::~QXQuad()
  /*!
    Destructor
  */
{}

void
QXQuad::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("QXQuad","populate");

  FixedOffset::populate(Control);
  engActive=Control.EvalTriple<int>(keyName,baseName,"","EngineeringActive");

  length=Control.EvalPair<double>(keyName,extraName,"Length");
  voidMat=Control.EvalPair<int>(keyName,extraName,"VoidMat");
  thickness=Control.EvalPair<double>(keyName,extraName,"Thickness");
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
  yokeMat=ModelSupport::EvalMat<int>(Control,keyName+"YokeMat");
  coilMat=ModelSupport::EvalMat<int>(Control,keyName+"CoilMat");

  //waterMat=ModelSupport::EvalMat<int>(Control,baseName+"WaterMat");

  return;
}

void
QXQuad::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: object for origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("QXQuad","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}

void
QXQuad::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("QXQuad","createSurfaces");

  // Dividers
  //ModelSupport::buildPlane(SMap,surfIndex+3,Origin,X);
  //ModelSupport::buildPlane(SMap,surfIndex+5,Origin,Z);

  ModelSupport::buildPlane(SMap,surfIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,surfIndex+2,Origin+Y*(length),Y);

  ModelSupport::buildPlane(SMap,surfIndex+90,Origin,X);
  ModelSupport::buildPlane(SMap,surfIndex+91,Origin,Z);

  double pipeRad = 2.801; //otherwise particle will start on surface
  
  ModelSupport::buildCylinder(SMap,surfIndex+92,Origin,Y,pipeRad);
  ModelSupport::buildCylinder(SMap,surfIndex+93,Origin,Y,pipeRad+thickness);
 
  //Yoke                                                                        //--------------------------------------------------//
  double x[22]={30.5,30.5   ,24.5   ,16.5122,16.5122,  0.0, 0.0, 8.0269,10.9456,2.2412,2.00,2.00,2.3763,3.9598,6.5985,7.35,8.25,9.15  ,21.5813,24.5  , 24.5,30.5};
  double z[22]={0.  ,16.5122,16.5122,24.5   ,30.5   , 30.5,24.5,24.5   ,21.5813,9.15   ,8.25,7.35,6.5986,3.9598,2.3763,2.00,2.0,2.2412,10.9456,8.0269,  0.0,0.0};

  Geometry::Plane* P[21];
  
  int SI(surfIndex);
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
    SI = surfIndex;
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

  double x_A[5]={6.33156,0.131697,0.128245,1.53278,10.0692};
  double z_A[5]={23.7463,14.97810,9.87756 ,8.93784,21.1291};

  Geometry::Plane* PA_1 = ModelSupport::buildPlane(SMap,surfIndex+24,Origin+Y*(length*0.5)+Geometry::Vec3D(x_A[0],0,z_A[0]),Geometry::Vec3D(fabs(z_A[1]-z_A[0]),0,(x_A[1]-x_A[0])));
  Geometry::Plane* PA_2 = ModelSupport::buildPlane(SMap,surfIndex+25,Origin+Y*(length*0.5)+Geometry::Vec3D(x_A[1],0,z_A[1]),Geometry::Vec3D(fabs(z_A[2]-z_A[1]),0,(x_A[2]-x_A[1])));
  Geometry::Plane* PA_3 = ModelSupport::buildPlane(SMap,surfIndex+26,Origin+Y*(length*0.5)+Geometry::Vec3D(x_A[2],0,z_A[2]),Geometry::Vec3D(fabs(z_A[3]-z_A[2]),0,fabs(x_A[3]-x_A[2])));
  Geometry::Plane* PA_4 = ModelSupport::buildPlane(SMap,surfIndex+27,Origin+Y*(length*0.5)+Geometry::Vec3D(x_A[3],0,z_A[3]),Geometry::Vec3D((z_A[4]-z_A[3]),0,-(x_A[4]-x_A[3])));
  Geometry::Plane* PA_5 = ModelSupport::buildPlane(SMap,surfIndex+28,Origin+Y*(length*0.5)+Geometry::Vec3D(x_A[4],0,z_A[4]),Geometry::Vec3D(fabs(z_A[0]-z_A[4]),0,fabs(x_A[0]-x_A[4])));

 // Coil B

  double x_B[5]={21.1291,8.93784,9.87756, 14.9781,23.7463};
  double z_B[5]={10.0692,1.53278,0.128245,0.131697,6.33156};

  Geometry::Plane* PB_1 = ModelSupport::buildPlane(SMap,surfIndex+29,Origin+Y*(length*0.5)+Geometry::Vec3D(x_B[0],0,z_B[0]),Geometry::Vec3D((z_B[1]-z_B[0]),0,-(x_B[1]-x_B[0])));
  Geometry::Plane* PB_2 = ModelSupport::buildPlane(SMap,surfIndex+30,Origin+Y*(length*0.5)+Geometry::Vec3D(x_B[1],0,z_B[1]),Geometry::Vec3D(fabs(z_B[2]-z_B[1]),0,fabs(x_B[2]-x_B[1])));
  Geometry::Plane* PB_3 = ModelSupport::buildPlane(SMap,surfIndex+31,Origin+Y*(length*0.5)+Geometry::Vec3D(x_B[2],0,z_B[2]),Geometry::Vec3D((z_B[3]-z_B[2]),0,-(x_B[3]-x_B[2])));
  Geometry::Plane* PB_4 = ModelSupport::buildPlane(SMap,surfIndex+32,Origin+Y*(length*0.5)+Geometry::Vec3D(x_B[3],0,z_B[3]),Geometry::Vec3D((z_B[4]-z_B[3]),0,-(x_B[4]-x_B[3])));
  Geometry::Plane* PB_5 = ModelSupport::buildPlane(SMap,surfIndex+33,Origin+Y*(length*0.5)+Geometry::Vec3D(x_B[4],0,z_B[4]),Geometry::Vec3D(fabs(z_B[0]-z_B[4]),0,fabs(x_B[0]-x_B[4])));


  SI = surfIndex;
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

    SI= SI+100;
  }

  
  return;
}

void
QXQuad::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("QXQuad","createObjects");

  std::string Out,Side;

  int SI(surfIndex);

  // Pipe
  Out=ModelSupport::getComposite(SMap,surfIndex,SI," 1 -2 -92 "); 
  System.addCell(MonteCarlo::Object(cellIndex++,voidMat,0.0,Out));
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,surfIndex,SI," 1 -2 92 -93 "); 
  System.addCell(MonteCarlo::Object(cellIndex++,pipeMat,0.0,Out));
  addOuterUnionSurf(Out);


  // Yokes
  
  SI = surfIndex;
  for(int i=0;i<4;i++){
    Out=ModelSupport::getComposite(SMap,surfIndex,SI," 1 -2 23M -3M 22M -4M "); 
    System.addCell(MonteCarlo::Object(cellIndex++,yokeMat,0.0,Out));
    addOuterUnionSurf(Out);
    
    Out=ModelSupport::getComposite(SMap,surfIndex,SI," 1 -2 -6M -7M 8M 9M");
    System.addCell(MonteCarlo::Object(cellIndex++,yokeMat,0.0,Out));
    addOuterUnionSurf(Out);
    
    Out=ModelSupport::getComposite(SMap,surfIndex,SI," 1 -2 -22M -5M -9M 10M");
    System.addCell(MonteCarlo::Object(cellIndex++,yokeMat,0.0,Out));
    addOuterUnionSurf(Out);
    
    Out=ModelSupport::getComposite(SMap,surfIndex,SI," 1 -2 -10M -20M 11M -19M 12M 18M 13M 17M 14M 16M 15M");
    System.addCell(MonteCarlo::Object(cellIndex++,yokeMat,0.0,Out));
    addOuterUnionSurf(Out);

    Out=ModelSupport::getComposite(SMap,surfIndex,SI," 1 -2 4M 5M 6M -7M -3M"); 
    System.addCell(MonteCarlo::Object(cellIndex++,60,0.0,Out));
    addOuterUnionSurf(Out);

    SI = SI +100;
  }

  
  int offset[4] = {0,900,1000,1100};
  
  for(int i=0;i<4;i++){
    SI = surfIndex +offset[i];
    Out=ModelSupport::getComposite(SMap,surfIndex,SI," 1 -2 24M 25M 26M -27M -28M");
    System.addCell(MonteCarlo::Object(cellIndex++,coilMat,0.0,Out));
    addOuterUnionSurf(Out);

    Out=ModelSupport::getComposite(SMap,surfIndex,SI," 1 -2 -29M 30M -31M -32M -33M ");
    System.addCell(MonteCarlo::Object(cellIndex++,coilMat,0.0,Out));
    addOuterUnionSurf(Out);
  
  }
  

  return;
}


void
QXQuad::createLinks()

  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("QXQuad","createLinks");

  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setLinkSurf(0,-SMap.realSurf(surfIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(surfIndex+12));


  // for (int i=6; i<8; i++)
  //   ELog::EM << keyName << " " << i << "\t" << getLinkSurf(i) << "\t" << getLinkPt(i) << "\t\t" << getLinkAxis(i) << ELog::endDiag;

  return;
}




void
QXQuad::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("QXQuad","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // essSystem
