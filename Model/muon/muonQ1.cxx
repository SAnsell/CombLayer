/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   muon/muonQ1.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell/Goran Skoro
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
#include <iterator>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
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
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "muonQ1.h"

namespace muSystem
{

muonQ1::muonQ1(const std::string& Key)  : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

muonQ1::muonQ1(const muonQ1& A) : 
  attachSystem::FixedRotate(A),attachSystem::ContainedComp(A),
  xSize(A.xSize),ySize(A.ySize),zSize(A.zSize),
  cutLenOut(A.cutLenOut),cutLenMid(A.cutLenMid),
  steelThick(A.steelThick),copperThick(A.copperThick),
  copperYSize(A.copperYSize),insertSize(A.insertSize),
  insertThick(A.insertThick),steelMat(A.steelMat),
  copperMat(A.copperMat),insertMat(A.insertMat)
  /*!
    Copy constructor
    \param A :: muonQ1 to copy
  */
{}

muonQ1&
muonQ1::operator=(const muonQ1& A)
  /*!
    Assignment operator
    \param A :: muonQ1 to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      xSize=A.xSize;
      ySize=A.ySize;
      zSize=A.zSize;
      cutLenOut=A.cutLenOut;
      cutLenMid=A.cutLenMid;
      steelThick=A.steelThick;
      copperThick=A.copperThick;
      copperYSize=A.copperYSize;
      insertSize=A.insertSize;
      insertThick=A.insertThick;
      steelMat=A.steelMat;
      copperMat=A.copperMat;
      insertMat=A.insertMat;
    }
  return *this;
}


muonQ1::~muonQ1() 
  /*!
    Destructor
  */
{}

void
muonQ1::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("muonQ1","populate");

  FixedRotate::populate(Control);
  
  xSize=Control.EvalVar<double>(keyName+"XSize");
  ySize=Control.EvalVar<double>(keyName+"YSize");
  zSize=Control.EvalVar<double>(keyName+"ZSize");
  cutLenOut=Control.EvalVar<double>(keyName+"CutLenOut");
  cutLenMid=Control.EvalVar<double>(keyName+"CutLenMid");

  steelThick=Control.EvalVar<double>(keyName+"SteelThick");
  copperThick=Control.EvalVar<double>(keyName+"CopperThick");
  copperYSize=Control.EvalVar<double>(keyName+"CopperYSize");    

  insertSize=Control.EvalVar<double>(keyName+"InsertSize");    
  insertThick=Control.EvalVar<double>(keyName+"InsertThick");    
      
  steelMat=ModelSupport::EvalMat<int>(Control,keyName+"SteelMat");    
  copperMat=ModelSupport::EvalMat<int>(Control,keyName+"CopperMat");
  insertMat=ModelSupport::EvalMat<int>(Control,keyName+"InsertMat");
         
  return;
}


void
muonQ1::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("muonQ1","createSurface");

  // Outer layer:
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*ySize/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*ySize/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,Origin-X*xSize/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,Origin+X*xSize/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*zSize/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*zSize/2.0,Z);
         // Corners:
  ModelSupport::buildPlane(SMap,buildIndex+101,Origin
			   -X*xSize/2.0-Z*(zSize/2.0-cutLenOut),-X-Z);
  ModelSupport::buildPlane(SMap,buildIndex+102,Origin
			   -X*xSize/2.0+Z*(zSize/2.0-cutLenOut),-X+Z);
  ModelSupport::buildPlane(SMap,buildIndex+103,Origin
			   +X*xSize/2.0-Z*(zSize/2.0-cutLenOut),X-Z);
  ModelSupport::buildPlane(SMap,buildIndex+104,Origin
			   +X*xSize/2.0+Z*(zSize/2.0-cutLenOut),X+Z);

  // Steel layer:
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*copperYSize/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*copperYSize/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,Origin-X*(xSize/2.0-steelThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,Origin+X*(xSize/2.0-steelThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,Origin-Z*(zSize/2.0-steelThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,Origin+Z*(zSize/2.0-steelThick),Z);
         // Corners:
  ModelSupport::buildPlane(SMap,buildIndex+111,Origin
			   -X*(xSize/2.0-steelThick)-Z*(zSize/2.0-steelThick-cutLenMid),-X-Z);
  ModelSupport::buildPlane(SMap,buildIndex+112,Origin
			   -X*(xSize/2.0-steelThick)+Z*(zSize/2.0-steelThick-cutLenMid),-X+Z);
  ModelSupport::buildPlane(SMap,buildIndex+113,Origin
			   +X*(xSize/2.0-steelThick)-Z*(zSize/2.0-steelThick-cutLenMid),X-Z);
  ModelSupport::buildPlane(SMap,buildIndex+114,Origin
			   +X*(xSize/2.0-steelThick)+Z*(zSize/2.0-steelThick-cutLenMid),X+Z);

  // Coper layer:
//  ModelSupport::buildPlane(SMap,buildIndex+21,Origin-Y*copperYSize/2.0,Y);
//  ModelSupport::buildPlane(SMap,buildIndex+22,Origin+Y*copperYSize/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+23,Origin-X*(xSize/2.0-steelThick-copperThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+24,Origin+X*(xSize/2.0-steelThick-copperThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+25,Origin-Z*(zSize/2.0-steelThick-copperThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+26,Origin+Z*(zSize/2.0-steelThick-copperThick),Z);

  // Inserts - top & bottom:
  ModelSupport::buildPlane(SMap,buildIndex+31,Origin-Y*insertSize/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+32,Origin+Y*insertSize/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+33,Origin-X*insertSize/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+34,Origin+X*insertSize/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+35,Origin-Z*(zSize/2.0-steelThick-copperThick-insertThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,Origin+Z*(zSize/2.0-steelThick-copperThick-insertThick),Z);
  
   // Inserts - left & right:
//  ModelSupport::buildPlane(SMap,buildIndex+31,Origin-Y*insertSize/2.0,Y);
//  ModelSupport::buildPlane(SMap,buildIndex+32,Origin+Y*insertSize/2.0,Y);
  ModelSupport::buildPlane(SMap,buildIndex+43,Origin-X*(xSize/2.0-steelThick-copperThick-insertThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+44,Origin+X*(xSize/2.0-steelThick-copperThick-insertThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+45,Origin-Z*insertSize/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+46,Origin+Z*insertSize/2.0,Z); 
  
  return;
}

void
muonQ1::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("muonQ1","createObjects");
  
  std::string Out;
  std::string Out1;
  std::string Out2;
  std::string Out3;
  std::string Out4;
  std::string Out5;
    
    // Steel
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "1 -2 3 -4 5 -6 -101 -102 -103 -104 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);
  Out1=ModelSupport::getComposite(SMap,buildIndex,
				 "(-13:14:-15:16:111:112:113:114)");  
  System.addCell(MonteCarlo::Object(cellIndex++,steelMat,0.0,Out+Out1));  

    // Copper
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "11 -12 13 -14 15 -16 -111 -112 -113 -114 ");
  addOuterUnionSurf(Out);
  addBoundaryUnionSurf(Out);
  Out1=ModelSupport::getComposite(SMap,buildIndex,
				 "(-23:24:-25:26)");
  Out2=ModelSupport::getComposite(SMap,buildIndex,
				 "(-31:32:-33:34:-36:16)");  	
  Out3=ModelSupport::getComposite(SMap,buildIndex,
				 "(-31:32:-33:34:35:-15)");
  Out4=ModelSupport::getComposite(SMap,buildIndex,
				 "(-31:32:-13:43:-45:46)"); 
  Out5=ModelSupport::getComposite(SMap,buildIndex,
				 "(-31:32:-44:14:-45:46)");
  
  // This is junk!!
  System.addCell(MonteCarlo::Object(cellIndex++,copperMat,
				   0.0,Out+Out1+Out2+Out3+Out4+Out5));  


    // Inserts
  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "31 -32 33 -34 36 -16 ");    
  System.addCell(MonteCarlo::Object(cellIndex++,insertMat,0.0,Out));    

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "31 -32 33 -34 -35 15 ");    
  System.addCell(MonteCarlo::Object(cellIndex++,insertMat,0.0,Out));    

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "31 -32 13 -43 45 -46 ");    
  System.addCell(MonteCarlo::Object(cellIndex++,insertMat,0.0,Out));    

  Out=ModelSupport::getComposite(SMap,buildIndex,
				 "31 -32 44 -14 45 -46 ");    
  System.addCell(MonteCarlo::Object(cellIndex++,insertMat,0.0,Out));    

    // Void
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 23 -24 25 -26 ");
  
  // This is junk!!
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,
				   Out+Out2+Out3+Out4+Out5));    
  return;
}


void
muonQ1::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("muonQ1","createLinks");

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(buildIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(buildIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+6));

  FixedComp::setConnect(0,Origin-Y*ySize/2.0,-Y);
  FixedComp::setConnect(1,Origin+Y*ySize/2.0,Y);
  FixedComp::setConnect(2,Origin-X*xSize/2.0,-X);
  FixedComp::setConnect(3,Origin+X*xSize/2.0,X);
  FixedComp::setConnect(4,Origin-Z*zSize/2.0,-Z);
  FixedComp::setConnect(5,Origin+Z*zSize/2.0,Z);

  return;
}

void
muonQ1::createAll(Simulation& System,
		  const attachSystem::FixedComp& FC,
		  const long int sideIndex)

  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("muonQ1","createAll");
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
