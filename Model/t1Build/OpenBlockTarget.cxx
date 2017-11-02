/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Build/OpenBlockTarget.cxx
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
#include "stringCombine.h"
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
#include "BeamWindow.h"
#include "ProtonVoid.h"
#include "TargetBase.h"
#include "OpenBlockTarget.h"

namespace ts1System
{

OpenBlockTarget::OpenBlockTarget(const std::string& Key)  :
  TargetBase(Key,6),
  ptIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(ptIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

OpenBlockTarget::OpenBlockTarget(const OpenBlockTarget& A) : 
  constructSystem::TargetBase(A),
  ptIndex(A.ptIndex),cellIndex(A.cellIndex),frontPlate(A.frontPlate),
  backPlate(A.backPlate),
  height(A.height),width(A.width),
  nBlock(A.nBlock),tBlock(A.tBlock),tVoid(A.tVoid),
  WWidth(A.WWidth),WHeight(A.WHeight),WVoidHeight(A.WVoidHeight),
  WVoidWidth(A.WVoidWidth),taThick(A.taThick),waterThick(A.waterThick),
  pressThick(A.pressThick),taMat(A.taMat),wMat(A.wMat),
  waterMat(A.waterMat),pressMat(A.pressMat)
  /*!
    Copy constructor
    \param A :: OpenBlockTarget to copy
  */
{}

OpenBlockTarget&
OpenBlockTarget::operator=(const OpenBlockTarget& A)
  /*!
    Assignment operator
    \param A :: OpenBlockTarget to copy
    \return *this
  */
{
  if (this!=&A)
    {
      constructSystem::TargetBase::operator=(A);
      cellIndex=A.cellIndex;
      frontPlate=A.frontPlate;
      backPlate=A.backPlate;
      height=A.height;
      width=A.width;
      nBlock=A.nBlock;
      tBlock=A.tBlock;
      tVoid=A.tVoid;
      WWidth=A.WWidth;
      WHeight=A.WHeight;
      WVoidHeight=A.WVoidHeight;
      WVoidWidth=A.WVoidWidth;
      taThick=A.taThick;
      waterThick=A.waterThick;
      pressThick=A.pressThick;
      taMat=A.taMat;
      wMat=A.wMat;
      waterMat=A.waterMat;
      pressMat=A.pressMat;
    }
  return *this;
}

OpenBlockTarget*
OpenBlockTarget::clone() const
  /*!
    Clone function
    \return new (this)
   */
{
  return new OpenBlockTarget(*this);
}


OpenBlockTarget::~OpenBlockTarget() 
 /*!
   Destructor
 */
{}

void
OpenBlockTarget::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Database of variables
 */
{
  ELog::RegMethod RegA("OpenBlockTarget","populate");

  attachSystem::FixedOffset::populate(Control);

  // Global values:
  height=Control.EvalVar<double>(keyName+"Height");
  width=Control.EvalVar<double>(keyName+"Width");

  // Blocks:
  nBlock=Control.EvalVar<size_t>(keyName+"NBlocks");
  double T,V;
  for(size_t i=0;i<nBlock;i++)
    {
      const std::string numStr=StrFunc::makeString(i);
      T=Control.EvalVar<double>(keyName+"Thick"+numStr);
      V=Control.EvalDefVar<double>(keyName+"Void"+numStr,0.0);
      tBlock.push_back(T);
      tVoid.push_back(V);
    }
  
  // Master Ta dimensions:
  taThick=Control.EvalVar<double>(keyName+"TaThick");
  waterThick=Control.EvalVar<double>(keyName+"WaterThick");

  // Master W Dimensions:
  WWidth=Control.EvalVar<double>(keyName+"WWidth");
  WHeight=Control.EvalVar<double>(keyName+"WHeight");  

  // Void if present
  WVoidWidth=Control.EvalVar<double>(keyName+"WVoidWidth");
  WVoidHeight=Control.EvalVar<double>(keyName+"WVoidHeight");  

  pressThick=Control.EvalVar<double>(keyName+"PressThick");

  pressMat=ModelSupport::EvalMat<int>(Control,keyName+"PressMat");
  taMat=ModelSupport::EvalMat<int>(Control,keyName+"TaMat");
  wMat=ModelSupport::EvalMat<int>(Control,keyName+"WMat");
  waterMat=ModelSupport::EvalMat<int>(Control,keyName+"WaterMat");

  return;
}
  
void
OpenBlockTarget::createUnitVector(const attachSystem::FixedComp& FC,
				  const long int indexPt)
  /*!
    Create the unit vectors
    \param FC :: Fixed compontent [front of target void vessel]
    - Y Down the beamline
  */
{
  ELog::RegMethod RegA("OpenBlockTarget","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,indexPt);
  applyOffset();
  
  return;
}

void
OpenBlockTarget::createSurfaces()
  /*!
    Create All the surfaces
    \param FC :: Fixed component for surface
  */
{
  ELog::RegMethod RegA("OpenBlockTarget","createSurface");

  // Create Outer box:
  
  ModelSupport::buildPlane(SMap,ptIndex+3,Origin-X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,ptIndex+4,Origin+X*(width/2.0),X);
  ModelSupport::buildPlane(SMap,ptIndex+5,Origin-Z*(height/2.0),Z);
  ModelSupport::buildPlane(SMap,ptIndex+6,Origin+Z*(height/2.0),Z);

  // Pressure layer
  ModelSupport::buildPlane(SMap,ptIndex+13,Origin-
			   X*(width/2.0+pressThick),X);
  ModelSupport::buildPlane(SMap,ptIndex+14,Origin+
			   X*(width/2.0+pressThick),X);
  ModelSupport::buildPlane(SMap,ptIndex+15,Origin-
			   Z*(height/2.0+pressThick),Z);
  ModelSupport::buildPlane(SMap,ptIndex+16,Origin+
			   Z*(height/2.0+pressThick),Z);
  
  // This could be a front window thickness?
  ModelSupport::buildPlane(SMap,ptIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,ptIndex+11,Origin-Y*pressThick,Y);


  // Cuts for W Outer Plates
  ModelSupport::buildPlane(SMap,ptIndex+103,Origin-X*(WWidth/2.0),X);
  ModelSupport::buildPlane(SMap,ptIndex+104,Origin+X*(WWidth/2.0),X);
  ModelSupport::buildPlane(SMap,ptIndex+105,Origin-Z*(WHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,ptIndex+106,Origin+Z*(WHeight/2.0),Z);

  // Cuts for Ta Outer Plates
  ModelSupport::buildPlane(SMap,ptIndex+203,Origin-X*(WWidth/2.0+taThick),X);
  ModelSupport::buildPlane(SMap,ptIndex+204,Origin+X*(WWidth/2.0+taThick),X);
  ModelSupport::buildPlane(SMap,ptIndex+205,Origin-Z*(WHeight/2.0+taThick),Z);
  ModelSupport::buildPlane(SMap,ptIndex+206,Origin+Z*(WHeight/2.0+taThick),Z);

  // Cuts for voids:
  ModelSupport::buildPlane(SMap,ptIndex+303,Origin-X*(WVoidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,ptIndex+304,Origin+X*(WVoidWidth/2.0),X);
  ModelSupport::buildPlane(SMap,ptIndex+305,Origin-Z*(WVoidHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,ptIndex+306,Origin+Z*(WVoidHeight/2.0),Z);
    
  double tStep(waterThick);
  int PT(ptIndex+1000);
  for(size_t i=0;i<nBlock;i++)
    {
      ModelSupport::buildPlane(SMap,PT+1,Origin+Y*tStep,Y);  // water : Ta
      tStep+=taThick;
      ModelSupport::buildPlane(SMap,PT+11,Origin+Y*tStep,Y);  // Ta : W
      if (tVoid[i]>Geometry::zeroTol)
	{
	  tStep+=(tBlock[i]-tVoid[i])/2.0;      
	  ModelSupport::buildPlane(SMap,PT+21,Origin+Y*tStep,Y);  // W : Void
	  tStep+=tVoid[i];
	  ModelSupport::buildPlane(SMap,PT+22,Origin+Y*tStep,Y);  // Void : W
	  tStep+=(tBlock[i]-tVoid[i])/2.0;
	}
      else
	tStep+=tBlock[i];

      ModelSupport::buildPlane(SMap,PT+12,Origin+Y*tStep,Y);  // W:Ta 
      tStep+=taThick;
      ModelSupport::buildPlane(SMap,PT+2,Origin+Y*tStep,Y);  // Ta: Water 
      tStep+=waterThick;
      PT+=100;
    }      

  // Close target at back
  ModelSupport::buildPlane(SMap,ptIndex+2,Origin+Y*tStep,Y);
  ModelSupport::buildPlane(SMap,ptIndex+12,Origin+Y*(tStep+pressThick),Y);

  FixedComp::setLinkSurf(1,SMap.realSurf(ptIndex+12));
  FixedComp::setConnect(1,Origin+Y*(tStep+pressThick),Y);

  return;
}

void
OpenBlockTarget::createObjects(Simulation& System)
  /*!
    Creates all the blocks/pressure vessel
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("OpenBlockTarget","createObjects");
  
  std::string Out;
  
  // Create the pressure vessel:
  Out=ModelSupport::getComposite(SMap,ptIndex,"11 -12 13 -14 15 -16 "
				 " ( -1 : 2 : -3 : 4 : -5 : 6 ) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,pressMat,0.0, Out));
  // create surround water
  Out=ModelSupport::getComposite(SMap,ptIndex,"1 -2 3 -4 5 -6 "
				 " ( -203 : 204 : -205 : 206 ) ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0, Out));


  const std::string H2OEdge=
    ModelSupport::getComposite(SMap,ptIndex," 203 -204 205 -206 ");

  // Make plates:
  int PT(ptIndex+1000);
  std::string preLayer=
    ModelSupport::getComposite(SMap,ptIndex," 1 ");
  for(size_t i=0;i<nBlock;i++)
    {
      // Front water
      Out=ModelSupport::getComposite(SMap,PT," -1 ")+H2OEdge+preLayer;
      System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));
      // Ta surround
      Out=ModelSupport::getComposite
	(SMap,ptIndex,PT,"1M -2M ( -11M:12M:-103:104:-105:106) ")+H2OEdge;
      System.addCell(MonteCarlo::Qhull(cellIndex++,taMat,0.0,Out));

      // Void case
      if (tVoid[i]>Geometry::zeroTol)
	{
	  // inner void
	  Out=ModelSupport::getComposite
	    (SMap,ptIndex,PT,"21M -22M 303 -304 305 -306");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));

	  // W with void hole
	  Out=ModelSupport::getComposite
	    (SMap,ptIndex,PT," 11M -12M 103 -104 105 -106 "
	     " (-21M : 22M : -303 : 304 : -305 : 306)");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));
	}
      else
	{
	  // W with no hole 	
	  Out=ModelSupport::getComposite
	    (SMap,ptIndex,PT,"11M -12M 103 -104 105 -106 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,wMat,0.0,Out));
	}
      preLayer=ModelSupport::getComposite(SMap,PT," 2 ");
      PT+=100;
    }
  // Last water plate
  Out=ModelSupport::getComposite(SMap,ptIndex," -2 ")+H2OEdge+preLayer;
  System.addCell(MonteCarlo::Qhull(cellIndex++,waterMat,0.0,Out));


  Out=ModelSupport::getComposite(SMap,ptIndex,"11 -12 13 -14 15 -16 ");
  addOuterSurf(Out);


  return;
}

Geometry::Vec3D
OpenBlockTarget::plateEdge(const size_t plateN,double& W,double& L) const
  /*!
    Function to obtain the plate centre point and width and length
    \param plateN :: Index number of plate [0 to 11]
    \param W :: output for the plate width
    \param L :: output for the plate length [total inc layers]
    \return plate centre point
  */
{
  ELog::RegMethod RegA("OpenBlockTarget","plateEdge");
  if (plateN>nBlock)
    throw ColErr::IndexError<size_t>(plateN,nBlock,"plate number+1");
  
  W=width;
  Geometry::Vec3D FPt(Origin);
  FPt += Y*(static_cast<double>(plateN+1) * waterThick+
	    static_cast<double>(2*plateN+1) * taThick);

  for(size_t i=1; i<= plateN;i++)
    FPt+=Y*tBlock[i-1];
  
  FPt+=Y*(tBlock[plateN]/2.0);
  L=tBlock[plateN]+2*taThick;

  return FPt;
}

void
OpenBlockTarget::createLinks()
  /*!
    Creates a full attachment set
  */
{
  ELog::RegMethod RegA("OpenBlockTarget","createLinks");


  FixedComp::setConnect(0,Origin-Y*pressThick,-Y);
  // 1 set in surfaces
  FixedComp::setConnect(2,Origin-X*(width/2.0+pressThick),-X);
  FixedComp::setConnect(3,Origin+X*(width/2.0+pressThick),X);
  FixedComp::setConnect(4,Origin-Z*(height/2.0+pressThick),-Z);
  FixedComp::setConnect(5,Origin+Z*(height/2.0+pressThick),Z);

  // Set Connect surfaces:
  FixedComp::setLinkSurf(0,-SMap.realSurf(ptIndex+11));
  FixedComp::setLinkSurf(1,SMap.realSurf(ptIndex+12));
  FixedComp::setLinkSurf(2,-SMap.realSurf(ptIndex+13));
  FixedComp::setLinkSurf(3,SMap.realSurf(ptIndex+14));
  FixedComp::setLinkSurf(4,-SMap.realSurf(ptIndex+15));
  FixedComp::setLinkSurf(5,SMap.realSurf(ptIndex+16));

  return;
}


double
OpenBlockTarget::getTargetLength() const
  /*!
    Calculate the target length
    \return target length [including water]
  */
{
  double TL=
    std::accumulate(tBlock.begin(),tBlock.end(),0.0,std::plus<double>());
  TL+=
    std::accumulate(tVoid.begin(),tVoid.end(),0.0,std::plus<double>());
  
  return TL+2.0*static_cast<double>(nBlock)*(taThick+waterThick);
}

void 
OpenBlockTarget::addProtonLine(Simulation& System,
			 const attachSystem::FixedComp& refFC,
			 const long int index)
  /*!
    Add a proton void cell
    \param System :: Simualation
    \param refFC :: reflector edge
    \param index :: Index of the proton cutting surface [6 typically (-7)]
   */
{
  ELog::RegMethod RegA("OpenBlockTarget","addProtonLine");

  // 0 ::  front face of target
  PLine->createAll(System,*this,0,refFC,index);
  createBeamWindow(System,1);
  System.populateCells();
  System.createObjSurfMap();
  return;
}



void
OpenBlockTarget::createAll(Simulation& System,
			   const attachSystem::FixedComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("OpenBlockTarget","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,0);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       

  return;
}

  
}  // NAMESPACE ts1System
