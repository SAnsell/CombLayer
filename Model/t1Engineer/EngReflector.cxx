/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Upgrade/EngReflector.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell/Goran Skoro
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
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "World.h"
#include "EngReflector.h"

namespace ts1System
{

EngReflector::EngReflector(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,13),
  attachSystem::CellMap(),
  refIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(refIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

EngReflector::EngReflector(const EngReflector& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  refIndex(A.refIndex),cellIndex(A.cellIndex),
  radius(A.radius),width(A.width),
  height(A.height),topCutHeight(A.topCutHeight),
  botCutHeight(A.botCutHeight),cutLen(A.cutLen),cutAngle(A.cutAngle),
  topCCHeight(A.topCCHeight),cCoff(A.cCoff),cutRadius(A.cutRadius),
  reflMat(A.reflMat),reflTemp(A.reflTemp),liftHeight(A.liftHeight),
  liftMat(A.liftMat),liftTemp(A.liftTemp),coolPadThick(A.coolPadThick),
  coolPadOffset(A.coolPadOffset),coolPadWidth(A.coolPadWidth),
  coolPadHeight(A.coolPadHeight),coolPadMat(A.coolPadMat),
  coolPadTemp(A.coolPadTemp),inCutXOffset(A.inCutXOffset),
  inCutYOffset(A.inCutYOffset),inCutZOffset(A.inCutZOffset),
  inCutThick(A.inCutThick),inCutWidth(A.inCutWidth),inCutAngle(A.inCutAngle),
  inCutHeight(A.inCutHeight),inCutMat(A.inCutMat),inCutTemp(A.inCutTemp),
  inBWatCutXOffset(A.inBWatCutXOffset),inBWatCutYOffset(A.inBWatCutYOffset),
  inBWatCutZOffset(A.inBWatCutZOffset),inBWatCutHeight(A.inBWatCutHeight),
  inBWatCutRadius(A.inBWatCutRadius),inBWatCutMat(A.inBWatCutMat),
  inBWatCutTemp(A.inBWatCutTemp),inSWatCutXOffset(A.inSWatCutXOffset),
  inSWatCutYOffset(A.inSWatCutYOffset),inSWatCutRadius(A.inSWatCutRadius)                     
  /*!
    Copy constructor
    \param A :: EngReflector to copy
  */
{}

EngReflector&
EngReflector::operator=(const EngReflector& A)
  /*!
    Assignment operator
    \param A :: EngReflector to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      radius=A.radius;
      width=A.width;
      height=A.height;
      topCutHeight=A.topCutHeight; 
      botCutHeight=A.botCutHeight;          
      cutLen=A.cutLen;
      cutAngle=A.cutAngle;      
      topCCHeight=A.topCCHeight; 
      cCoff=A.cCoff;
      cutRadius=A.cutRadius;          
      reflMat=A.reflMat;  
      reflTemp=A.reflTemp; 
      liftHeight=A.liftHeight;
      liftMat=A.liftMat;
      liftTemp=A.liftTemp;      
      coolPadThick=A.coolPadThick;
      coolPadOffset=A.coolPadOffset;
      coolPadWidth=A.coolPadWidth;
      coolPadHeight=A.coolPadHeight;
      coolPadMat=A.coolPadMat;
      coolPadTemp=A.coolPadTemp;      
      inCutXOffset=A.inCutXOffset;
      inCutYOffset=A.inCutYOffset;  
      inCutZOffset=A.inCutZOffset;
      inCutThick=A.inCutThick;
      inCutWidth=A.inCutWidth;
      inCutAngle=A.inCutAngle;
      inCutHeight=A.inCutHeight;
      inCutMat=A.inCutMat;
      inCutTemp=A.inCutTemp;     
      inBWatCutXOffset=A.inBWatCutXOffset;
      inBWatCutYOffset=A.inBWatCutYOffset;  
      inBWatCutZOffset=A.inBWatCutZOffset;
      inBWatCutHeight=A.inBWatCutHeight;
      inBWatCutRadius=A.inBWatCutRadius;     
      inBWatCutMat=A.inBWatCutMat;
      inBWatCutTemp=A.inBWatCutTemp;     
      inSWatCutXOffset=A.inSWatCutXOffset;
      inSWatCutYOffset=A.inSWatCutYOffset;  
      inSWatCutRadius=A.inSWatCutRadius;     
    }
  return *this;
}

EngReflector::~EngReflector() 
  /*!
    Destructor
  */
{}

void
EngReflector::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase
 */
{
  ELog::RegMethod RegA("EngReflector","populate");

  FixedOffset::populate(Control);

  radius=Control.EvalVar<double>(keyName+"Radius");
  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  topCutHeight=Control.EvalVar<double>(keyName+"TopCutHeight"); 
  botCutHeight=Control.EvalVar<double>(keyName+"BotCutHeight");     
  cutLen=Control.EvalVar<double>(keyName+"CutLen");
  cutAngle=Control.EvalVar<double>(keyName+"CutAngle"); 
  topCCHeight=Control.EvalVar<double>(keyName+"TopCCHeight"); 
  cCoff=Control.EvalVar<double>(keyName+"CCoff");
  cutRadius=Control.EvalVar<double>(keyName+"CutRadius");    
  reflMat=ModelSupport::EvalMat<int>(Control,keyName+"ReflMat");
  reflTemp=Control.EvalVar<double>(keyName+"ReflTemp");    

  liftHeight=Control.EvalVar<double>(keyName+"LiftHeight");
  liftMat=ModelSupport::EvalMat<int>(Control,keyName+"LiftMat");
  liftTemp=Control.EvalVar<double>(keyName+"LiftTemp");    
   
  coolPadThick=Control.EvalVar<double>(keyName+"CoolPadThick");
  coolPadOffset=Control.EvalVar<double>(keyName+"CoolPadOffset");
  coolPadWidth=Control.EvalVar<double>(keyName+"CoolPadWidth");
  coolPadHeight=Control.EvalVar<double>(keyName+"CoolPadHeight");
  coolPadMat=ModelSupport::EvalMat<int>(Control,keyName+"CoolPadMat");
  coolPadTemp=Control.EvalVar<double>(keyName+"CoolPadTemp");      

  inCutXOffset=Control.EvalVar<double>(keyName+"InCutXOffset");
  inCutYOffset=Control.EvalVar<double>(keyName+"InCutYOffset");  
  inCutZOffset=Control.EvalVar<double>(keyName+"InCutZOffset");
  inCutThick=Control.EvalVar<double>(keyName+"InCutThick");
  inCutWidth=Control.EvalVar<double>(keyName+"InCutWidth");
  inCutAngle=Control.EvalVar<double>(keyName+"InCutAngle");  
  inCutHeight=Control.EvalVar<double>(keyName+"InCutHeight");
  inCutMat=ModelSupport::EvalMat<int>(Control,keyName+"InCutMat");
  inCutTemp=Control.EvalVar<double>(keyName+"InCutTemp");      

  inBWatCutXOffset=Control.EvalVar<double>(keyName+"InBWatCutXOffset");
  inBWatCutYOffset=Control.EvalVar<double>(keyName+"InBWatCutYOffset");  
  inBWatCutZOffset=Control.EvalVar<double>(keyName+"InBWatCutZOffset");
  inBWatCutHeight=Control.EvalVar<double>(keyName+"InBWatCutHeight");
  inBWatCutRadius=Control.EvalVar<double>(keyName+"InBWatCutRadius");
  inBWatCutMat=ModelSupport::EvalMat<int>(Control,keyName+"InBWatCutMat");
  inBWatCutTemp=Control.EvalVar<double>(keyName+"InBWatCutTemp");   

  inSWatCutXOffset=Control.EvalVar<double>(keyName+"InSWatCutXOffset");
  inSWatCutYOffset=Control.EvalVar<double>(keyName+"InSWatCutYOffset");  
  inSWatCutRadius=Control.EvalVar<double>(keyName+"InSWatCutRadius");
          
  return;
}
  
void
EngReflector::createUnitVector(const attachSystem::FixedComp& FC,
			       const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Linked object
    \param sideIndex 
  */
{
  ELog::RegMethod RegA("EngReflector","createUnitVector");
  FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();

  return;
}
  
void
EngReflector::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("EngReflector","createSurface");

  
  // Outer layer:
  ModelSupport::buildCylinder(SMap,refIndex+7,Origin,Z,radius);
  ModelSupport::buildPlane(SMap,refIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,refIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,refIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,refIndex+6,Origin+Z*height/2.0,Z);

  // Cuts:
  ModelSupport::buildPlaneRotAxis(SMap,refIndex+11,Origin
			   -X*width/2.0-Y*cutLen,-X,Z,cutAngle);			   
  ModelSupport::buildPlaneRotAxis(SMap,refIndex+12,Origin
			   -X*width/2.0+Y*cutLen,-X,Z,-cutAngle);
  ModelSupport::buildPlaneRotAxis(SMap,refIndex+13,Origin
			   +X*width/2.0-Y*cutLen,X,Z,-cutAngle);			   
  ModelSupport::buildPlaneRotAxis(SMap,refIndex+14,Origin
			   +X*width/2.0+Y*cutLen,X,Z,cutAngle);
			   
			   			   
  ModelSupport::buildPlane(SMap,refIndex+15,Origin
               -Z*(height/2.0-botCutHeight),Z);
  ModelSupport::buildPlane(SMap,refIndex+16,Origin
               +Z*(height/2.0-topCutHeight),Z);

  // Cut cylinder:
  ModelSupport::buildCylinder(SMap,refIndex+17,Origin
               -Y*(cCoff+cutRadius),Z,cutRadius);

  ModelSupport::buildPlane(SMap,refIndex+26,Origin
               +Z*(height/2.0-topCCHeight),Z);

  // Lift plate:
  ModelSupport::buildPlane(SMap,refIndex+36,Origin
               +Z*(height/2.0+liftHeight),Z);
               
  // Cooling pads:
  //thicknesses
  ModelSupport::buildPlaneRotAxis(SMap,refIndex+21,Origin
			   -X*(width/2.0+coolPadThick)-Y*cutLen,-X,Z,cutAngle);			   
  ModelSupport::buildPlaneRotAxis(SMap,refIndex+22,Origin
			   -X*(width/2.0+coolPadThick)+Y*cutLen,-X,Z,-cutAngle);
  ModelSupport::buildPlaneRotAxis(SMap,refIndex+23,Origin
			   +X*(width/2.0+coolPadThick)-Y*cutLen,X,Z,-cutAngle);			   
  ModelSupport::buildPlaneRotAxis(SMap,refIndex+24,Origin
			   +X*(width/2.0+coolPadThick)+Y*cutLen,X,Z,cutAngle);
  //sides
  ModelSupport::buildPlaneRotAxis(SMap,refIndex+101,Origin
			   -X*width/2.0
			   -Y*(cutLen+coolPadOffset),-X,Z,-90.+cutAngle);			   
  ModelSupport::buildPlaneRotAxis(SMap,refIndex+201,Origin-X*width/2.0
			   -Y*(cutLen+coolPadOffset+coolPadWidth),-X,Z,-90.0+cutAngle);	

  ModelSupport::buildPlaneRotAxis(SMap,refIndex+102,Origin-X*width/2.0
			   +Y*(cutLen+coolPadOffset),-X,Z,-90.-cutAngle);
  ModelSupport::buildPlaneRotAxis(SMap,refIndex+202,Origin-X*width/2.0
			   +Y*(cutLen+coolPadOffset+coolPadWidth),-X,Z,-90.0-cutAngle);

  ModelSupport::buildPlaneRotAxis(SMap,refIndex+103,Origin+X*width/2.0
			   -Y*(cutLen+coolPadOffset),X,Z,-90.-cutAngle);			   
  ModelSupport::buildPlaneRotAxis(SMap,refIndex+203,Origin+X*width/2.0
			   -Y*(cutLen+coolPadOffset+coolPadWidth),X,Z,-90.0-cutAngle);			   
			   			   			   
  ModelSupport::buildPlaneRotAxis(SMap,refIndex+104,Origin+X*width/2.0
			   +Y*(cutLen+coolPadOffset),X,Z,-90.+cutAngle);					   
  ModelSupport::buildPlaneRotAxis(SMap,refIndex+204,Origin+X*width/2.0
			   +Y*(cutLen+coolPadOffset+coolPadWidth),X,Z,-90.0+cutAngle);	
  //heights		   			   
  ModelSupport::buildPlane(SMap,refIndex+105,
			   Origin-Z*(height/2.0-coolPadHeight),Z);
  ModelSupport::buildPlane(SMap,refIndex+106,
			   Origin+Z*(height/2.0-coolPadHeight),Z);

//
// Cutouts:    Just a temporary solution - would need a possibility to include
//             the object(s) into FlightLines

 //inner cut (Cold Moderators)
  ModelSupport::buildPlane(SMap,refIndex+401,Origin
                +Y*(inCutYOffset-inCutThick/2.0),Y);
  ModelSupport::buildPlane(SMap,refIndex+402,Origin
                +Y*(inCutYOffset+inCutThick/2.0),Y);
  ModelSupport::buildPlaneRotAxis(SMap,refIndex+403,Origin
			   +X*(inCutXOffset-inCutWidth/2.0),X,Z,inCutAngle);		
  ModelSupport::buildPlane(SMap,refIndex+404,Origin
                +X*(inCutXOffset+inCutWidth/2.0),X);
  ModelSupport::buildPlane(SMap,refIndex+405,Origin
                +Z*(inCutZOffset-inCutHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,refIndex+406,Origin
                +Z*(inCutZOffset+inCutHeight/2.0),Z); 

//inner cut(s) (Water Moderator)                

  ModelSupport::buildPlane(SMap,refIndex+415,Origin
                +Z*(inBWatCutZOffset-inBWatCutHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,refIndex+416,Origin
                +Z*(inBWatCutZOffset+inBWatCutHeight/2.0),Z);     
  ModelSupport::buildCylinder(SMap,refIndex+417,Origin
                  +X*inBWatCutXOffset+Y*inBWatCutYOffset,Z,inBWatCutRadius);


  ModelSupport::buildCylinder(SMap,refIndex+427,Origin
                  +X*inSWatCutXOffset+Y*inSWatCutYOffset,Z,inSWatCutRadius);

  return;
}

void
EngReflector::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("EngReflector","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,refIndex,
				 "401 -402 403 -404 405 -406");
  System.addCell(MonteCarlo::Qhull(cellIndex++,inCutMat,inCutTemp,Out));

  Out=ModelSupport::getComposite(SMap,refIndex,
				 "-417 415 -416");
  System.addCell(MonteCarlo::Qhull(cellIndex++,
                            inBWatCutMat,inBWatCutTemp,Out));

  Out=ModelSupport::getComposite(SMap,refIndex,"-427 415 -416");
  System.addCell(MonteCarlo::Qhull(cellIndex++,
                             inBWatCutMat,inBWatCutTemp,Out));
    				   
  Out=ModelSupport::getComposite(SMap,refIndex,"-7 3 -4 5 -6");
  Out+=ModelSupport::getComposite(SMap,refIndex," (-12:7:-16:6) ");
  Out+=ModelSupport::getComposite(SMap,refIndex," (-13:7:-16:6) ");
  Out+=ModelSupport::getComposite(SMap,refIndex," (-11:7:-5:15) ");
  Out+=ModelSupport::getComposite(SMap,refIndex, " (-14:7:-5:15) ");
  Out+=ModelSupport::getComposite(SMap,refIndex," (17:7:-26:6) ");			 
  addOuterSurf(Out);
  Out+=ModelSupport::getComposite(SMap,refIndex," (-401:402:-403:404:-405:406) ");			 
  Out+=ModelSupport::getComposite(SMap,refIndex," (417:-415:416) ");		
  Out+=ModelSupport::getComposite(SMap,refIndex," (427:-415:416) ");		
  System.addCell(MonteCarlo::Qhull(cellIndex++,reflMat,reflTemp,Out));
  addCell("Reflector",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,refIndex,"-7 3 -4 6 -36");
  Out+=ModelSupport::getComposite(SMap,refIndex," (-12:7:36:-6) ");
  Out+=ModelSupport::getComposite(SMap,refIndex," (-13:7:36:-6) ");
  Out+=ModelSupport::getComposite(SMap,refIndex," (17:7:36:-6) ");			 
  System.addCell(MonteCarlo::Qhull(cellIndex++,liftMat,liftTemp,Out));
  addOuterUnionSurf(Out);	

  Out=ModelSupport::getComposite(SMap,refIndex,"11  -21 -101 201 5 -105");
  System.addCell(MonteCarlo::Qhull(cellIndex++,coolPadMat,coolPadTemp,Out));	 
  addOuterUnionSurf(Out);	

  Out=ModelSupport::getComposite(SMap,refIndex,"12  -22 102 -202 106 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,coolPadMat,coolPadTemp,Out));
  addOuterUnionSurf(Out);	
  
  Out=ModelSupport::getComposite(SMap,refIndex,"13  -23 103 -203 106 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,coolPadMat,coolPadTemp,Out));		 
  addOuterUnionSurf(Out);	
  
  Out=ModelSupport::getComposite(SMap,refIndex,"14  -24 -104 204 5 -105");
  System.addCell(MonteCarlo::Qhull(cellIndex++,coolPadMat,coolPadTemp,Out));
  addOuterUnionSurf(Out);				 

    
  return;
}

std::string
EngReflector::getComposite(const std::string& surfList) const
  /*!
    Exposes local version of getComposite
    \param surfList :: surface list
    \return Composite string
  */
{
  return ModelSupport::getComposite(SMap,refIndex,surfList);
}

void
EngReflector::addToInsertChain(attachSystem::ContainedComp& CC) const
  /*!
    Adds this object to the containedComp to be inserted.
    \param CC :: ContainedComp object to add to this
  */
{
  ELog::RegMethod RegA("EngReflector","addToInsertChain");

  for(int i=refIndex+1;i<cellIndex;i++)
    CC.addInsertCell(i);
    
  return;
}


void
EngReflector::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("Reflector","createLinks");

  FixedComp::setLinkSurf(0,SMap.realSurf(refIndex+7));
  FixedComp::setLinkSurf(1,SMap.realSurf(refIndex+11));
  FixedComp::setLinkSurf(2,-SMap.realSurf(refIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(refIndex+12));
  FixedComp::setLinkSurf(4,-SMap.realSurf(refIndex+17));
  FixedComp::setLinkSurf(5,SMap.realSurf(refIndex+14));
  FixedComp::setLinkSurf(6,SMap.realSurf(refIndex+4));
  FixedComp::setLinkSurf(7,SMap.realSurf(refIndex+13));
  FixedComp::setLinkSurf(8,-SMap.realSurf(refIndex+5));
  FixedComp::setLinkSurf(9,SMap.realSurf(refIndex+6));
 
  FixedComp::setLinkSurf(10,SMap.realSurf(refIndex+403));   
  FixedComp::setLinkSurf(11,SMap.realSurf(refIndex+417));   
  FixedComp::setLinkSurf(12,SMap.realSurf(refIndex+427));   
    
  FixedComp::setConnect(0,Origin+Z*radius,Z);
  FixedComp::setConnect(2,Origin-X*width/2.0,-X);
  FixedComp::setConnect(6,Origin+X*width/2.0,X);

  FixedComp::setConnect(1,Origin-X*width/2.0-Y*cutLen,-X-Y);
  FixedComp::setConnect(3,Origin-X*width/2.0+Y*cutLen,-X+Y);
  FixedComp::setConnect(5,Origin+X*width/2.0+Y*cutLen,X+Y);
  FixedComp::setConnect(7,Origin+X*width/2.0-Y*cutLen,X-Y);
  FixedComp::setConnect(8,Origin-Z*height/2.0,-Z);
  FixedComp::setConnect(9,Origin+Z*height/2.0,Z);

  FixedComp::setConnect(10,Origin-X*10.0,-X);
  FixedComp::setConnect(11,Origin
                 +X*inBWatCutXOffset+Y*inBWatCutYOffset
                 +Z*inBWatCutRadius,Z);
  FixedComp::setConnect(12,Origin
                 +X*inSWatCutXOffset+Y*inSWatCutYOffset
                 +Z*inSWatCutRadius,Z);

  
  return;
}

std::vector<int>
EngReflector::getCells() const
  /*!
    Return all the main reflector cells
    \return Vector of cells
  */
{
  std::vector<int> Out;
  for(int i=refIndex+1;i<cellIndex;i++)
    Out.push_back(i);
  return Out;
}


void
EngReflector::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
  */
{
  ELog::RegMethod RegA("EngReflector","createAll");

  populate(System.getDataBase());
  createUnitVector(FC,0);
  createSurfaces();
  createLinks();
  createObjects(System);
  insertObjects(System);              

  return;
}

}  // NAMESPACE ts1System
