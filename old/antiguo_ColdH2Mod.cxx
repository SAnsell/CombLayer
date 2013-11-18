/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/antiguo_ColdH2Mod.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"

//#include "WaterMod.h"
#include "ColdH2Mod.h"


namespace bibSystem
{

ColdH2Mod::ColdH2Mod(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,6), /** Aqui le decimos cuantos kinking points queremos*/
  coldH2Index(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(coldH2Index+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

ColdH2Mod::ColdH2Mod(const ColdH2Mod& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  coldH2Index(A.coldH2Index),cellIndex(A.cellIndex),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),
  zAngle(A.zAngle),width(A.width),height(A.height),
  depth(A.depth),wallThick(A.wallThick),waterMat(A.waterMat),
  wallMat(A.wallMat)
  /*!
    Copy constructor
    \param A :: ColdH2Mod to copy
  */
{}

ColdH2Mod&
ColdH2Mod::operator=(const ColdH2Mod& A)
  /*!
    Assignment operator
    \param A :: ColdH2Mod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      width=A.width;
      height=A.height;
      depth=A.depth;
      wallThick=A.wallThick;
      waterMat=A.waterMat;
      wallMat=A.wallMat;
    }
  return *this;
}


ColdH2Mod::~ColdH2Mod()
  /*!
    Destructor
  */
{}

void
ColdH2Mod::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
  
  /** DEFINIMOS LAS VARIABLES DEL OBJETO*/
{
  ELog::RegMethod RegA("ColdH2Mod","populate");

  /** Variables para posicionar el origen y la orientacion del sistema local*/
  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  /** Variables especificas del objeto (moderador en este caso)*/
  
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  sideGap=Control.EvalVar<double>(keyName+"SideGap");
  frontGap=Control.EvalVar<double>(keyName+"FrontGap");
  backGap=Control.EvalVar<double>(keyName+"BackGap");
  vertGap=Control.EvalVar<double>(keyName+"VertGap");

  wallThick=Control.EvalVar<double>(keyName+"WallThick");

  wallMat=Control.EvalVar<int>(keyName+"WallMat");  
  waterMat=Control.EvalVar<int>(keyName+"WaterMat");  

  return;
}


void
ColdH2Mod::createUnitVector(const attachSystem::FixedComp& FC,
			   const size_t sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
  
  /** VECTOR UNITARIO DE ORIENTACION DEL OBJETO*/
{
  ELog::RegMethod RegA("ColdH2Mod","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
  Origin=FC.getLinkPt(sideIndex);

  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}

void
ColdH2Mod::createSurfaces()
  /*!
    Create planes for the silicon and Polyethene layers
  */
  
    /** RELACIONES ENTRE LAS SUPERFICIES
     El valor de las variables se define en el archivo de variables, y son 
     modificables a mano sin tener que compilar. Esto facilita la parametrizacion 
     y optimizacion neutronica del modelo*/
{
  ELog::RegMethod RegA("ColdH2Mod","createSurfaces");

  ModelSupport::buildPlane(SMap,coldH2Index+1,Origin-Y*depth/2.0,Y);  
  ModelSupport::buildPlane(SMap,coldH2Index+2,Origin+Y*depth/2.0,Y);  
  ModelSupport::buildPlane(SMap,coldH2Index+3,Origin-X*width/2.0,X);  
  ModelSupport::buildPlane(SMap,coldH2Index+4,Origin+X*width/2.0,X);  
  ModelSupport::buildPlane(SMap,coldH2Index+5,Origin-Z*height/2.0,Z);  
  ModelSupport::buildPlane(SMap,coldH2Index+6,Origin+Z*height/2.0,Z);  

  ModelSupport::buildPlane(SMap,coldH2Index+11,Origin-Y*(depth/2.0+wallThick),Y);  
  ModelSupport::buildPlane(SMap,coldH2Index+12,Origin+Y*(depth/2.0+wallThick),Y);  
  ModelSupport::buildPlane(SMap,coldH2Index+13,Origin-X*(width/2.0+wallThick),X);  
  ModelSupport::buildPlane(SMap,coldH2Index+14,Origin+X*(width/2.0+wallThick),X);  
  ModelSupport::buildPlane(SMap,coldH2Index+15,Origin-Z*(height/2.0+wallThick),Z); 
  ModelSupport::buildPlane(SMap,coldH2Index+16,Origin+Z*(height/2.0+wallThick),Z); 

  ModelSupport::buildPlane(SMap,coldH2Index+21,
			   Origin-Y*(depth/2.0+wallThick+frontGap),Y);  
  ModelSupport::buildPlane(SMap,coldH2Index+22,Origin+
			   Y*(depth/2.0+wallThick+backGap),Y);  
  ModelSupport::buildPlane(SMap,coldH2Index+23,Origin-
			   X*(width/2.0+wallThick+sideGap),X);  
  ModelSupport::buildPlane(SMap,coldH2Index+24,Origin+
			   X*(width/2.0+wallThick+sideGap),X);  
  ModelSupport::buildPlane(SMap,coldH2Index+25,Origin-
			   Z*(height/2.0+wallThick+vertGap),Z); 
  ModelSupport::buildPlane(SMap,coldH2Index+26,Origin+
			   Z*(height/2.0+wallThick+vertGap),Z); 

  return; 
}

void
ColdH2Mod::createObjects(Simulation& System,
			const attachSystem::ContainedComp& CC)
  /*!
    Create the simple moderator
    \param System :: Simulation to add results
   */
  
    /** RELACIONES DE SUPERFICIES PARA CONSTRUIR EL OBJETO. En coldH2Index
     se van almacenando las posiciones para el mapa de superficies global. */
{
  ELog::RegMethod RegA("ColdH2Mod","createObjects");
  
  std::string Out;

  // Water
  Out=ModelSupport::getComposite(SMap,coldH2Index,"1 -2 3 -4 5 -6");	
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,waterTemp,Out));

  // Wall of water moderator
  Out=ModelSupport::getComposite(SMap,coldH2Index,
				 "11 -12 13 -14 15 -16 (-1:2:-3:4:-5:6)");	
  System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat,waterTemp,Out));

  // Wall of water moderator
  Out=ModelSupport::getComposite(SMap,coldH2Index,
	     	 "21 -22 23 -24 25 -26 (-11:12:-13:14:-15:16)");
  Out+=CC.getExclude();
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

  Out=ModelSupport::getComposite(SMap,coldH2Index,"21 -22 23 -24 25 -26" );
  addOuterSurf(Out);

  return; 
}

void
ColdH2Mod::createLinks()
  /*!
    Creates a full attachment set
    Surfaces pointing outwards
  */
  
  /** PUNTOS DE VINCULO CON OTRAS SUPERFICIES.
   Hay que escribir una linea por cada uno de los puntos que definamos. 
   Aqui hay 6. En alguna otra parte hay que acordarse de poner 6*/
{
  ELog::RegMethod RegA("ColdH2Mod","createLinks");
  // set Links :: Inner links:
  // Wrapper layer
  // Index : Point :: Normal
  FixedComp::setConnect(0,Origin-Y*(depth/2.0+wallThick+frontGap),-Y);  
  FixedComp::setConnect(1,Origin+Y*(depth/2.0+wallThick+backGap),Y);  
  FixedComp::setConnect(2,Origin-X*(width/2.0+wallThick+sideGap),-X);  
  FixedComp::setConnect(3,Origin+X*(width/2.0+wallThick+sideGap),X);  
  FixedComp::setConnect(4,Origin-Z*(height/2.0+wallThick+vertGap),-Z);  
  FixedComp::setConnect(5,Origin+Y*(height/2.0+wallThick+vertGap),Z);  

  // WHY HAVENT I WRITTEN AutoLinkSurf()
  FixedComp::setLinkSurf(0,-SMap.realSurf(coldH2Index+21)); // Esta en negativo porque refiere la superficie 21 en su lado negativo. Deduzco que llevan orientacion...
  FixedComp::setLinkSurf(1,SMap.realSurf(coldH2Index+22));
  FixedComp::setLinkSurf(2,-SMap.realSurf(coldH2Index+23));
  FixedComp::setLinkSurf(3,SMap.realSurf(coldH2Index+24));
  FixedComp::setLinkSurf(4,-SMap.realSurf(coldH2Index+25));
  FixedComp::setLinkSurf(5,SMap.realSurf(coldH2Index+26));
  
  return;
}

void
ColdH2Mod::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC,
		    const size_t sideIndex,
		    const attachSystem::ContainedComp& CC)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: Side index
   */
{
  ELog::RegMethod RegA("ColdH2Mod","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System,CC);

  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE bibSystem
