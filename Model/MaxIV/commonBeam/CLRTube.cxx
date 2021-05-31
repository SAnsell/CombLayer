/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonBeam/CLRTube.cxx
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#include "surfEqual.h"
#include "Quadratic.h"
#include "Plane.h"
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
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "ExternalCut.h"

#include "CLRTube.h"

namespace xraySystem
{

CLRTube::CLRTube(const std::string& Key) :
  attachSystem::ContainedGroup("Main","PortA","PortB"),
  attachSystem::FixedRotate(Key,8),
  attachSystem::CellMap(),
  attachSystem::SurfMap(),
  attachSystem::ExternalCut()
 /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CLRTube::~CLRTube()
  /*!
    Destructor
  */
{}

void
CLRTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("CLRTube","populate");

  FixedRotate::populate(Control);

  length=Control.EvalVar<double>(keyName+"Length");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");

  innerLength=Control.EvalVar<double>(keyName+"InnerLength");

  zLift=Control.EvalDefVar<double>(keyName+"ZLift",0.0);
  inBeam=Control.EvalDefVar<int>(keyName+"InBeam",1);
  captureWidth=Control.EvalVar<double>(keyName+"CaptureWidth");
  captureHeight=Control.EvalVar<double>(keyName+"CaptureHeight");
  captureDepth=Control.EvalVar<double>(keyName+"CaptureDepth");

  supportWidth=Control.EvalVar<double>(keyName+"SupportWidth");
  supportHeight=Control.EvalVar<double>(keyName+"SupportHeight");
  supportDepth=Control.EvalVar<double>(keyName+"SupportDepth");

  magWidth=Control.EvalVar<double>(keyName+"MagWidth");
  magHeight=Control.EvalVar<double>(keyName+"MagHeight");
  magDepth=Control.EvalVar<double>(keyName+"MagDepth");

  lensNSize=Control.EvalVar<size_t>(keyName+"LensNSize");
  lensLength=Control.EvalVar<double>(keyName+"LensLength");
  lensMidGap=Control.EvalVar<double>(keyName+"LensMidGap");
  lensRadius=Control.EvalVar<double>(keyName+"LensRadius");  
  lensOuterRadius=Control.EvalVar<double>(keyName+"LensOuterRadius");
  lensSupportRadius=Control.EvalVar<double>(keyName+"LensSupportRadius");

  innerRadius=Control.EvalVar<double>(keyName+"InnerRadius");
  innerThick=Control.EvalVar<double>(keyName+"InnerThick");

  portLength=Control.EvalVar<double>(keyName+"PortLength");
  portRadius=Control.EvalVar<double>(keyName+"PortRadius");
  portThick=Control.EvalVar<double>(keyName+"PortThick");
  
  flangeRadius=Control.EvalVar<double>(keyName+"FlangeRadius");
  flangeLength=Control.EvalVar<double>(keyName+"FlangeLength");

  voidMat=ModelSupport::EvalMat<int>(Control,keyName+"VoidMat");
  lensMat=ModelSupport::EvalMat<int>(Control,keyName+"LensMat");
  lensOuterMat=ModelSupport::EvalMat<int>(Control,keyName+"LensOuterMat");
  pipeMat=ModelSupport::EvalMat<int>(Control,keyName+"PipeMat");
  mainMat=ModelSupport::EvalMat<int>(Control,keyName+"MainMat");
  magnetMat=ModelSupport::EvalMat<int>(Control,keyName+"MagnetMat");
  flangeMat=ModelSupport::EvalMat<int>(Control,keyName+"FlangeMat");

  return;
}

void
CLRTube::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CLRTube","createSurfaces");

  if (!isActive("front"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+1,Origin-Y*(portLength+length/2.0),Y);
      ExternalCut::setCutSurf("front",SMap.realSurf(buildIndex+1));
    }

  if (!isActive("back"))
    {
      ModelSupport::buildPlane
	(SMap,buildIndex+2,Origin+Y*(portLength+length/2.0),Y);
      ExternalCut::setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }
  const Geometry::Vec3D beamOrg((inBeam) ? Origin : Origin+Z*zLift);

  ModelSupport::buildCylinder
    (SMap,buildIndex+7,beamOrg,Y,innerRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+17,beamOrg,Y,innerRadius+innerThick);

  // main 
  ModelSupport::buildPlane
    (SMap,buildIndex+101,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+102,Origin+Y*(length/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+103,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+104,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+105,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+106,Origin+Z*(height/2.0),Z);
  ModelSupport::buildCylinder
    (SMap,buildIndex+107,Origin,Y,innerRadius);


  // Support space (outer)
  ModelSupport::buildPlane
    (SMap,buildIndex+203,Origin-X*(supportWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+204,Origin+X*(supportWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+205,Origin-Z*supportDepth,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+206,Origin+Z*supportHeight,Z);

  // moving support (outer surf)
  ModelSupport::buildPlane
    (SMap,buildIndex+301,beamOrg-Y*(innerLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+302,beamOrg+Y*(innerLength/2.0),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+303,beamOrg-X*(magWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+304,beamOrg+X*(magWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+305,beamOrg-Z*magDepth,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+306,beamOrg+Z*magHeight,Z);

  // capture
  ModelSupport::buildPlane
    (SMap,buildIndex+403,beamOrg-X*(captureWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+404,beamOrg+X*(captureWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,buildIndex+405,beamOrg-Z*captureDepth,Z);
  ModelSupport::buildPlane
    (SMap,buildIndex+406,beamOrg+Z*captureHeight,Z);


  // Inner tube
  ModelSupport::buildCylinder
    (SMap,buildIndex+527,Origin,Y,flangeRadius);

  ModelSupport::buildPlane
    (SMap,buildIndex+501,Origin-Y*(length/2.0+portThick),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+502,Origin+Y*(length/2.0+portThick),Y);  

  
  // Port Radius
  ModelSupport::buildCylinder
    (SMap,buildIndex+607,Origin,Y,portRadius);
  ModelSupport::buildCylinder
    (SMap,buildIndex+617,Origin,Y,portRadius+portThick);

  ModelSupport::buildPlane
    (SMap,buildIndex+611,Origin-Y*(length/2.0+portLength-flangeLength),Y);
  ModelSupport::buildPlane
    (SMap,buildIndex+612,Origin+Y*(length/2.0+portLength-flangeLength),Y);  

  // LENS:

  makeCylinder("LensOuter",SMap,buildIndex+1007,beamOrg,Y,lensOuterRadius);
  makeCylinder("LensSupport",SMap,buildIndex+1017,beamOrg,Y,lensSupportRadius);

  int SIndex(buildIndex+1000);

  const double lStep(lensLength*static_cast<double>(lensNSize)/2.0);
  Geometry::Vec3D lensOrg(beamOrg-Y*lStep);

  ModelSupport::buildPlane(SMap,SIndex+1,lensOrg-Y*(lensLength/2.0),Y);
  for(size_t i=0;i<lensNSize;i++)
    {
      makePlane("LensDivider",SMap,SIndex+11,lensOrg+Y*(lensLength/2.0),Y);
      ModelSupport::buildSphere
	(SMap,SIndex+8,lensOrg-Y*(lensRadius+lensMidGap/2.0),lensRadius);
      ModelSupport::buildSphere
      	(SMap,SIndex+9,lensOrg+Y*(lensRadius+lensMidGap/2.0),lensRadius);
      lensOrg+=Y*lensLength;
      
      SIndex+=10;
    }    
  return;
}

void
CLRTube::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CLRTube","createObjects");

  HeadRule HR;
  const HeadRule frontHR(getRule("front"));
  const HeadRule backHR(getRule("back"));
  const int lSize(buildIndex+static_cast<int>(lensNSize)*10);
  
  // Main pipe [exclude lens]
  if (lensNSize)
    {
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,lSize,"301 -302 -7 (-1001:1001M:1017)");
    }
  else
    {
      HR=ModelSupport::getHeadRule(SMap,buildIndex,lSize,"301 -302 -7 ");
    }    

  makeCell("Void",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"301 -302 7 -17");
  makeCell("Pipe",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"301 -302 17 403 -404 405 -406");
  makeCell("captureVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"301 -302 303 -304 305 -306 (-403:404:-405:406)");
  makeCell("MoveOuter",System,cellIndex++,magnetMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"301 -302 203 -204 205 -206 (-303:304:-305:306)");
  makeCell("Support",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"101 -102 103 -104 105 -106 (-203:204:-205:206)");
  makeCell("Outer",System,cellIndex++,mainMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"203 -204 205 -206 101 -301");
  makeCell("EndVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"203 -204 205 -206 -102 302");
  makeCell("EndVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -607 -501 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex," -607 502 ");
  makeCell("Void",System,cellIndex++,voidMat,0.0,HR*backHR);
  

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"17 103 -104 105 -106  501 -101");
  makeCell("PortPlate",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 501 -101");
  makeCell("PortVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"17 103 -104 105 -106  -502 102");
  makeCell("PortPlate",System,cellIndex++,pipeMat,0.0,HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-17 -502 102");
  makeCell("PortVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-617 607 -501 ");
  makeCell("PortTube",System,cellIndex++,pipeMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-617 607 502 ");
  makeCell("PortTube",System,cellIndex++,pipeMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-527 617 -611");
  makeCell("FlangeA",System,cellIndex++,flangeMat,0.0,HR*frontHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-527 617 612");
  makeCell("FlangeB",System,cellIndex++,flangeMat,0.0,HR*backHR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-527 617 -501 611");
  makeCell("PortVoid",System,cellIndex++,voidMat,0.0,HR);

  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"-527 617 502 -612");
  makeCell("PortVoid",System,cellIndex++,voidMat,0.0,HR);


  // make lens:
  if (lensNSize) 
    {
      // support
      HR=ModelSupport::getHeadRule
	(SMap,buildIndex,lSize,"1001 -1001M -1017 1007");
      makeCell("LensSupport",System,cellIndex++,lensOuterMat,0.0,HR);

      int SIndex(buildIndex+1000);
      
      
      for(size_t i=0;i<lensNSize;i++)
	{
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,SIndex,"1M -8M -1007");
	  makeCell("LensVoid",System,cellIndex++,voidMat,0.0,HR);
	  HR=ModelSupport::getHeadRule(SMap,buildIndex,SIndex,"-11M -9M -1007");
	  makeCell("LensVoid",System,cellIndex++,voidMat,0.0,HR);
	  HR=ModelSupport::getHeadRule
	    (SMap,buildIndex,SIndex,"1M -11M 8M 9M -1007");
	  makeCell("Lens",System,cellIndex++,lensMat,0.0,HR);
	  SIndex+=10;
	}    
    }
  
  HR=ModelSupport::getHeadRule
    (SMap,buildIndex,"501 -502 103 -104 105 -106");
  addOuterSurf("Main",HR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"-501 -527");
  addOuterSurf("PortA",HR*frontHR);

  HR=ModelSupport::getHeadRule(SMap,buildIndex,"502 -527");
  addOuterSurf("PortB",HR*backHR);
  
  return;
}


void
CLRTube::createLinks()
  /*!
    Create all the linkes
  */
{
  ELog::RegMethod RegA("CLRTube","createLinks");

  ExternalCut::createLink("front",*this,0,Origin,Y);
  ExternalCut::createLink("back",*this,1,Origin,Y);
  
  return;
}

void
CLRTube::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Si\mulation item
    \param FC :: Central origin
    \param sideIndex :: link point for origin
  */
{
  ELog::RegMethod RegA("CLRTube","createAll");

  populate(System.getDataBase());
  createCentredUnitVector(FC,sideIndex,length/2.0+portLength);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);

  return;
}

}  // constructSystem
