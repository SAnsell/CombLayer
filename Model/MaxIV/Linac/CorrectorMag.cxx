/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   Linac/CorrectorMag.cxx
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
#include "surfDIter.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "SurInter.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "SimProcess.h"
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
#include "ExternalCut.h" 
#include "BaseMap.h"
#include "SurfMap.h"
#include "CellMap.h" 

#include "CorrectorMag.h"

namespace tdcSystem
{

CorrectorMag::CorrectorMag(const std::string& Key) :
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  baseName(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: KeyName
  */
{}

CorrectorMag::CorrectorMag(const std::string& Base,
		   const std::string& Key) : 
  attachSystem::FixedRotate(Key,6),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  baseName(Base)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Base :: Base KeyName
    \param Key :: KeyName
  */
{}


CorrectorMag::~CorrectorMag() 
  /*!
    Destructor
  */
{}

void
CorrectorMag::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: DataBase for variables
  */
{
  ELog::RegMethod RegA("CorrectorMag","populate");

  FixedRotate::populate(Control);

  magOffset=Control.EvalVar<double>(keyName+"MagOffset");
  magHeight=Control.EvalVar<double>(keyName+"MagHeight");
  magWidth=Control.EvalVar<double>(keyName+"MagWidth");
  magLength=Control.EvalVar<double>(keyName+"MagLength");
  magCorner=Control.EvalVar<double>(keyName+"MagCorner");
  magInnerWidth=Control.EvalVar<double>(keyName+"MagInnerWidth");
  magInnerLength=Control.EvalVar<double>(keyName+"MagInnerLength");

  pipeClampYStep=Control.EvalVar<double>(keyName+"PipeClampYStep");
  pipeClampZStep=Control.EvalVar<double>(keyName+"PipeClampZStep");
  pipeClampThick=Control.EvalVar<double>(keyName+"PipeClampThick");
  pipeClampWidth=Control.EvalVar<double>(keyName+"PipeClampWidth");
  pipeClampHeight=Control.EvalVar<double>(keyName+"PipeClampHeight");

  frameHeight=Control.EvalVar<double>(keyName+"FrameHeight");

  voidMat=ModelSupport::EvalDefMat<int>(Control,keyName+"VoidMat",0);
  coilMat=ModelSupport::EvalMat<int>(Control,keyName+"CoilMat");
  clampMat=ModelSupport::EvalMat<int>(Control,keyName+"ClampMat");
  frameMat=ModelSupport::EvalMat<int>(Control,keyName+"FrameMat");

  return;
}


void
CorrectorMag::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CorrectorMag","createSurface");

  // left at 1000 / right at 2000
 
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin-Y*(magLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*(magLength/2.0),Y);

  ModelSupport::buildPlane(SMap,buildIndex+5,Origin-Z*(magHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,Origin+Z*(magHeight/2.0),Z);

   
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin-Y*(magInnerLength/2.0),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,Origin+Y*(magInnerLength/2.0),Y);

     
  // Left/Right magnet surfaces:
  const Geometry::Vec3D LOrg(Origin-X*magOffset);
  ModelSupport::buildPlane(SMap,buildIndex+1003,
			   LOrg-X*(magWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+1004,
			   LOrg+X*(magWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+1013,
			   LOrg-X*(magInnerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+1014,
			   LOrg+X*(magInnerWidth/2.0),X);

  const Geometry::Vec3D ROrg(Origin+X*magOffset);
  ModelSupport::buildPlane(SMap,buildIndex+2003,
			   ROrg-X*(magWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+2004,
			   ROrg+X*(magWidth/2.0),X);

  ModelSupport::buildPlane(SMap,buildIndex+2013,
			   ROrg-X*(magInnerWidth/2.0),X);
  ModelSupport::buildPlane(SMap,buildIndex+2014,
			   ROrg+X*(magInnerWidth/2.0),X);

  // frame stuff

  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(frameHeight+magHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(frameHeight+magHeight/2.0),Z);


  
  // calc corner:
  double XScale(magCorner-magWidth/2.0);   // note: make negative!
  double YScale(magCorner-magLength/2.0);
  int CIndex(buildIndex+1007);
  for(size_t i=0;i<4;i++)
    {
      const Geometry::Vec3D cylLOrg=LOrg+X*XScale+Y*YScale;
      const Geometry::Vec3D cylROrg=ROrg+X*XScale+Y*YScale;
      ModelSupport::buildCylinder(SMap,CIndex,cylLOrg,Z,magCorner);
      ModelSupport::buildCylinder(SMap,CIndex+1000,cylROrg,Z,magCorner);
      CIndex+=10;
      if (i % 2) YScale *= -1.0;
      XScale *= -1.0;
    }


  // Front/back clamp :
  const Geometry::Vec3D COrgA
    (Origin-Y*(magInnerLength/2.0+pipeClampThick/2.0+pipeClampYStep));
  const Geometry::Vec3D COrgB
    (Origin+Y*(magInnerLength/2.0+pipeClampThick/2.0+pipeClampYStep));
  
  ModelSupport::buildPlane
    (SMap,buildIndex+3001,COrgA-Y*(pipeClampThick/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+3002,COrgA+Y*(pipeClampThick/2.0),Y);

  ModelSupport::buildPlane
    (SMap,buildIndex+3101,COrgB-Y*(pipeClampThick/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+3102,COrgB+Y*(pipeClampThick/2.0),Y);

  ModelSupport::buildPlane
    (SMap,buildIndex+3003,COrgA-X*(pipeClampWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+3004,COrgA+X*(pipeClampWidth/2.0),X);

  ModelSupport::buildPlane
    (SMap,buildIndex+3005,COrgA-Z*pipeClampZStep,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+3015,COrgA-Z*(pipeClampZStep+pipeClampHeight),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+3006,COrgA+Z*pipeClampZStep,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+3016,COrgA+Z*(pipeClampZStep+pipeClampHeight),Z);


  return;
}

void
CorrectorMag::createObjects(Simulation& System)
  /*!
    Builds all the objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CorrectorMag","createObjects");
  
  std::string Out;
  const std::string ICell=isActive("Inner") ? getRuleStr("Inner") : "";

  const double extraValue
    (magLength-(magInnerLength+2.0*pipeClampThick+2.0*pipeClampYStep));

  const int extraFlag((extraValue<Geometry::zeroTol) ? 1 :
		      ((extraValue>-Geometry::zeroTol) ? -1 : 0));
  
  const std::string fullSurf = (extraFlag>0) ?
    ModelSupport::getComposite(SMap,buildIndex," 3001 -3102" ) :
    ModelSupport::getComposite(SMap,buildIndex," 1 -2" );
  
  // Frame
  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 1013 -1014 5 -6" );
  makeCell("FrameInnerLeft",System,cellIndex++,frameMat,0.0,Out);  

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 2013 -2014 5 -6" );
  makeCell("FrameInnerRight",System,cellIndex++,frameMat,0.0,Out);  

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 1013 -2014 -5 15" );
  makeCell("FrameBase",System,cellIndex++,frameMat,0.0,Out);  

  Out=ModelSupport::getComposite(SMap,buildIndex," 11 -12 1013 -2014 6 -16" );
  makeCell("FrameTop",System,cellIndex++,frameMat,0.0,Out);  

  // Magnets
  int BI(buildIndex);
  for(const std::string partName : {"Left","Right"} )
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,BI,
		 " 1 -2 1003M -1013M 5 -6 (-1007M:11) (-1027M:-12)");
      makeCell(partName+"Mag",System,cellIndex++,coilMat,0.0,Out);  

      Out=ModelSupport::getComposite
	(SMap,buildIndex,BI," 1 -2 -1004M 1014M 5 -6 (-1017M:11) (-1037M:-12)");
      makeCell(partName+"Mag",System,cellIndex++,coilMat,0.0,Out);
  
      Out=ModelSupport::getComposite
	(SMap,buildIndex,BI," 1 -2 1013M -1014M 5 -6 (-11:12) ");
      makeCell(partName+"Mag",System,cellIndex++,coilMat,0.0,Out);
  
      Out=ModelSupport::getComposite
	(SMap,buildIndex,BI," 1003M -1013M 1 -11  5 -6 1007M ");
      makeCell(partName+"CornerVoid",System,cellIndex++,voidMat,0.0,Out);
      
      Out=ModelSupport::getComposite
	(SMap,buildIndex,BI," -1004M 1014M 1 -11 5 -6 1017M ");
      makeCell(partName+"CornerVoid",System,cellIndex++,voidMat,0.0,Out);
      
      Out=ModelSupport::getComposite
	(SMap,buildIndex,BI," 1003M -1013M -2 12 5 -6 1027M ");
      makeCell(partName+"CornerVoid",System,cellIndex++,voidMat,0.0,Out);
      
      Out=ModelSupport::getComposite
	(SMap,buildIndex,BI," -1004M 1014M -2 12 5 -6 1037M ");
      makeCell(partName+"CornerVoid",System,cellIndex++,voidMat,0.0,Out);

      BI+=1000;
    }

  // clamp:
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 3001 -3002 3003 -3004 -3005 3015 " );
  makeCell("BaseClampA",System,cellIndex++,clampMat,0.0,Out+ICell);  

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 3001 -3002 3003 -3004 3006 -3016 " );
  makeCell("TopClampA",System,cellIndex++,clampMat,0.0,Out+ICell);  

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 3101 -3102 3003 -3004 -3005 3015 " );
  makeCell("BaseClampB",System,cellIndex++,clampMat,0.0,Out+ICell);  

  Out=ModelSupport::getComposite
    (SMap,buildIndex," 3101 -3102 3003 -3004 3006 -3016 " );
  makeCell("TopClampB",System,cellIndex++,clampMat,0.0,Out+ICell);  


  Out=ModelSupport::getComposite
    (SMap,buildIndex," 3001 -3002 3003 -3004 3005 -3006 " );
  makeCell("ClampVoid",System,cellIndex++,voidMat,0.0,Out+ICell);  
  
  Out=ModelSupport::getComposite
    (SMap,buildIndex," 3101 -3102 3003 -3004 3005 -3006 " );
  makeCell("ClampVoid",System,cellIndex++,voidMat,0.0,Out+ICell);
  
  if (extraFlag>0)
    {    
      Out=ModelSupport::getComposite
	(SMap,buildIndex," 3001 -1 1003 -1004 5 -6 " );
      makeCell("MagAVoid",System,cellIndex++,voidMat,0.0,Out+ICell);
      Out=ModelSupport::getComposite
	(SMap,buildIndex," 3001 -1 2003 -2004 5 -6 " );
      makeCell("MagRightVoid",System,cellIndex++,voidMat,0.0,Out+ICell);
      Out=ModelSupport::getComposite
	(SMap,buildIndex," -3102 2 1003 -1004 5 -6 " );
      makeCell("MagRightVoid",System,cellIndex++,voidMat,0.0,Out+ICell);
      Out=ModelSupport::getComposite
	(SMap,buildIndex," -3102 2 2003 -2004 5 -6 " );
      makeCell("MagRightVoid",System,cellIndex++,voidMat,0.0,Out+ICell);
    }
  
  // Voids
  Out=ModelSupport::getComposite
    (SMap,buildIndex,
     " 1003 -2004 6 -16 (-11:12:-1013:2014) "
      " ( -3001 : 3002 : -3003 : 3004 : -3006 : 3016 ) "
      " ( -3101 : 3102 : -3003 : 3004 : -3006 : 3016 ) ");
  makeCell("TopVoid",System,cellIndex++,voidMat,0.0,Out+fullSurf);  

  Out=ModelSupport::getComposite
    (SMap,buildIndex,
     " 1003 -2004 -5 15 (-11:12:-1013:2014) "
     " ( -3001 : 3002 : -3003 : 3004 : 3005 : -3015 ) "
     " ( -3101 : 3102 : -3003 : 3004 : 3005 : -3015 ) ");
  makeCell("BaseVoid",System,cellIndex++,voidMat,0.0,Out+fullSurf);  
  
  // MAIN VOID
  Out=ModelSupport::getComposite(SMap,buildIndex," 1004 -2003 5 -6 "
     " ( -3001 : 3002 : -3003 : 3004 ) "
     " ( -3101 : 3102 : -3003 : 3004 ) " );
  makeCell("Void",System,cellIndex++,voidMat,0.0,Out+ICell+fullSurf);  


  Out=ModelSupport::getComposite
    (SMap,buildIndex,"  1003 -2004 15 -16 " );
  addOuterSurf(Out+fullSurf);

  createLinks(extraFlag);

  return;
}

void 
CorrectorMag::createLinks(const bool extraFlag)
  /*!
    Create the linked units
   */
{
  ELog::RegMethod RegA("CorrectorMag","createLinks");

  if (!extraFlag)
    {
      FixedComp::setConnect(0,Origin-Y*(magLength/2.0),-Y);     
      FixedComp::setConnect(1,Origin+Y*(magLength/2.0),Y);     
      
      FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+11));
      FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));
    }
  else
    {
      const Geometry::Vec3D COrgA
	(Origin-Y*(magInnerLength/2.0+pipeClampThick+pipeClampYStep));
      const Geometry::Vec3D COrgB
	(Origin+Y*(magInnerLength/2.0+pipeClampThick+pipeClampYStep));

      FixedComp::setConnect(0,COrgA,-Y);     
      FixedComp::setConnect(1,COrgB,Y);     
      
      FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+3001));
      FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+3102));
    }
      
  return;
}

void
CorrectorMag::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed point track 
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("CorrectorMag","createAll");
  
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  insertObjects(System);   
  
  return;
}
  
}  // NAMESPACE tdcSystem
