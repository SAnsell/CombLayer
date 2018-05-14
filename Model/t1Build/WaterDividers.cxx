/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/WaterDividers.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#include "Cylinder.h"
#include "Line.h"
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
#include "SimProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "PlateTarget.h"
#include "WaterDividers.h"

namespace ts1System
{

WaterDividers::WaterDividers(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,0),
  wIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(wIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

WaterDividers::WaterDividers(const WaterDividers& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  wIndex(A.wIndex),cellIndex(A.cellIndex),
  conHeight(A.conHeight),
  fblkConnect(A.fblkConnect),fblkSize(A.fblkSize),
  fblkSndStep(A.fblkSndStep),fblkSndOut(A.fblkSndOut),
  fblkSndWidth(A.fblkSndWidth),fblkSndLen(A.fblkSndLen),
  fblkWallThick(A.fblkWallThick),fblkEndLen(A.fblkEndLen),
  fblkEndThick(A.fblkEndThick),mblkConnect(A.mblkConnect),
  mblkSize(A.mblkSize),mblkOutRad(A.mblkOutRad),
  mblkInRad(A.mblkInRad),mblkExtLen(A.mblkExtLen),
  mblkExtWidth(A.mblkExtWidth),mblkWallThick(A.mblkWallThick),
  mblkEndLen(A.mblkEndLen),mblkEndThick(A.mblkEndThick),
  bblkConnect(A.bblkConnect),bblkSize(A.bblkSize),
  bblkOutRad(A.bblkOutRad),bblkInRad(A.bblkInRad),
  bblkExtLen(A.bblkExtLen),bblkExtWidth(A.bblkExtWidth),
  bblkWallThick(A.bblkWallThick),bblkEndLen(A.bblkEndLen),
  bblkEndThick(A.bblkEndThick),
  insOutThick(A.insOutThick),insInThick(A.insInThick),insWidth(A.insWidth),
  insHeight(A.insHeight),insRad(A.insRad),dMat(A.dMat),insMat(A.insMat),
  cornerThick(A.cornerThick),cornerWidth(A.cornerWidth)       
  /*!
    Copy constructor
    \param A :: WaterDividers to copy
  */
{}

WaterDividers&
WaterDividers::operator=(const WaterDividers& A)
  /*!
    Assignment operator
    \param A :: WaterDividers to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      conHeight=A.conHeight;
      fblkConnect=A.fblkConnect;
      fblkSize=A.fblkSize;
      fblkSndStep=A.fblkSndStep;
      fblkSndOut=A.fblkSndOut;
      fblkSndWidth=A.fblkSndWidth;
      fblkSndLen=A.fblkSndLen;
      fblkWallThick=A.fblkWallThick;
      fblkEndLen=A.fblkEndLen;
      fblkEndThick=A.fblkEndThick;
      mblkConnect=A.mblkConnect;
      mblkSize=A.mblkSize;
      mblkOutRad=A.mblkOutRad;
      mblkInRad=A.mblkInRad;
      mblkExtLen=A.mblkExtLen;
      mblkExtWidth=A.mblkExtWidth;
      mblkWallThick=A.mblkWallThick;
      mblkEndLen=A.mblkEndLen;
      mblkEndThick=A.mblkEndThick;
      bblkConnect=A.bblkConnect;
      bblkSize=A.bblkSize;
      bblkOutRad=A.bblkOutRad;
      bblkInRad=A.bblkInRad;
      bblkExtLen=A.bblkExtLen;
      bblkExtWidth=A.bblkExtWidth;
      bblkWallThick=A.bblkWallThick;
      bblkEndLen=A.bblkEndLen;
      bblkEndThick=A.bblkEndThick;
      insOutThick=A.insOutThick;
      insInThick=A.insInThick;
      insWidth=A.insWidth;
      insHeight=A.insHeight;              
      insRad=A.insRad;                   
      dMat=A.dMat;
      insMat=A.insMat;
      cornerThick=A.cornerThick;
      cornerWidth=A.cornerWidth;            
    }
  return *this;
}

WaterDividers::~WaterDividers() 
 /*!
   Destructor
  */
{}

void
WaterDividers::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database
 */
{
  ELog::RegMethod RegA("WaterDividers","populate");
  
  conHeight=Control.EvalVar<double>(keyName+"ConnectHeight");

  fblkConnect=Control.EvalVar<size_t>(keyName+"FBlkConnect");
  fblkSize=Control.EvalVar<double>(keyName+"FBlkSize");
  fblkSndStep=Control.EvalVar<double>(keyName+"FBlkSndStep");
  fblkSndOut=Control.EvalVar<double>(keyName+"FBlkSndOut");
  fblkSndWidth=Control.EvalVar<double>(keyName+"FBlkSndWidth");
  fblkSndLen=Control.EvalVar<double>(keyName+"FBlkSndLen");
  fblkWallThick=Control.EvalVar<double>(keyName+"FBlkWallThick");
  fblkEndLen=Control.EvalVar<double>(keyName+"FBlkEndLen");
  fblkEndThick=Control.EvalVar<double>(keyName+"FBlkEndThick");

  mblkConnect=Control.EvalVar<size_t>(keyName+"MBlkConnect");
  mblkSize=Control.EvalVar<double>(keyName+"MBlkSize");
  mblkOutRad=Control.EvalVar<double>(keyName+"MBlkOutRad");
  mblkInRad=Control.EvalVar<double>(keyName+"MBlkInRad");
  mblkExtLen=Control.EvalVar<double>(keyName+"MBlkExtLen");
  mblkExtWidth=Control.EvalVar<double>(keyName+"MBlkExtWidth");
  mblkWallThick=Control.EvalVar<double>(keyName+"MBlkWallThick");
  mblkEndLen=Control.EvalVar<double>(keyName+"MBlkEndLen");
  mblkEndThick=Control.EvalVar<double>(keyName+"MBlkEndThick");

  bblkConnect=Control.EvalVar<size_t>(keyName+"BBlkConnect");
  bblkSize=Control.EvalVar<double>(keyName+"BBlkSize");
  bblkOutRad=Control.EvalVar<double>(keyName+"BBlkOutRad");
  bblkInRad=Control.EvalVar<double>(keyName+"BBlkInRad");
  bblkExtLen=Control.EvalVar<double>(keyName+"BBlkExtLen");
  bblkExtWidth=Control.EvalVar<double>(keyName+"BBlkExtWidth");
  bblkWallThick=Control.EvalVar<double>(keyName+"BBlkWallThick");
  bblkEndLen=Control.EvalVar<double>(keyName+"BBlkEndLen");
  bblkEndThick=Control.EvalVar<double>(keyName+"BBlkEndThick");

  insOutThick=Control.EvalVar<double>(keyName+"InsOutThick");
  insInThick=Control.EvalVar<double>(keyName+"InsInThick");
  insWidth=Control.EvalVar<double>(keyName+"InsWidth");
  insHeight=Control.EvalVar<double>(keyName+"InsHeight");              
  insRad=Control.EvalVar<double>(keyName+"InsRad");             

  dMat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  insMat=ModelSupport::EvalMat<int>(Control,keyName+"InsMat");

  cornerThick=Control.EvalVar<double>(keyName+"CornerThick");
  cornerWidth=Control.EvalVar<double>(keyName+"CornerWidth");
  
  return;
}
  
void
WaterDividers::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed compontent [front of target void vessel]
    - Y Down the beamline
  */
{
  ELog::RegMethod RegA("WaterDividers","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,0);
  return;
}

void
WaterDividers::createSurfaces(const PlateTarget& PT,
			      const attachSystem::FixedComp& Vessel)
  /*!
    Create All the surfaces
    \param PT :: Fixed component for target 
    \param Vessel :: Fixed component for vessel containing
  */
{
  ELog::RegMethod RegA("WaterDividers","createSurface");

  // VacVessel values [Back plate]: 
  SMap.addMatch(wIndex+2002,Vessel.getLinkSurf(2));
  const Geometry::Vec3D VesselPt=Vessel.getLinkPt(2);

  SMap.addMatch(wIndex+2005,Vessel.getLinkSurf(5));
  SMap.addMatch(wIndex+2006,Vessel.getLinkSurf(6));


  // FORWARD DIVIDER
  double PWidth,PLen;
  const Geometry::Vec3D PCent=
    PT.plateEdge(fblkConnect,PWidth,PLen);
  PLen/=2.0;
  // Target connected block
  ModelSupport::buildPlane(SMap,wIndex+1,PCent-Y*PLen,Y);
  ModelSupport::buildPlane(SMap,wIndex+2,PCent+Y*PLen,Y);
  ModelSupport::buildPlane(SMap,wIndex+3,PCent-X*PWidth,X);
  ModelSupport::buildPlane(SMap,wIndex+4,PCent+X*PWidth,X);
  ModelSupport::buildPlane(SMap,wIndex+5,Origin-Z*conHeight/2.0,Z);
  ModelSupport::buildPlane(SMap,wIndex+6,Origin+Z*conHeight/2.0,Z);
  // Outside of target connected block
  ModelSupport::buildPlane(SMap,wIndex+13,
			   PCent-X*(PWidth+fblkSize),X);
  ModelSupport::buildPlane(SMap,wIndex+14,
			   PCent+X*(PWidth+fblkSize),X);
  
  // snd block:
  ModelSupport::buildPlane(SMap,wIndex+21,
			   PCent+Y*(fblkSndStep-fblkSndLen/2.0),Y);
  ModelSupport::buildPlane(SMap,wIndex+22,
			   PCent+Y*(fblkSndStep+fblkSndLen/2.0),Y);

  ModelSupport::buildPlane
    (SMap,wIndex+23,PCent-X*(PWidth+fblkSndOut-fblkSndWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,wIndex+33,PCent-X*(PWidth+fblkSndOut+fblkSndWidth/2.0),X);

  ModelSupport::buildPlane
    (SMap,wIndex+24,PCent+X*(PWidth+fblkSndOut-fblkSndWidth/2.0),X);
  ModelSupport::buildPlane
    (SMap,wIndex+34,PCent+X*(PWidth+fblkSndOut+fblkSndWidth/2.0),X);

  // Connector [surf 47,49]:
  Geometry::Vec3D A,B;
  for(int lrSign=-1;lrSign<2;lrSign+=2)  // left/right sign
    {
      int index((lrSign<0) ? wIndex+47 : wIndex+57);
      for(int fbSign=-1;fbSign<2;fbSign+=2,index++)   // front/back
	{
	  A=PCent+X*(lrSign*(PWidth+fblkSize))+Y*(fbSign*PLen);
	  B=PCent+X*lrSign*(PWidth+fblkSndOut-(fbSign*fblkSndWidth/2.0))+
	    Y*(fblkSndStep-fblkSndLen/2.0);
	  ModelSupport::buildPlane(SMap,index,A,B,B+Z,X);
	}
    }

  // Long divider:
  ModelSupport::buildPlane
    (SMap,wIndex+63,PCent-X*(PWidth+fblkSndOut-fblkWallThick/2.0),X);
  ModelSupport::buildPlane
    (SMap,wIndex+73,PCent-X*(PWidth+fblkSndOut+fblkWallThick/2.0),X);

  ModelSupport::buildPlane
    (SMap,wIndex+64,PCent+X*(PWidth+fblkSndOut-fblkWallThick/2.0),X);
  ModelSupport::buildPlane
    (SMap,wIndex+74,PCent+X*(PWidth+fblkSndOut+fblkWallThick/2.0),X);
  
  // End block:
  ModelSupport::buildPlane(SMap,wIndex+81,VesselPt-Y*(fblkEndLen),Y);
  ModelSupport::buildPlane(SMap,wIndex+83,
			   PCent-X*(PWidth+fblkSndOut+fblkEndThick/2.0),X);
  ModelSupport::buildPlane(SMap,wIndex+84,
			   PCent-X*(PWidth+fblkSndOut-fblkEndThick/2.0),X);
  ModelSupport::buildPlane(SMap,wIndex+93,
			   PCent+X*(PWidth+fblkSndOut-fblkEndThick/2.0),X);
  ModelSupport::buildPlane(SMap,wIndex+94,
			   PCent+X*(PWidth+fblkSndOut+fblkEndThick/2.0),X);


  // MID DIVIDER
  double MWidth,MLen;
  const Geometry::Vec3D MCent=
    PT.plateEdge(mblkConnect,MWidth,MLen);
  MLen/=2.0;

  ModelSupport::buildPlane(SMap,wIndex+101,MCent-Y*MLen,Y);
  ModelSupport::buildPlane(SMap,wIndex+102,MCent+Y*MLen,Y);
  ModelSupport::buildPlane(SMap,wIndex+103,MCent-X*MWidth,X);
  ModelSupport::buildPlane(SMap,wIndex+104,MCent+X*MWidth,X);


  ModelSupport::buildPlane(SMap,wIndex+113,
			   MCent-X*(MWidth+mblkSize),X);
  ModelSupport::buildPlane(SMap,wIndex+114,
			   MCent+X*(MWidth+mblkSize),X);
  // Extension block
  ModelSupport::buildPlane(SMap,wIndex+122,
			   MCent+Y*(MLen+mblkExtLen),Y);
  ModelSupport::buildPlane(SMap,wIndex+123,
			   MCent-X*(MWidth+mblkSize-mblkExtWidth),X);
  ModelSupport::buildPlane(SMap,wIndex+124,
			   MCent+X*(MWidth+mblkSize-mblkExtWidth),X);

  // OuterCylinders:
  ModelSupport::buildCylinder(SMap,wIndex+137,
			      MCent-X*(MWidth+mblkSize-mblkOutRad)
			      -Y*(MLen-mblkOutRad),Z,mblkOutRad);
  ModelSupport::buildPlane(SMap,wIndex+132,
			   MCent-Y*(MLen-mblkOutRad),Y);
  ModelSupport::buildPlane(SMap,wIndex+133,
			   MCent-X*(MWidth+mblkSize-mblkOutRad),X);

  // InnerCylinders:
  ModelSupport::buildCylinder(SMap,wIndex+147,
		      MCent-X*(MWidth+mblkSize-mblkExtWidth-mblkOutRad)
			      +Y*(MLen+mblkInRad),Z,mblkInRad);
  ModelSupport::buildPlane(SMap,wIndex+142,
			   MCent+Y*(MLen+mblkInRad),Y);
  ModelSupport::buildPlane(SMap,wIndex+143,
		   MCent-X*(MWidth+mblkSize-mblkExtWidth-mblkOutRad),X);

  // OuterCylinders:
  ModelSupport::buildCylinder(SMap,wIndex+138,
			      MCent+X*(MWidth+mblkSize-mblkOutRad)
			      -Y*(MLen-mblkOutRad),Z,mblkOutRad);
  ModelSupport::buildPlane(SMap,wIndex+134,
			   MCent+X*(MWidth+mblkSize-mblkOutRad),X);

  // InnerCylinders:
  ModelSupport::buildCylinder(SMap,wIndex+148,
		      MCent+X*(MWidth+mblkSize-mblkExtWidth-mblkOutRad)
			      +Y*(MLen+mblkInRad),Z,mblkInRad);
  ModelSupport::buildPlane(SMap,wIndex+144,
		   MCent+X*(MWidth+mblkSize-mblkExtWidth-mblkOutRad),X);

  // Long divider:
  ModelSupport::buildPlane
    (SMap,wIndex+163,MCent-X*(MWidth+mblkSize-mblkExtWidth/2.0
			      -mblkWallThick/2.0),X);
  ModelSupport::buildPlane
    (SMap,wIndex+173,MCent-X*(MWidth+mblkSize-mblkExtWidth/2.0
			      +mblkWallThick/2.0),X);

  ModelSupport::buildPlane
    (SMap,wIndex+164,MCent+X*(MWidth+mblkSize-mblkExtWidth/2.0
			      -mblkWallThick/2.0),X);
  ModelSupport::buildPlane
    (SMap,wIndex+174,MCent+X*(MWidth+mblkSize-mblkExtWidth/2.0
			      +mblkWallThick/2.0),X);

  // End block:
  ModelSupport::buildPlane(SMap,wIndex+181,VesselPt-Y*(mblkEndLen),Y);
  ModelSupport::buildPlane(SMap,wIndex+183,
			   MCent-X*(MWidth+mblkSize-mblkExtWidth/2.0
				    +mblkEndThick/2.0),X);
  ModelSupport::buildPlane(SMap,wIndex+184,
			   MCent-X*(MWidth+mblkSize-mblkExtWidth/2.0
				    -mblkEndThick/2.0),X);
  ModelSupport::buildPlane(SMap,wIndex+193,
			   MCent+X*(MWidth+mblkSize-mblkExtWidth/2.0
				    -mblkEndThick/2.0),X);
  ModelSupport::buildPlane(SMap,wIndex+194,
			   MCent+X*(MWidth+mblkSize-mblkExtWidth/2.0
				    +mblkEndThick/2.0),X);

  // ------------------------------------------------------------
  //      BACK DIVIDER
  // ------------------------------------------------------------


  double BWidth,BLen;
  double BConst;
  const Geometry::Vec3D BCent=
    PT.plateEdge(bblkConnect,BWidth,BLen);
  BLen=1.1;
  BConst=0.2;

  ModelSupport::buildPlane(SMap,wIndex+201,BCent-Y*BConst-Y*BLen,Y);
  ModelSupport::buildPlane(SMap,wIndex+202,BCent-Y*BConst+Y*BLen,Y);
  ModelSupport::buildPlane(SMap,wIndex+203,BCent-Y*BConst-X*BWidth,X);
  ModelSupport::buildPlane(SMap,wIndex+204,BCent-Y*BConst+X*BWidth,X);


  ModelSupport::buildPlane(SMap,wIndex+213,
			   BCent-Y*BConst-X*(BWidth+bblkSize),X);
  ModelSupport::buildPlane(SMap,wIndex+214,
			   BCent-Y*BConst+X*(BWidth+bblkSize),X);
  // Extension block
  ModelSupport::buildPlane(SMap,wIndex+222,
			   BCent-Y*BConst+Y*(BLen+bblkExtLen),Y);
  ModelSupport::buildPlane(SMap,wIndex+223,
			   BCent-Y*BConst-X*(BWidth+bblkSize-bblkExtWidth),X);
  ModelSupport::buildPlane(SMap,wIndex+224,
			   BCent-Y*BConst+X*(BWidth+bblkSize-bblkExtWidth),X);

  // OuterCylinders:
  ModelSupport::buildCylinder(SMap,wIndex+237,
			      BCent-Y*BConst-X*(BWidth+bblkSize-bblkOutRad)
			      -Y*(BLen-bblkOutRad),Z,bblkOutRad);
  ModelSupport::buildPlane(SMap,wIndex+232,
			   BCent-Y*BConst-Y*(BLen-bblkOutRad),Y);
  ModelSupport::buildPlane(SMap,wIndex+233,
			   BCent-Y*BConst-X*(BWidth+bblkSize-bblkOutRad),X);

  // InnerCylinders:
  ModelSupport::buildCylinder(SMap,wIndex+247,
		      BCent-Y*BConst-X*(BWidth+bblkSize-bblkExtWidth-bblkOutRad)
			      +Y*(BLen+bblkInRad),Z,bblkInRad);
  ModelSupport::buildPlane(SMap,wIndex+242,
			   BCent-Y*BConst+Y*(BLen+bblkInRad),Y);
  ModelSupport::buildPlane(SMap,wIndex+243,
		   BCent-Y*BConst-X*(BWidth+bblkSize-bblkExtWidth-bblkOutRad),X);

  // OuterCylinders:
  ModelSupport::buildCylinder(SMap,wIndex+238,
			      BCent-Y*BConst+X*(BWidth+bblkSize-bblkOutRad)
			      -Y*(BLen-bblkOutRad),Z,bblkOutRad);
  ModelSupport::buildPlane(SMap,wIndex+234,
			   BCent-Y*BConst+X*(BWidth+bblkSize-bblkOutRad),X);

  // InnerCylinders:
  ModelSupport::buildCylinder(SMap,wIndex+248,
		      BCent-Y*BConst+X*(BWidth+bblkSize-bblkExtWidth-bblkOutRad)
			      +Y*(BLen+bblkInRad),Z,bblkInRad);
  ModelSupport::buildPlane(SMap,wIndex+244,
		   BCent-Y*BConst+X*(BWidth+bblkSize-bblkExtWidth-bblkOutRad),X);

  // Long divider:
  ModelSupport::buildPlane
    (SMap,wIndex+263,BCent-Y*BConst-X*(BWidth+bblkSize-bblkExtWidth/2.0
			      -bblkWallThick/2.0),X);
  ModelSupport::buildPlane
    (SMap,wIndex+273,BCent-Y*BConst-X*(BWidth+bblkSize-bblkExtWidth/2.0
			      +bblkWallThick/2.0),X);

  ModelSupport::buildPlane
    (SMap,wIndex+264,BCent-Y*BConst+X*(BWidth+bblkSize-bblkExtWidth/2.0
			      -bblkWallThick/2.0),X);
  ModelSupport::buildPlane
    (SMap,wIndex+274,BCent-Y*BConst+X*(BWidth+bblkSize-bblkExtWidth/2.0
			      +bblkWallThick/2.0),X);

  // End block:
  // ModelSupport::buildPlane(SMap,wIndex+281,VesselPt-Y*(bblkEndLen),Y);
  ModelSupport::buildPlane(SMap,wIndex+283,
			   BCent-Y*BConst-X*(BWidth+bblkSize-bblkExtWidth/2.0
				    +bblkEndThick/2.0),X);
  ModelSupport::buildPlane(SMap,wIndex+284,
			   BCent-Y*BConst-X*(BWidth+bblkSize-bblkExtWidth/2.0
				    -bblkEndThick/2.0),X);
  ModelSupport::buildPlane(SMap,wIndex+293,
			   BCent-Y*BConst+X*(BWidth+bblkSize-bblkExtWidth/2.0
				    -bblkEndThick/2.0),X);
  ModelSupport::buildPlane(SMap,wIndex+294,
			   BCent-Y*BConst+X*(BWidth+bblkSize-bblkExtWidth/2.0
				    +bblkEndThick/2.0),X);

  // Insert:

  ModelSupport::buildPlane(SMap,wIndex+301,VesselPt-Y*insOutThick,Y);
  ModelSupport::buildPlane(SMap,wIndex+303,VesselPt-X*insWidth/2.0,X);
  ModelSupport::buildPlane(SMap,wIndex+304,VesselPt+X*insWidth/2.0,X);
  ModelSupport::buildPlane(SMap,wIndex+305,VesselPt-Z*insHeight/2.0,Z);
  ModelSupport::buildPlane(SMap,wIndex+306,VesselPt+Z*insHeight/2.0,Z);

  ModelSupport::buildPlane(SMap,wIndex+401,VesselPt-Y*insInThick,Y);
  ModelSupport::buildCylinder(SMap,wIndex+407,Origin,Y,insRad);

  // Corners:

  ModelSupport::buildPlane(SMap,wIndex+501,VesselPt-Y*cornerThick,Y);
  ModelSupport::buildPlane(SMap,wIndex+503,VesselPt-X*cornerWidth/2.0,X);
  ModelSupport::buildPlane(SMap,wIndex+504,VesselPt+X*cornerWidth/2.0,X);

  return;
}

void
WaterDividers::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("WaterDividers","createObjects");

  std::string Out;
  // Front divider:
  Out=ModelSupport::getComposite(SMap,wIndex,
				 "1 47 33 (-2:-48:-23) -3 -22 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterSurf(Out);

  Out=ModelSupport::getComposite(SMap,wIndex,
				 "1 -57 -34 (-2:58:24) 4 -22 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterUnionSurf(Out);

  // Thin divider
  Out=ModelSupport::getComposite(SMap,wIndex,"22 -81 -63 73 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,wIndex,"22 -81 64 -74 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterUnionSurf(Out);

  // End block on left
  Out=ModelSupport::getComposite(SMap,wIndex,"81 -2002 83 -84 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterUnionSurf(Out);
  // End block on right
  Out=ModelSupport::getComposite(SMap,wIndex,"81 -2002 93 -94 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterUnionSurf(Out);

  //---------------------
  // Mid section divider:
  //---------------------

  Out=ModelSupport::getComposite(SMap,wIndex,
				 "101  (-137:132:133 ) "
				 "(-102 : -123 : (147 -142 -143)) "
				 "-122 -103 113 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterUnionSurf(Out);

  // Mid section divider:
  Out=ModelSupport::getComposite(SMap,wIndex,
				 "101  (-138:132:-134 ) "
				 "(-102 : 124 : (148 -142 144)) "
				 "-122 104 -114 5 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterUnionSurf(Out);
  
  // Mid thin dividers:
  Out=ModelSupport::getComposite(SMap,wIndex,"122 -181 -163 173 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,wIndex,"122 -181 164 -174 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterUnionSurf(Out);

  // End block on left
  Out=ModelSupport::getComposite(SMap,wIndex,"181 -2002 183 -184 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterUnionSurf(Out);
  // End block on right
  Out=ModelSupport::getComposite(SMap,wIndex,"181 -2002 193 -194 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterUnionSurf(Out);

  //---------------------------------------------------------------
  // End section divider:
  //---------------------------------------------------------------

   Out=ModelSupport::getComposite(SMap,wIndex,
  				 "201  (-237:232:233 ) "
  				 "(-202 : -223 : (247 -242 -243)) "
  				 "-222 -203 213 5 -6 ");
   System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
   addOuterUnionSurf(Out);

  // Back section divider:
   Out=ModelSupport::getComposite(SMap,wIndex,
  				 "201  (-238:232:-234 ) "
    			 "(-202 : 224 : (248 -242 244)) "
  				 "-222 204 -214 5 -6 ");
   System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
   addOuterUnionSurf(Out);
  
  // End block on left
  Out=ModelSupport::getComposite(SMap,wIndex,"222 -2002 283 -284 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterUnionSurf(Out);
  // End block on right
  Out=ModelSupport::getComposite(SMap,wIndex,"222 -2002 293 -294 5 -6");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterUnionSurf(Out);

  // Insert
 
  Out=ModelSupport::getComposite(SMap,wIndex,"301 -2002 303 -304 305 -306");
  addOuterUnionSurf(Out);
  Out+=ModelSupport::getComposite(SMap,wIndex,"(-401:2002:407) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));  
  
  Out=ModelSupport::getComposite(SMap,wIndex,"401 -2002 -407");
  System.addCell(MonteCarlo::Qhull(cellIndex++,insMat,0.0,Out));
  addOuterUnionSurf(Out);

  // Corners
  Out=ModelSupport::getComposite(SMap,wIndex,"501 -2002 -503 284 2005 -2006");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterUnionSurf(Out);

  Out=ModelSupport::getComposite(SMap,wIndex,"501 -2002 504 -293 2005 -2006");
  System.addCell(MonteCarlo::Qhull(cellIndex++,dMat,0.0,Out));
  addOuterUnionSurf(Out);
  
  return;
}

void
WaterDividers::createLinks()
  /*!
    Creates a full attachment set
  */
{
  return;
}

void
WaterDividers::createAll(Simulation& System,
			 const PlateTarget& PT,
			 const attachSystem::FixedComp& Vessel)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param PT :: Plate target
  */
{
  ELog::RegMethod RegA("WaterDividers","createAll");
  populate(System.getDataBase());

  createUnitVector(PT);
  createSurfaces(PT,Vessel);
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
