/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/PressVessel.cxx
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
#include <numeric>
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
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
#include "FixedUnit.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "channel.h"
#include "boxValues.h"
#include "boxUnit.h"
#include "BoxLine.h"
#include "PressVessel.h"
#include "SurInter.h"

namespace ts1System
{

PressVessel::PressVessel(const std::string& Key)  :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,12),
  outerWallCell(0),IVoidCell(0),targetLen(0.0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PressVessel::PressVessel(const PressVessel& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  outerWallCell(A.outerWallCell),IVoidCell(A.IVoidCell),
  width(A.width),
  height(A.height),length(A.length),frontLen(A.frontLen),
  sideWallThick(A.sideWallThick),topWallThick(A.topWallThick),
  frontWallThick(A.frontWallThick),cutX(A.cutX),cutY(A.cutY),
  viewRadius(A.viewRadius),viewSteelRadius(A.viewSteelRadius),
  viewThickness(A.viewThickness),endYStep1(A.endYStep1),
  endXOutSize1(A.endXOutSize1),endZOutSize1(A.endZOutSize1),
  endYStep2(A.endYStep2),endXOutSize2(A.endXOutSize2),
  endZOutSize2(A.endZOutSize2),steelBulkThick(A.steelBulkThick),
  steelBulkRadius(A.steelBulkRadius),
  watInThick(A.watInThick),watInRad(A.watInRad),          
  nBwch(A.nBwch),begXstep(A.begXstep),endXstep(A.endXstep),       
  bigWchbegThick(A.bigWchbegThick),bigWchbegZstep(A.bigWchbegZstep), 
  bigWchbegHeight(A.bigWchbegHeight),bigWchbegWidth(A.bigWchbegWidth),
  bigWchendThick(A.bigWchendThick),bigWchendZstep(A.bigWchendZstep), 
  bigWchendHeight(A.bigWchendHeight),bigWchendWidth(A.bigWchendWidth),
  winHouseOutRad(A.winHouseOutRad),
  winHouseThick(A.winHouseThick),CItem(A.CItem),wallMat(A.wallMat),
  taMat(A.taMat),waterMat(A.waterMat),sideZCenter(A.sideZCenter)
  /*!
    Copy constructor
    \param A :: PressVessel to copy
  */
{}

PressVessel&
PressVessel::operator=(const PressVessel& A)
  /*!
    Assignment operator
    \param A :: PressVessel to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      outerWallCell=A.outerWallCell;
      IVoidCell=A.IVoidCell;
      width=A.width;
      height=A.height;
      length=A.length;
      frontLen=A.frontLen;
      sideWallThick=A.sideWallThick;
      topWallThick=A.topWallThick;
      frontWallThick=A.frontWallThick;
      cutX=A.cutX;
      cutY=A.cutY;
      viewRadius=A.viewRadius;
      viewSteelRadius=A.viewSteelRadius;
      viewThickness=A.viewThickness;
      endYStep1=A.endYStep1;
      endXOutSize1=A.endXOutSize1;
      endZOutSize1=A.endZOutSize1;
      endYStep2=A.endYStep2;
      endXOutSize2=A.endXOutSize2;
      endZOutSize2=A.endZOutSize2;
      steelBulkThick=A.steelBulkThick;
      steelBulkRadius=A.steelBulkRadius;
      watInThick=A.watInThick;
      watInRad=A.watInRad;                  
      nBwch=A.nBwch;
      begXstep=A.begXstep;
      endXstep=A.endXstep;                  
      bigWchbegThick=A.bigWchbegThick;
      bigWchbegZstep=A.bigWchbegZstep; 
      bigWchbegHeight=A.bigWchbegHeight; 
      bigWchbegWidth=A.bigWchbegWidth;
      bigWchendThick=A.bigWchendThick;
      bigWchendZstep=A.bigWchendZstep; 
      bigWchendHeight=A.bigWchendHeight; 
      bigWchendWidth=A.bigWchendWidth;               
      winHouseOutRad=A.winHouseOutRad;
      winHouseThick=A.winHouseThick;
      CItem=A.CItem;
      wallMat=A.wallMat;
      taMat=A.taMat;
      waterMat=A.waterMat;
      sideZCenter=A.sideZCenter;
    }
  return *this;
}


PressVessel::~PressVessel() 
 /*!
   Destructor
 */
{}

void
PressVessel::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase
 */
{
  ELog::RegMethod RegA("PressVessel","populate");

  FixedOffset::populate(Control);

  width=Control.EvalVar<double>(keyName+"Width");
  height=Control.EvalVar<double>(keyName+"Height");
  length=targetLen+Control.EvalVar<double>(keyName+"Length");
  frontLen=Control.EvalVar<double>(keyName+"Front");
  cutX=Control.EvalVar<double>(keyName+"CornerCutX");
  cutY=Control.EvalVar<double>(keyName+"CornerCutY");
  topWallThick=Control.EvalVar<double>(keyName+"TopWallThick");
  sideWallThick=Control.EvalVar<double>(keyName+"SideWallThick");
  frontWallThick=Control.EvalVar<double>(keyName+"FrontWallThick");
  // Material
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");
  taMat=ModelSupport::EvalMat<int>(Control,keyName+"TaMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  viewRadius=Control.EvalVar<double>("ProtonVoidViewRadius");
  viewSteelRadius=Control.EvalVar<double>(keyName+"ViewSteelRadius");
  viewThickness=Control.EvalVar<double>(keyName+"ViewFrontThick");

  // Pressure Vessel end
  endYStep1=Control.EvalVar<double>(keyName+"EndYStep1");
  endXOutSize1=Control.EvalVar<double>(keyName+"EndXOutSize1");
  endZOutSize1=Control.EvalVar<double>(keyName+"EndZOutSize1");
  endYStep2=Control.EvalVar<double>(keyName+"EndYStep2");
  endXOutSize2=Control.EvalVar<double>(keyName+"EndXOutSize2");
  endZOutSize2=Control.EvalVar<double>(keyName+"EndZOutSize2");

  steelBulkThick=Control.EvalVar<double>(keyName+"SteelBulkThick");
  steelBulkRadius=Control.EvalVar<double>(keyName+"SteelBulkRadius");
 
  watInThick=Control.EvalVar<double>(keyName+"WatInThick");
  watInRad=Control.EvalVar<double>(keyName+"WatInRad");
  
   // Big Water Channels at the end  

  // Number of channels:
  nBwch=Control.EvalVar<size_t>(keyName+"NBwch");
  for(size_t index=0;index<nBwch;index++)
    {
      begXstep.push_back(Control.EvalVar<double>
			 (keyName+"BegXstep"+std::to_string(index)));
      endXstep.push_back(Control.EvalVar<double>
			 (keyName+"EndXstep"+std::to_string(index)));
    }		        
    
  bigWchbegThick=Control.EvalVar<double>(keyName+"BigWchbegThick");
  bigWchbegZstep=Control.EvalVar<double>(keyName+"BigWchbegZstep"); 
  bigWchbegHeight=Control.EvalVar<double>(keyName+"BigWchbegHeight"); 
  bigWchbegWidth=Control.EvalVar<double>(keyName+"BigWchbegWidth");

  bigWchendThick=Control.EvalVar<double>(keyName+"BigWchendThick");
  bigWchendZstep=Control.EvalVar<double>(keyName+"BigWchendZstep"); 
  bigWchendHeight=Control.EvalVar<double>(keyName+"BigWchendHeight"); 
  bigWchendWidth=Control.EvalVar<double>(keyName+"BigWchendWidth");
      
  // WINDOW HOUSING - Tantalum
  winHouseOutRad=Control.EvalVar<double>(keyName+"WinHouseOutRad");
  winHouseThick=Control.EvalVar<double>(keyName+"WinHouseThick");

  // Side feedthrough
  sideZCenter=Control.EvalVar<double>(keyName+"SideZCenter");
  sideXOffset=Control.EvalVar<double>(keyName+"SideXOffset");
  sideHeight=Control.EvalVar<double>(keyName+"SideHeight");
  sideWidth=Control.EvalVar<double>(keyName+"SideWidth");

  return;
}
  
void
PressVessel::createUnitVector(const attachSystem::FixedComp& FC,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: FixedComp for origin and axis
    \param sideIndex :: SideIndex link point
  */
{
  ELog::RegMethod RegA("PressVessel","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
PressVessel::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("PressVessel","createSurface");

  // First layer [Bulk]
  ModelSupport::buildPlane(SMap,buildIndex+1,
			   Origin-Y*frontLen,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,
			   Origin+Y*length,Y);
  ModelSupport::buildPlane(SMap,buildIndex+3,
			   Origin-X*width,X);
  ModelSupport::buildPlane(SMap,buildIndex+4,
			   Origin+X*width,X);
  ModelSupport::buildPlane(SMap,buildIndex+5,
			   Origin-Z*height,Z);
  ModelSupport::buildPlane(SMap,buildIndex+6,
			   Origin+Z*height,Z);

  ModelSupport::buildPlane(SMap,buildIndex+8,
			   Origin-Y*frontLen-X*cutX,
			   Origin+Y*cutY-X*width,
			   Origin+Y*cutY-X*width+Z,
			   -X-Y);
  ModelSupport::buildPlane(SMap,buildIndex+9,
			   Origin-Y*frontLen+X*cutX,
			   Origin+Y*cutY+X*width,
			   Origin+Y*cutY+X*width+Z,
			   X-Y);

  ModelSupport::buildPlane(SMap,buildIndex+11,
			   Origin-Y*(frontLen-frontWallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+12,
			   Origin+Y*(length-sideWallThick),Y);
  ModelSupport::buildPlane(SMap,buildIndex+13,
			   Origin-X*(width-sideWallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+14,
			   Origin+X*(width-sideWallThick),X);
  ModelSupport::buildPlane(SMap,buildIndex+15,
			   Origin-Z*(height-topWallThick),Z);
  ModelSupport::buildPlane(SMap,buildIndex+16,
			   Origin+Z*(height-topWallThick),Z);
  
  const Geometry::Plane* PX=SMap.realPtr<Geometry::Plane>(buildIndex+8);
  ModelSupport::buildPlane(SMap,buildIndex+18,
			   Origin-Y*frontLen-X*cutX-
			   PX->getNormal()*sideWallThick,
			   PX->getNormal());
  PX=SMap.realPtr<Geometry::Plane>(buildIndex+9);
  ModelSupport::buildPlane(SMap,buildIndex+19,
			   Origin-Y*frontLen+X*cutX-
			   PX->getNormal()*sideWallThick,
			   PX->getNormal());

  // Front face insert
  ModelSupport::buildPlane(SMap,buildIndex+21,
			   Origin-Y*viewThickness,Y);
  ModelSupport::buildPlane(SMap,buildIndex+22,Origin,Y);
  
  ModelSupport::buildCylinder(SMap,buildIndex+27,Origin,Y,viewRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+28,Origin,Y,viewSteelRadius);
  
  // Pressure Vessel end
  ModelSupport::buildPlane(SMap,buildIndex+31,
			   Origin+Y*(length-endYStep1),Y);
  ModelSupport::buildPlane(SMap,buildIndex+33,
			   Origin-X*endXOutSize1/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+34,
			   Origin+X*endXOutSize1/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+35,
			   Origin-Z*endZOutSize1/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+36,
			   Origin+Z*endZOutSize1/2.0,Z);

  ModelSupport::buildPlane(SMap,buildIndex+41,
			   Origin+Y*(length-endYStep2),Y);
  ModelSupport::buildPlane(SMap,buildIndex+43,
			   Origin-X*endXOutSize2/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+44,
			   Origin+X*endXOutSize2/2.0,X);
  ModelSupport::buildPlane(SMap,buildIndex+45,
			   Origin-Z*endZOutSize2/2.0,Z);
  ModelSupport::buildPlane(SMap,buildIndex+46,
			   Origin+Z*endZOutSize2/2.0,Z);

  ModelSupport::buildPlane(SMap,buildIndex+51,
			   Origin+Y*(length+steelBulkThick),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+57,Origin,Y,steelBulkRadius);


  ModelSupport::buildPlane(SMap,buildIndex+61,
			   Origin-Y*winHouseThick,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+67,Origin,Y,winHouseOutRad);
  
  // Water IN  
  
  ModelSupport::buildPlane(SMap,buildIndex+71,
			   Origin+Y*(length+watInThick),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+77,Origin,Y,watInRad);					   


   // Big Water Channels at the end 
  const Geometry::Plane* endYPlane=
    ModelSupport::buildPlane(SMap,buildIndex+701,
			     Origin+Y*(length+steelBulkThick-bigWchendThick),Y);

  const Geometry::Plane* begYPlane=
     ModelSupport::buildPlane(SMap,buildIndex+801,
			      Origin+Y*(length+bigWchbegThick),Y);

  const Geometry::Plane* enddZPlane=
    ModelSupport::buildPlane(SMap,buildIndex+705,
			     Origin+Z*(bigWchendZstep-bigWchendHeight/2.0),Z);

  const Geometry::Plane* endtZPlane=
    ModelSupport::buildPlane(SMap,buildIndex+706,
			     Origin+Z*(bigWchendZstep+bigWchendHeight/2.0),Z);

  const Geometry::Plane* begdZPlane=
    ModelSupport::buildPlane(SMap,buildIndex+805,
			     Origin+Z*(bigWchbegZstep-bigWchbegHeight/2.0),Z);

  const Geometry::Plane* begtZPlane=
    ModelSupport::buildPlane(SMap,buildIndex+806,
			     Origin+Z*(bigWchbegZstep+bigWchbegHeight/2.0),Z);	
 
  int SN(buildIndex+1000);
  for(size_t i=0;i<nBwch;i++)
    {
      const Geometry::Plane* endmXPlane=
	ModelSupport::buildPlane(SMap,SN+3,Origin+
				 X*(endXstep[i]-bigWchendWidth/2.0),X);

      const Geometry::Plane* endpXPlane=
	ModelSupport::buildPlane(SMap,SN+4,Origin+
				 X*(endXstep[i]+bigWchendWidth/2.0),X);		

      const Geometry::Plane* begmXPlane=
	ModelSupport::buildPlane(SMap,SN+13,Origin+
				 X*(begXstep[i]-bigWchbegWidth/2.0),X);

      const Geometry::Plane* begpXPlane=
	ModelSupport::buildPlane(SMap,SN+14,Origin+
				 X*(begXstep[i]+bigWchbegWidth/2.0),X);		
      
      Geometry::Vec3D WPts[8];
      WPts[0]=SurInter::getPoint(begmXPlane,begYPlane,begtZPlane);
      WPts[1]=SurInter::getPoint(endmXPlane,endYPlane,endtZPlane);
      WPts[2]=SurInter::getPoint(endmXPlane,endYPlane,enddZPlane);    
      WPts[3]=SurInter::getPoint(begpXPlane,begYPlane,begtZPlane);
      WPts[4]=SurInter::getPoint(endpXPlane,endYPlane,endtZPlane);
      WPts[5]=SurInter::getPoint(endpXPlane,endYPlane,enddZPlane);  
      WPts[6]=SurInter::getPoint(begmXPlane,begYPlane,begdZPlane);
      WPts[7]=SurInter::getPoint(endpXPlane,endYPlane,endtZPlane);
      
      // left 
      ModelSupport::buildPlane(SMap,SN+23,WPts[0],WPts[1],WPts[2],X);
      // right 
      ModelSupport::buildPlane(SMap,SN+24,WPts[3],WPts[4],WPts[5],X);
      // bottom  
      ModelSupport::buildPlane(SMap,SN+25,WPts[6],WPts[2],WPts[5],Z);      
      // top  
      ModelSupport::buildPlane(SMap,SN+26,WPts[0],WPts[1],WPts[7],Z);

      SN+=100;
    }
  return;
}

void
PressVessel::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("PressVessel","createObjects");
  
  std::string Out;

  HeadRule HR;
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 3 -4 5 -6 -8 -9 ");
  addOuterSurf(Out);

  Out+=ModelSupport::getComposite(SMap,buildIndex,"(-11:12:-13:14:-15:16:18:19)"
				  " (11:28) (11:-61:67:-28) ");
  Out+=ModelSupport::getComposite(SMap,buildIndex, "(-12:71:77) ");                       				  
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  outerWallCell=cellIndex-1;

  // Inner Volume
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -12 13 -14 15 -16 -18 -19 "
				 " (28:22) ");
  System.addCell(MonteCarlo::Object(cellIndex++,waterMat,0.0,Out));
  IVoidCell=cellIndex-1;

  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -22 (27:21) -28 ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  
    // Water IN  
  Out=ModelSupport::getComposite(SMap,buildIndex,"12 -71 -77 ");
  System.addCell(MonteCarlo::Object(cellIndex++,waterMat,0.0,Out)); 

   // Big Water Channels at the end    
  int SN(buildIndex+1000);
  for(size_t i=0;i<nBwch;i++)  
    {
      Out=ModelSupport::getComposite(SMap,buildIndex,SN,"701 -51 705 -706 3M -4M");
      System.addCell(MonteCarlo::Object(cellIndex++,waterMat,0.0,Out)); 
      
      Out=ModelSupport::getComposite(SMap,buildIndex,SN,
				     "2 -801 805 -806 13M -14M");
      System.addCell(MonteCarlo::Object(cellIndex++,waterMat,0.0,Out));  

      Out=ModelSupport::getComposite(SMap,buildIndex,SN,
				     "801 -701 23M -24M 25M -26M");
      System.addCell(MonteCarlo::Object(cellIndex++,waterMat,0.0,Out));  
      SN+=100;
     }

  // Pressure Vessel end
  Out=ModelSupport::getComposite(SMap,buildIndex, "31 -41 33 -34 35 -36 ");
  addOuterUnionSurf(Out);
  Out+=ModelSupport::getComposite(SMap,buildIndex, "(-31:41:-3:4:-5:6) ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex, "41 -2 43 -44 45 -46 ");
  addOuterUnionSurf(Out);
  Out+=ModelSupport::getComposite(SMap,buildIndex, "(-41:2:-3:4:-5:6) ");
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex, "2 -51 -57 ");
  addOuterUnionSurf(Out);


  // THIS NEEDS TO BE IN SECTIONS: 
  // [ i.e not in one object unless surrounded by a wrapper]

  std::string OutA=
    ModelSupport::getComposite(SMap,buildIndex, "((-701:51:-705:706) : ( ");
  std::string OutB=
    ModelSupport::getComposite(SMap,buildIndex, "((-2:801:-805:806) : ( ");
  std::string OutC=
    ModelSupport::getComposite(SMap,buildIndex, "((-801:701) : ( ");
  SN=buildIndex+1000;
  for(size_t i=0;i<nBwch;SN+=100,i++)  
    {
      // THIS WORKS -- therefore it is independent and redundent (?)
      OutA+=ModelSupport::getComposite(SMap,SN," (-3:4) ");
      OutB+=ModelSupport::getComposite(SMap,SN," (-13:14) ");
      OutC+=ModelSupport::getComposite(SMap,SN," (-23:24:-25:26)");
    }
  OutA+="))";
  OutB+="))";
  OutC+="))";
  Out+=OutA+OutB+OutC; 
  Out+=ModelSupport::getComposite(SMap,buildIndex, "(-2:71:77) ");                       
  System.addCell(MonteCarlo::Object(cellIndex++,wallMat,0.0,Out));
  // WINDOW HOUSING - Tantalum
  Out=ModelSupport::getComposite(SMap,buildIndex, "-22 61 -67 28");
  System.addCell(MonteCarlo::Object(cellIndex++,taMat,0.0,Out));

  return;
}

int
PressVessel::addProtonLine(Simulation& System,
			   const std::string& RefSurfBoundary)
   /*!
     Adds a proton line up to the edge of the reflector
     \param System :: Simulation to add
     \param RefSurfBoundary :: reflector outside boundary
     \return proton cell nubmer
   */
{
  ELog::RegMethod RegA("PressVessel","addProtonLine");
  // Proton flight line:
  std::string Out=ModelSupport::getComposite(SMap,buildIndex, "-21 -27 ");
  Out+=RefSurfBoundary;
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  return cellIndex-1;
}

void
PressVessel::createLinks()
  /*!
    Creates a full attachment set [Internal]
  */
{
  // set Links :: Inner links:
  FixedComp::setConnect(0,Origin,Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(buildIndex+22));

  FixedComp::setConnect(1,Origin+Y*(length-sideWallThick),-Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+12));

  FixedComp::setConnect(2,Origin-X*(width-sideWallThick),X);
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+13));

  FixedComp::setConnect(3,Origin+X*(width-sideWallThick),-X);
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+14));

  FixedComp::setConnect(4,Origin-Z*(height-topWallThick),Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(buildIndex+15));

  FixedComp::setConnect(5,Origin+Z*(height-topWallThick),-Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(buildIndex+16));
  // Exit window
  FixedComp::setConnect(6,Origin-Y*viewThickness,Y);
  FixedComp::setLinkSurf(6,SMap.realSurf(buildIndex+21));

  // Outer Layers:
  FixedComp::setConnect(7,Origin-X*width,X);
  FixedComp::setLinkSurf(7,SMap.realSurf(buildIndex+3));

  FixedComp::setConnect(8,Origin+X*width,-X);
  FixedComp::setLinkSurf(8,SMap.realSurf(buildIndex+4));

  FixedComp::setConnect(9,Origin-Z*height,Z);
  FixedComp::setLinkSurf(9,SMap.realSurf(buildIndex+5));

  FixedComp::setConnect(10,Origin+Z*height,-Z);
  FixedComp::setLinkSurf(10,SMap.realSurf(buildIndex+6));

  FixedComp::setConnect(11,Origin+Y*length,-Y);
  FixedComp::setLinkSurf(11,SMap.realSurf(buildIndex+2));

  return;
}

void
PressVessel::buildChannels(Simulation& System)
  /*!
    Construct the channels in the outer steel
    \param System :: Simuation for adding cells 
  */
{
  ELog::RegMethod RegA("PressVessel","buildChannels");

  const FuncDataBase& Control=System.getDataBase();

  const int NChannel= Control.EvalVar<int>(keyName+"NChannel");
  channel* CPtr(0);
  for(int i=0;i<NChannel;i++)
    {
      CItem.push_back(channel("PVesselChannel",i,buildIndex));
      CPtr=&CItem.back();
      CPtr->addInsertCell(outerWallCell);
      CPtr->setDefaultValues(Control,CPtr);
      CPtr->createAll(System,*this,0);
    }
  return;
}

void
PressVessel::buildFeedThrough(Simulation& System)
  /*!
    Build the feed through in the pressure steel vessel
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("PressVessel","buildFeedThrough");

  for(int i=0;i<4;i++)
    {
      ModelSupport::BoxLine 
	SideWaterChannel("sideWaterChannel"+std::to_string(i+1));   

      const double sX((i % 2) ? -1 : 1);
      const double sZ((i / 2) ? -1 : 1);
//       ELog::EM<<"Start of feedThrough"<<ELog::endDebug;
      const Geometry::Vec3D PStart=getLinkPt(1)+Y*cutY+
	        X*sX*sideXOffset+Z*sZ*(sideZCenter-sideHeight);
      const Geometry::Vec3D PEnd=getLinkPt(2)+
	        X*sX*sideXOffset+Z*sZ*(sideZCenter-sideHeight);

      SideWaterChannel.addPoint(PStart);
      SideWaterChannel.addPoint(PEnd);
      SideWaterChannel.addSection(sideWidth,sideHeight,waterMat,0.0);
      SideWaterChannel.setInitZAxis(Z);
      SideWaterChannel.build(System);
    }

  for(size_t i=0;i<nBwch;i++)
    {
      ModelSupport::BoxLine 
	EndWaterChannel("endWaterChannel"+std::to_string(i+1));   

//      const double sX((i % 2) ? -1 : 1);
//      const double sZ((i / 2) ? -1 : 1);
//       ELog::EM<<"Start of feedThrough"<<ELog::endDebug;
      const Geometry::Vec3D PStart=getLinkPt(2)+X*begXstep[i];
      const Geometry::Vec3D PEnd=getLinkPt(12)+X*begXstep[i];

      EndWaterChannel.addPoint(PStart);
      EndWaterChannel.addPoint(PEnd);
      EndWaterChannel.addSection(bigWchbegWidth/2.0,bigWchbegHeight/2.0,waterMat,0.0);
      EndWaterChannel.setInitZAxis(Z);
      EndWaterChannel.build(System);
    }

  return;
}


void
PressVessel::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int sideIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: FixedComp for origin
    \param sideIndex :: Offset point
  */
{
  ELog::RegMethod RegA("PressVessel","createAll");
  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  buildChannels(System);
//  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
