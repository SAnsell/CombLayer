/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   chip/ChipIRShutterFlat.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <iterator>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

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
#include "PointOperation.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "shutterBlock.h"
#include "SimProcess.h"
#include "SurInter.h"
#include "Simulation.h"
#include "insertInfo.h"
#include "insertBaseInfo.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "InsertComp.h"
#include "GeneralShutter.h"
#include "chipDataStore.h"
#include "ChipIRShutterFlat.h"


namespace shutterSystem
{

ChipIRShutterFlat::ChipIRShutterFlat(const size_t ID,const std::string& K,
			     const std::string& CKey)  : 
  GeneralShutter(ID,K),chipKey(CKey)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param ID :: Index number of the shutter
    \param K :: Keyname to extract
    \param CKey :: Keyname of specific chipShutter items
  */
{}

ChipIRShutterFlat::ChipIRShutterFlat(const ChipIRShutterFlat& A) : 
  GeneralShutter(A),
  chipKey(A.chipKey),CInfo(A.CInfo),forwardStep(A.forwardStep),
  midStep(A.midStep),linerStartXZ(A.linerStartXZ),linerEndXZ(A.linerEndXZ),
  insertStep(A.insertStep),shineMat(A.shineMat)
  /*!
    Copy constructor
    \param A :: ChipIRShutterFlat to copy
  */
{}

ChipIRShutterFlat&
ChipIRShutterFlat::operator=(const ChipIRShutterFlat& A)
  /*!
    Assignment operator
    \param A :: ChipIRShutterFlat to copy
    \return *this
  */
{
  if (this!=&A)
    {
      GeneralShutter::operator=(A);
      chipKey=A.chipKey;
      CInfo=A.CInfo;
      forwardStep=A.forwardStep;
      midStep=A.midStep;
      linerStartXZ=A.linerStartXZ;
      linerEndXZ=A.linerEndXZ;
      insertStep=A.insertStep;
      shineMat=A.shineMat;
    }
  return *this;
}

ChipIRShutterFlat::~ChipIRShutterFlat() 
  /*!
    Destructor
  */
{}

void
ChipIRShutterFlat::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  GeneralShutter::populate(System);
  const FuncDataBase& Control=System.getDataBase();

  forwardStep=Control.EvalVar<double>(chipKey+"FStep");
  midStep=Control.EvalVar<double>(chipKey+"MFStep");
  shineMat=ModelSupport::EvalMat<int>(Control,chipKey+"ShineMat");

  if (shineMat)
    {
      linerStartXZ[0]=-Control.EvalVar<double>(chipKey+"ShineFrontLeft");
      linerStartXZ[1]=Control.EvalVar<double>(chipKey+"ShineFrontRight");
      linerStartXZ[2]=-Control.EvalVar<double>(chipKey+"ShineFrontDown");
      linerStartXZ[3]=Control.EvalVar<double>(chipKey+"ShineFrontUp");
      
      linerEndXZ[0]=-Control.EvalVar<double>(chipKey+"ShineEndLeft");
      linerEndXZ[1]=Control.EvalVar<double>(chipKey+"ShineEndRight");
      linerEndXZ[2]=-Control.EvalVar<double>(chipKey+"ShineEndDown");
      linerEndXZ[3]=Control.EvalVar<double>(chipKey+"ShineEndUp");

      insertStep=Control.EvalVar<double>(chipKey+"ShineInsertStep");

      backScrapThick=Control.EvalVar<double>(chipKey+"BackScrapThick");
      backScrapExtra=Control.EvalVar<double>(chipKey+"BackScrapExtra");
      backScrapMat=ModelSupport::EvalMat<int>(Control,chipKey+"BackScrapMat");
    }
  
  populated=1;
  return;
}

void
ChipIRShutterFlat::createCInfoTable(Simulation& System)
  /*!
    Create the table of insert components
    \param System :: Simulation to use    
  */
{
  ELog::RegMethod RegA("ChipIRShutterFlat","createCInfoTable");

  CInfo.clear();
  
  if (!DPlane)
    { 
      ELog::EM<<"Unable to build Chip Shutter Insert :\n";
      ELog::EM<<"Missing Divide Plane (40000)"<<ELog::endErr;
      return;
    }

  CInfo.push_back(insertBaseInfo(System,SMap));
  insertBaseInfo& ASection=CInfo.back();
  
  // top/bottom : back/front
  ASection.setSides(SMap.realSurf(surfIndex+13),
		    -SMap.realSurf(surfIndex+14),
		    -SMap.realSurf(surfIndex+125),
		    SMap.realSurf(surfIndex+126));

  ASection.setInit(frontPt+Z*voidZOffset,BeamAxis, 
		   frontPt+BeamAxis*forwardStep+Z*voidZOffset,BeamAxis);
  ASection.populate(chipKey+"I");
  
  ASection.setFPlane(SMap.realSurf(surfIndex+501));
  return;
}

void
ChipIRShutterFlat::createShutterInsert(Simulation& System)
  /*!
    Create the walls for the chipIR hutch
    \param System :: Simulation
  */
{
  ELog::RegMethod RegA("ChipIRShutter","createShutterInsert");
  const masterRotate& MR=masterRotate::Instance();
  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();
  // Create forward plane:
  // Plane is projected along pipe BUT is normal to the shutter direction:


  std::string Out;
  const std::string dSurf=divideStr();
  const double zShift=(closed % 2) ? 
    voidZOffset+closedZShift-openZShift : voidZOffset;
  const Geometry::Vec3D Pt=frontPt+BeamAxis*forwardStep+Z*zShift;
  
  ModelSupport::buildPlane(SMap,surfIndex+501,Pt,BeamAxis);

  CS.setCNum(chipIRDatum::secScatCentre,MR.calcRotate(Pt));

  insertBaseInfo& ASection=CInfo.front();
  // Create front object

  System.removeCell(innerVoidCell);              // Inner void
  // Add front spacer:
  Out=ModelSupport::getComposite(SMap,surfIndex,
				 "-501 7")+dSurf+ASection.getFullSides();
  System.addCell(MonteCarlo::Qhull(innerVoidCell,0,0.0,Out));
  // ADD scatter objects:
  ASection.createObjects(surfIndex+502,cellIndex);

  return;
}  


void
ChipIRShutterFlat::createShinePipe(Simulation& System)
/*!
  Create the shine path
  \param System :: Simulation to add the shine path 
*/
{
  ELog::RegMethod RegA("ChipIRShutterFlat","createShinePipe");

  const masterRotate& MR=masterRotate::Instance();
  chipIRDatum::chipDataStore& CS=chipIRDatum::chipDataStore::Instance();

  if (CInfo.empty())
    throw ColErr::EmptyValue<std::string>("CInfo");
  const insertBaseInfo& ASection=CInfo.back();

  const std::string dSurf=divideStr();
  const int bPlane=ASection.getBPlane();
  const int outCyl=SMap.realSurf(surfIndex+17);
  
  // Check Centre Point:
  
  std::ostringstream cx;
  cx<<" "<<bPlane<<" "<<-outCyl<<" "<<dSurf;
  const std::string fullLength=cx.str();

  cx.str("");
  cx<<" "<<bPlane<<" "<<dSurf;
  const std::string leadLength=cx.str();
  // ADD SHINE PIPE IF active:
  if (shineMat)
    {
      // Create BackScrap
      const Geometry::Cylinder* BPtr=
	SMap.realPtr<Geometry::Cylinder>(outCyl);
      ModelSupport::buildCylinder(SMap,surfIndex+2007,
				  BPtr->getCentre(),BPtr->getNormal(),
				  BPtr->getRadius()-backScrapThick);
      
	
      std::string Out;
      // Centre of void : along beam axis 
      const Geometry::Vec3D Cp(ASection.getLastPt());
      const Geometry::Vec3D Ep(endPt+Z*voidZOffset);
      // left/right/base/top
      ModelSupport::buildPlane(SMap,surfIndex+1503,
			       Cp+X*linerStartXZ[0]+Z*linerStartXZ[2],
			       Cp+X*linerStartXZ[0]+Z*linerStartXZ[3],
			       Ep+X*linerEndXZ[0]+Z*linerEndXZ[2],
			       X);
      ModelSupport::buildPlane(SMap,surfIndex+1504,
			       Cp+X*linerStartXZ[1]+Z*linerStartXZ[2],
			       Cp+X*linerStartXZ[1]+Z*linerStartXZ[3],
			       Ep+X*linerEndXZ[1]+Z*linerEndXZ[2],
			       -X);
      ModelSupport::buildPlane(SMap,surfIndex+1505,
			       Cp+X*linerStartXZ[0]+Z*linerStartXZ[2],
			       Cp+X*linerStartXZ[1]+Z*linerStartXZ[2],
			       Ep+X*linerEndXZ[1]+Z*linerEndXZ[2],
			       Z);
      ModelSupport::buildPlane(SMap,surfIndex+1506,
			       Cp+X*linerStartXZ[0]+Z*linerStartXZ[3],
			       Cp+X*linerStartXZ[1]+Z*linerStartXZ[3],
			       Ep+X*linerEndXZ[1]+Z*linerEndXZ[3],
			       -Z);
      // Inner void
      Out=ModelSupport::getComposite(SMap,surfIndex,"1503 1504 1505 1506");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+fullLength));
      // Lead
      Out=ModelSupport::getComposite(SMap,surfIndex,
				     "(-1503:-1504:-1505:-1506) -2007 ");
      Out+=ASection.getFullSides()+leadLength;
      System.addCell(MonteCarlo::Qhull(cellIndex++,shineMat,0.0,Out));
      // Aluminium
      Out=ModelSupport::getComposite(SMap,surfIndex,
				     "(-1503:-1504:-1505:-1506) 2007 ");
      Out+=ASection.getFullSides()+fullLength;
      System.addCell(MonteCarlo::Qhull(cellIndex++,backScrapMat,0.0,Out));

      
      // ADD SHINE POINTS TO CHIPDATUM:
      for(size_t i=0;i<4;i++)
	{
	  CS.setDNum(chipIRDatum::shutterPt1+i,
	     MR.calcRotate(Cp+X*linerStartXZ[i/2]+Z*linerStartXZ[2+(i%2)]));
	  CS.setDNum(chipIRDatum::shutterPt5+i,
	     MR.calcRotate(Ep+X*linerEndXZ[i/2]+Z*linerEndXZ[2+(i%2)]));
	}
      // Shine Points:
      Geometry::Vec3D Pt;
      Geometry::Vec3D frontPt;
      Geometry::Vec3D backPt;
      for(int i=0;i<4;i++)
	{
	  Pt=SurInter::getPoint(SMap.realSurfPtr(surfIndex+505),
				SMap.realSurfPtr(surfIndex+1503+(i%2)),
				SMap.realSurfPtr(surfIndex+1505+i/2));
	  CS.setCNum(chipIRDatum::shinePt1+static_cast<size_t>(i),
		     MR.calcRotate(Pt));
	  frontPt+=Pt;
	  Pt=SurInter::getPoint(SMap.realSurfPtr(outCyl),
				SMap.realSurfPtr(surfIndex+1503+(i%2)),
				SMap.realSurfPtr(surfIndex+1505+i/2),
				Ep);
	  CS.setCNum(chipIRDatum::shinePt5+static_cast<size_t>(i),
		     MR.calcRotate(Pt));
	  backPt+=Pt;
	}
      FixedComp::setConnect(0,frontPt/4.0,-beamAxis);
      FixedComp::setConnect(1,backPt/4.0,beamAxis);
    }
  else
    {
      System.addCell(cellIndex++,0,ASection.getFullSides()+fullLength);  
    }
  
  return;
}

const insertBaseInfo&
ChipIRShutterFlat::getLastItem() const
  /*!
    Gets the last item in the base shell to 
    allow downstream items.
    \return CInfo last item
  */
{
  if (CInfo.empty())
    throw ColErr::EmptyValue<std::string>("ChipIRShutterFlat::getLastItem");
  return CInfo.back();
}

void
ChipIRShutterFlat::createAll(Simulation& System,const double ZOffset,
			     const attachSystem::FixedComp* FCPtr)
  /*!
    Create the shutter
    \param System :: Simulation to process
  */
{
  ELog::RegMethod RegA("ChipIRShutterFlat","createAll");
  GeneralShutter::createAll(System,ZOffset,FCPtr);
  populate(System);
  createCInfoTable(System);
  createShutterInsert(System);  
  createShinePipe(System);
  return;
}
  
}  // NAMESPACE shutterSystem
