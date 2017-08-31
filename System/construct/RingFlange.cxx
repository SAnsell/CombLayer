/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   construct/RingFlange.cxx
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
#include "SurfMap.h"
#include "RingSeal.h"
#include "RingFlange.h"


namespace constructSystem
{

RingFlange::RingFlange(const std::string& Key) :
  RingSeal(Key),nBolts(0),rotAngleOffset(0.0)
  /*!
    Default constructor
    \param Key :: Key name for variables
  */
{}

RingFlange::RingFlange(const RingFlange& A) : 
  RingSeal(A),
  nBolts(A.nBolts),rotAngleOffset(A.rotAngleOffset),
  boltRadius(A.boltRadius),boltCentDist(A.boltCentDist),
  boltMat(A.boltMat)
  /*!
    Copy constructor
    \param A :: RingFlange to copy
  */
{}

RingFlange&
RingFlange::operator=(const RingFlange& A)
  /*!
    Assignment operator
    \param A :: RingFlange to copy
    \return *this
  */
{
  if (this!=&A)
    {
      RingSeal::operator=(A);
      nBolts=A.nBolts;
      rotAngleOffset=A.rotAngleOffset;
      boltRadius=A.boltRadius;
      boltCentDist=A.boltCentDist;
      boltMat=A.boltMat;
    }
  return *this;
}

void
RingFlange::populate(const FuncDataBase& Control)
  /*!
    Sets the size of the object
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("RingFlange","populate");

  RingSeal::populate(Control);

  nBolts=Control.EvalDefVar<size_t>(keyName+"NBolts",0);

  if (nBolts)
    {
      rotAngleOffset=Control.EvalDefVar<double>(keyName+"RotAngleOffset",0.0);
      boltRadius=Control.EvalVar<double>(keyName+"BoltRadius");

      boltCentDist=Control.EvalDefVar<double>(keyName+"BoltCentDist",
					      radius+deltaRad/2.0);
      boltMat=ModelSupport::EvalMat<int>(Control,keyName+"BoltMat");
    }
  // Window??

  // 0 : NONE : -1 MAKE WINDOW only cut : 1 make window cut to edge
  windowFlag=Control.EvalDefVar<int>(keyName+"WindowFlag",0);

  if (windowFlag)
    {
      windowThick=Control.EvalVar<double>(keyName+"WindowThick");
      windowStep=Control.EvalDefVar<double>(keyName+"WindowStep",0.0);	
      windowMat=ModelSupport::EvalMat<int>(Control,keyName+"WindowMat");
    }  
  
  return;
}

void
RingFlange::insertBolt(Simulation& System,
		       const double angle,
		       const double boltSolidAngle,
		       const std::string& exclude) const
  /*!
    Insert the bolt in to the segmented (or not)
    ring seal
    \param System :: Simluation to use						
    \param angle :: angle of bolt
    \param boltSolidAngle :: solid angle cut out by bolt
    \param exclude :: excluded  
   */
{
  ELog::RegMethod RegA("RingFlange","insertBolt");

  if (!NSection)
    {
      const int CN=getCell("Ring",0);
      MonteCarlo::Qhull* segComp=System.findQhull(CN);
      if (!segComp)
	throw ColErr::InContainerError<int>
	  (CN,"RingCell not found in object"+keyName);
      segComp->addSurfString(exclude);
      return;
    }
  
  const double angleR=360.0/static_cast<double>(NSection);
  double aVal(angle-deltaAngle);
  if (aVal<0.0) aVal+=360.0;
  const size_t index=static_cast<size_t>(aVal/angleR);
  const double xAngle=angle-(static_cast<double>(index)*angleR);
  std::vector<int> cellN;
  // before:
  
  if (xAngle+angleR/2.0>boltSolidAngle)
    {
      const int CN=getCell("Ring",(NSection+index-1) % NSection);
      cellN.push_back(CN);
    }
  // after:
  if (angleR/2.0-xAngle>boltSolidAngle)
    {
      const int CN=getCell("Ring",(index+1) % NSection);
      cellN.push_back(CN);
    }
  const int CN=getCell("Ring",index);
  cellN.push_back(CN);

  for(const int CN : cellN)
    {
      MonteCarlo::Qhull* segComp=System.findQhull(CN);
      if (!segComp) 
	throw ColErr::InContainerError<int>
	  (CN,"RingCell not found in object"+keyName);
      segComp->addSurfString(exclude);
    }  
  return;
}

void
RingFlange::addWindow(Simulation& System)
  /*!
    Add the window
    \param System :: Simualation
  */
{
  ELog::RegMethod RegA("RingFlange","addWindow");


  if (windowFlag)
    {
      // Add links: EXTRA:
      const size_t NLink=NConnect();
      if (NLink<12) setNConnect(12);

      std::string Out;
      int windowIndex(ringIndex+2000);
      ModelSupport::buildPlane(SMap,windowIndex+1,
			       Origin+Y*(windowStep-windowThick/2.0),Y);
      ModelSupport::buildPlane(SMap,windowIndex+2,
			       Origin+Y*(windowStep+windowThick/2.0),Y);

      // Create window
      const std::string radSurf=innerStruct.complement().display();
      Out=ModelSupport::getComposite(SMap,windowIndex,"1 -2  ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,windowMat,0.0,Out+radSurf));
      addCell("window",cellIndex-1);
      
      if (windowFlag>0)
	{
	  Out=ModelSupport::getComposite(SMap,windowIndex,ringIndex," 2 -2M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+radSurf));
	  addCell("window",cellIndex-1);
	  Out=ModelSupport::getComposite(SMap,windowIndex,ringIndex," -1 1M ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+radSurf));
	  addCell("window",cellIndex-1);
	  Out=ModelSupport::getComposite(SMap,ringIndex," 1 -2 ");

	}
      // exclude:
      addOuterUnionSurf(Out+radSurf);

      // Add links:	  
      setConnect(6,Origin+Y*(windowStep-windowThick/2.0),-Y);
      setConnect(7,Origin+Y*(windowStep+windowThick/2.0),Y);
      setLinkSurf(6,-SMap.realSurf(windowIndex+1));
      setLinkSurf(7,SMap.realSurf(windowIndex+2));



    }
  
  return;
}
  
void
RingFlange::addBolts(Simulation& System) 
  /*!
    Add the bolts
    \param System :: Simulation for object
  */
{
  ELog::RegMethod RegA("RingFlange","addBolts");

  if (nBolts>0)
    {
      std::string Out;
      const double boltSolidAngle
	((180.0/M_PI)*std::atan((boltRadius+Geometry::zeroTol)/boltCentDist));
      const std::string FBStr=
	ModelSupport::getComposite(SMap,ringIndex," 1 -2 ");
      
      const double angleBR=360.0/static_cast<double>(nBolts);
      Geometry::Vec3D BAxis(Z*boltCentDist);
      const Geometry::Quaternion QStart=
	Geometry::Quaternion::calcQRotDeg(rotAngleOffset,Y);

      const Geometry::Quaternion QBolt=
	Geometry::Quaternion::calcQRotDeg(angleBR,Y);
      
      QStart.rotate(BAxis);
      double angleBCent(rotAngleOffset);  // angle of centre of bolt 
      
      int boltIndex(ringIndex+1000);
      for(size_t i=0;i<nBolts;i++)
        {
	  const Geometry::Vec3D boltC(Origin+BAxis);
          ModelSupport::buildCylinder(SMap,boltIndex+7,boltC,Y,boltRadius);
          QBolt.rotate(BAxis);
	  
	  Out=ModelSupport::getComposite(SMap,boltIndex," -7 ");
	  System.addCell(MonteCarlo::Qhull(cellIndex++,boltMat,0.0,Out+FBStr));
          addCell("Bolts",cellIndex-1);
	  // exclude:
	  Out=ModelSupport::getComposite(SMap,boltIndex," 7 ");
	  insertBolt(System,angleBCent,boltSolidAngle,Out);
	  
	  angleBCent+=angleBR;
	  boltIndex+=10; // AT END
	}
    }
  return;
}
  
void
RingFlange::createAll(Simulation& System,
                     const attachSystem::FixedComp& FC,
                     const long int sideIndex)
  /*!
    Generic function to create everything
    \param System :: Simulation 
    \param FC :: Fixed component to set axis etc
    \param sideIndex :: position of linkpoint
  */
{
  ELog::RegMethod RegA("RingFlange","createAllNoPopulate");

  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  addBolts(System);
  addWindow(System);
  insertObjects(System);
  return;
}

  
}  // NAMESPACE constructSystem
