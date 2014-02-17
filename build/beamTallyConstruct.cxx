/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/beamTallyConstruct.cxx
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
#include <iterator>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Tensor.h"
#include "Vec3D.h"
#include "Triple.h"
#include "support.h"
#include "stringCombine.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "TallyCreate.h"
#include "Transform.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Line.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "MainProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "LinearComp.h"
#include "PositionSupport.h"
#include "Simulation.h"
#include "inputParam.h"
#include "cellDistance.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "SurfLine.h"

#include "InsertComp.h"
#include "shutterBlock.h"
#include "GeneralShutter.h"
#include "BlockShutter.h"

#include "TallySelector.h" 
#include "SpecialSurf.h"
#include "basicConstruct.h" 
#include "pointConstruct.h" 
#include "beamTallyConstruct.h" 


namespace tallySystem
{

beamTallyConstruct::beamTallyConstruct() :
  pointConstruct()
  /// Constructor
{}

beamTallyConstruct::beamTallyConstruct(const beamTallyConstruct& A) :
  pointConstruct(A)
  /*!
    Copy Constructor
  */
{}

beamTallyConstruct&
beamTallyConstruct::operator=(const beamTallyConstruct& A) 
  /// Assignment operator
{
  if (this!=&A)
    {
      pointConstruct::operator=(A);
    }
  return *this;
}

void
beamTallyConstruct::processPoint(Simulation& System,
			     const mainSystem::inputParam& IParam,
			     const size_t Index) const
  /*!
    Add point tally (s) as needed
    \param System :: Simulation to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
   */
{
  ELog::RegMethod RegA("beamTallyConstruct","processPoint");

  const size_t NItems=IParam.itemCnt("tally",Index);
  if (NItems<2)
    throw ColErr::IndexError<size_t>(NItems,2,
				     "Insufficient items for tally");


  const std::string PType(IParam.getCompValue<std::string>("tally",Index,1)); 

  if (PType=="help")  // 
    {
      ELog::EM<<
	"beamline [number] {modName,viewindex,beamDist,windowOffset,ZRotAngle} "
        "1000 cm from the moderator surface \n"
	"shutterline [number] {modName,viewindex,beamDist,windowOffset,ZRotAngle} "
        "Uses shutter insert to build window \n"
	"viewline [number] {beamDist,timeOffset,windowOffset,ZRotAngle} "
        "Uses shutter insert to build time offset window \n"
	<<ELog::endBasic;
      pointConstruct::processPoint(System,IParam,Index);
      return;
    }
  
  if (PType=="beamline" || PType=="shutterline")
    {
      std::string modName;
      int viewIndex(0);
      const int beamNum=inputItem<int>(IParam,Index,2,
				       "beamline number not given");
      double beamDist(1000.0);
      double windowOffset(0.0);
      double pointZRot(0.0);

      checkItem<std::string>(IParam,Index,3,modName);
      checkItem<int>(IParam,Index,4,viewIndex);
      checkItem<double>(IParam,Index,5,beamDist);
      checkItem<double>(IParam,Index,6,windowOffset);
      checkItem<double>(IParam,Index,7,pointZRot);

      if (PType=="beamline")
	addBeamLineTally(System,beamNum-1,beamDist,modName,
			 viewIndex,windowOffset,pointZRot);
      
      else
	addShutterTally(System,beamNum-1,beamDist,modName,
			viewIndex,windowOffset,pointZRot);
      
      return;
    }
  if (PType=="viewline")  // beamline
    {
      const int beamNum=inputItem<int>(IParam,Index,2,
				       "beamline number not given");
      double beamDist(1000.0);
      double windowOffset(0.0);
      double pointZRot(0.0);
      double timeOffset(0.0);

      checkItem<double>(IParam,Index,3,beamDist);
      checkItem<double>(IParam,Index,4,timeOffset);
      checkItem<double>(IParam,Index,5,windowOffset);
      checkItem<double>(IParam,Index,6,pointZRot);

      addViewLineTally(System,beamNum-1,beamDist,timeOffset,
		       windowOffset,pointZRot);
      return;
    }
  if (PType=="viewInner")  // beamline Inner
    {
      const int beamNum=inputItem<int>(IParam,Index,2,
				       "beamline number not given");
      const std::string face=inputItem<std::string>(IParam,Index,3,
			       "beamline face : front/back not given");
      const int linkNumber=getLinkIndex(face);

      double beamDist(1000.0);
      double windowOffset(0.0);
      double pointZRot(0.0);
      double timeOffset(0.0);

      checkItem<double>(IParam,Index,4,beamDist);
      checkItem<double>(IParam,Index,5,timeOffset);
      checkItem<double>(IParam,Index,6,windowOffset);
      checkItem<double>(IParam,Index,7,pointZRot);

      addViewInnerTally(System,beamNum-1,linkNumber,
			beamDist,timeOffset,
			windowOffset,pointZRot);
      return;
    }
  pointConstruct::processPoint(System,IParam,Index);
  return;
}

void 
beamTallyConstruct::addBeamLineTally(Simulation& System,
				 const int beamNum,
				 const double beamDist,
				 const std::string& modName,
				 const int viewSurface,
				 const double windowOffset,
				 const double pointZRot) const
  /*!
    Adds a beamline tally to the system
    \param System :: Simulation to add tallies
    \param beamNum :: Beamline to use [1-18]
    \param beamDist :: Distance from moderator face
    \param modName :: Moderator Name to view
    \param viewSurface :: Surface index
    \param windowOffset :: Distance to move window towards tally point
    \param pointZRot :: Z axis rotation of the beamline
  */
{
  ELog::RegMethod RegA("beamTallyConstruct","addBeamLineTally");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  std::vector<int> Planes;
  //  const int tNum=System.nextTallyNum(5);
  Geometry::Vec3D TPoint;

  int masterPlane(0);

  size_t iLP((viewSurface>=0) ? static_cast<size_t>(viewSurface) : 
	     static_cast<size_t>(-viewSurface-1));
  int VSign((viewSurface<0) ? -1 : 1);

  const attachSystem::FixedComp* ModPtr;
  const attachSystem::FixedComp* ShutterPtr;

  // Do something if using old TS2 style point tally
  if (modName.empty())
    {
      VSign=1;
      if (beamNum<4)
	{
	  ModPtr=OR.getObject<attachSystem::FixedComp>("decoupled");
	  iLP=0;
	}
      else if (beamNum<9)
	{
	  ModPtr=OR.getObject<attachSystem::FixedComp>("hydrogen");
	  iLP=0;
	}
      else if (beamNum<14)
	{
	  ModPtr=OR.getObject<attachSystem::FixedComp>("groove");
	  iLP=0;
	}
      else
	{
	  ModPtr=OR.getObject<attachSystem::FixedComp>("decoupled");
	  iLP=1;
	}
    }
  else
    {
      ModPtr=OR.getObject<attachSystem::FixedComp>(modName);
    }
  
  ShutterPtr=OR.getObject<attachSystem::FixedComp>
    (StrFunc::makeString(std::string("shutter"),beamNum));

  if (!ShutterPtr)    
    throw ColErr::InContainerError<int>(beamNum,"Shutter Object not found");
  if (!ModPtr)    
    throw ColErr::InContainerError<std::string>
      (modName,"Moderator Object not found");

  // MODERATOR PLANE
  masterPlane=
    ModPtr->getExitWindow(iLP,Planes);

  const attachSystem::TwinComp* TwinPtr=
    dynamic_cast<const attachSystem::TwinComp*>(ShutterPtr);

  Geometry::Vec3D BAxis=(TwinPtr) ? 
    TwinPtr->getBY()*-1.0 :  ShutterPtr->getLinkAxis(0);
  Geometry::Vec3D shutterPoint=(TwinPtr) ?
    TwinPtr->getBeamStart() : 
    ShutterPtr->getLinkPt(0); 
  // CALC Intercept between Moderator boundary
  std::vector<Geometry::Vec3D> Window=
    calcWindowIntercept(masterPlane,Planes,shutterPoint);

  // CALC intesect between shutter axis / shutter point and moderator plane
  std::vector<Geometry::Vec3D> OutPts;
  Geometry::Line LN(shutterPoint,-BAxis);
  Geometry::Plane SX(0,0);
  SX.setPlane(Window[0],Window[1],Window[2]);
  if (!LN.intersect(OutPts,SX))
    ELog::EM<<"Failed to intersect moderator window"<<ELog::endErr;
  const Geometry::Vec3D MidPt=OutPts.front();

  const int tNum=System.nextTallyNum(5);
  std::transform(Window.begin(),Window.end(),Window.begin(),
        boost::bind(std::minus<Geometry::Vec3D>(),_1,BAxis*windowOffset));
  std::vector<Geometry::Vec3D>::iterator vc;
  ELog::EM<<"BEAM START "<<shutterPoint<<ELog::endDebug;
  for(vc=Window.begin();vc!=Window.end();vc++)
    ELog::EM<<"Window == "<<*vc<<ELog::endDebug;

  // Apply rotation
  if (fabs(pointZRot)>Geometry::zeroTol)
    {
      const Geometry::Vec3D Z=ShutterPtr->getZ();
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(pointZRot,Z);
      Qxy.rotate(BAxis);
    }
  
  addF5Tally(System,tNum,MidPt-BAxis*beamDist,Window);
  ELog::EM<<"Tally: "<<tNum<<" "<<MidPt-BAxis*beamDist<<ELog::endTrace;
  // (NORMAL VIEW):
  // tallySystem::setF5Position(System,tNum,RefPtr->getViewOrigin(beamNum),
  // 			     GS->getBeamAxis(),beamDist,0.0);
  return;
}

void 
beamTallyConstruct::addShutterTally(Simulation& System,
				const int beamNum,
				const double beamDist,
				const std::string& modName,
				const int viewSurface,
				const double windowOffset,
				const double pointZRot) const
  /*!
    Adds a Shutterline tally to the system
    \param System :: Simulation to add tallies
    \param beamNum :: Beamline to use [1-18]
    \param beamDist :: Distance from moderator face
    \param modName :: Moderator Name to view
    \param viewSurface :: Surface index
    \param windowOffset :: Distance to move window towards tally point
    \param pointZRot :: Z axis rotation of the beamline
  */
{
  ELog::RegMethod RegA("beamTallyConstruct","addShutterTally");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  std::vector<int> Planes;
  //  const int tNum=System.nextTallyNum(5);
  Geometry::Vec3D TPoint;

  int masterPlane(0);

  size_t iLP((viewSurface>=0) ? static_cast<size_t>(viewSurface) : 
	     static_cast<size_t>(-viewSurface-1));
  int VSign((viewSurface<0) ? -1 : 1);

  const attachSystem::FixedComp* ModPtr;
  const attachSystem::FixedComp* ShutterPtr;

  ModPtr=OR.getObject<attachSystem::FixedComp>(modName);  
  ShutterPtr=OR.getObject<attachSystem::FixedComp>
    (StrFunc::makeString(std::string("shutter"),beamNum));

  if (!ShutterPtr)    
    throw ColErr::InContainerError<int>(beamNum,"Shutter Object not found");
  if (!ModPtr)    
    throw ColErr::InContainerError<std::string>
      (modName,"Moderator Object not found");

  // MODERATOR PLANE
  masterPlane=ModPtr->getExitWindow(iLP,Planes);

  const attachSystem::TwinComp* TwinPtr=
    dynamic_cast<const attachSystem::TwinComp*>(ShutterPtr);

  Geometry::Vec3D BAxis=(TwinPtr) ? 
    TwinPtr->getBY()*-1.0 :  ShutterPtr->getLinkAxis(0);
  Geometry::Vec3D shutterPoint=(TwinPtr) ?
    TwinPtr->getBeamStart() : 
    ShutterPtr->getLinkPt(0); 
  // CALC Intercept between Moderator boundary
  std::vector<Geometry::Vec3D> Window=
    calcWindowIntercept(masterPlane,Planes,shutterPoint);


  // CALC intesect between shutter axis / shutter point and moderator plane
  std::vector<Geometry::Vec3D> OutPts;
  Geometry::Line LN(shutterPoint,-BAxis);
  Geometry::Plane SX(0,0);
  SX.setPlane(Window[0],Window[1],Window[2]);
  if (!LN.intersect(OutPts,SX))
    ELog::EM<<"Failed to intersect moderator window"<<ELog::endErr;
  const Geometry::Vec3D MidPt=OutPts.front();

  const int tNum=System.nextTallyNum(5);
  std::transform(Window.begin(),Window.end(),Window.begin(),
        boost::bind(std::minus<Geometry::Vec3D>(),_1,BAxis*windowOffset));
  std::vector<Geometry::Vec3D>::iterator vc;
  ELog::EM<<"BEAM START "<<shutterPoint<<ELog::endDebug;
  for(vc=Window.begin();vc!=Window.end();vc++)
    ELog::EM<<"Window == "<<*vc<<ELog::endDebug;

  // Apply rotation
  if (fabs(pointZRot)>Geometry::zeroTol)
    {
      const Geometry::Vec3D Z=ShutterPtr->getZ();
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(pointZRot,Z);
      Qxy.rotate(BAxis);
    }
  
  addF5Tally(System,tNum,MidPt-BAxis*beamDist,Window);
  ELog::EM<<"Tally: "<<tNum<<" "<<MidPt-BAxis*beamDist<<ELog::endTrace;
  // (NORMAL VIEW):
  // tallySystem::setF5Position(System,tNum,RefPtr->getViewOrigin(beamNum),
  // 			     GS->getBeamAxis(),beamDist,0.0);
  return;
}

void 
beamTallyConstruct::addViewLineTally(Simulation& System,
				 const int beamNum,
				 const double beamDist,
				 const double timeOffset,
				 const double windowOffset,
				 const double pointZRot) const
  /*!
    Adds a beamline tally to the system
    \param System :: Simulation to add tallies
    \param beamNum :: Beamline to use [1-18]
    \param beamDist :: Distance from moderator face
    \param timeOffset :: Time back step for tally
    \param windowOffset :: Distance to move window towards tally point
    \param pointZRot :: Z axis rotation of the beamline
  */
{
  ELog::RegMethod RegA("beamTallyConstruct","addViewLineTally");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  std::vector<int> Planes;
  //  const int tNum=System.nextTallyNum(5);
  Geometry::Vec3D TPoint;

  int masterPlane(0);

  const attachSystem::FixedComp* ShutterPtr;

  ShutterPtr=OR.getObject<attachSystem::FixedComp>
    (StrFunc::makeString(std::string("shutter"),beamNum));

  if (!ShutterPtr)    
    throw ColErr::InContainerError<int>(beamNum,"Shutter Object not found");

  // MODERATOR PLANE
  masterPlane=ShutterPtr->getExitWindow(0,Planes);

  const attachSystem::TwinComp* TwinPtr=
    dynamic_cast<const attachSystem::TwinComp*>(ShutterPtr);
  Geometry::Vec3D BAxis=(TwinPtr) ? 
    TwinPtr->getBY()*-1.0 :  ShutterPtr->getLinkAxis(0);
  Geometry::Vec3D shutterPoint=(TwinPtr) ?
    TwinPtr->getBeamStart() : 
    ShutterPtr->getLinkPt(0); 
  // CALC Intercept between Moderator boundary
  std::vector<Geometry::Vec3D> Window=
    calcWindowIntercept(masterPlane,Planes,shutterPoint);

 
  const int tNum=System.nextTallyNum(5);
  std::transform(Window.begin(),Window.end(),Window.begin(),
        boost::bind(std::minus<Geometry::Vec3D>(),_1,BAxis*windowOffset));

  std::vector<Geometry::Vec3D>::iterator vc;
  for(vc=Window.begin();vc!=Window.end();vc++)
    ELog::EM<<"Window == "<<*vc<<ELog::endDebug;

  // Apply rotation
  if (fabs(pointZRot)>Geometry::zeroTol)
    {
      const Geometry::Vec3D Z=ShutterPtr->getZ();
      const Geometry::Quaternion Qxy=
	Geometry::Quaternion::calcQRotDeg(pointZRot,Z);
      Qxy.rotate(BAxis);
    }
  
  addF5Tally(System,tNum,shutterPoint-BAxis*beamDist,
	     Window,timeOffset);
//  ELog::EM<<"Tally: "<<tNum<<" "<<MidPt-BAxis*beamDist<<ELog::endTrace;
  // (NORMAL VIEW):
  // tallySystem::setF5Position(System,tNum,RefPtr->getViewOrigin(beamNum),
  // 			     GS->getBeamAxis(),beamDist,0.0);
  return;
}

void 
beamTallyConstruct::addViewInnerTally(Simulation& System,
				 const int beamNum,
				 const int faceFlag,     
				 const double beamDist,
				 const double timeOffset,
				 const double windowOffset,
				 const double pointZRot) const
  /*!
    Adds a beamline tally to the system
    \param System :: Simulation to add tallies
    \param beamNum :: Beamline to use [1-18]
    \param faceFlag :: Face -- Front/Back
    \param beamDist :: Distance from moderator face
    \param timeOffset :: Time back step for tally
    \param windowOffset :: Distance to move window towards tally point
    \param pointZRot :: Z axis rotation of the beamline
  */
{

  ELog::RegMethod RegA("beamTallyConstruct","addViewInnerTally");
      
  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();
  
  std::vector<int> Planes;
  //  const int tNum=System.nextTallyNum(5);
  Geometry::Vec3D TPoint;

  int masterPlane(0);

  const attachSystem::FixedComp* ShutterPtr;

  ShutterPtr=OR.getObject<attachSystem::FixedComp>
    (StrFunc::makeString(std::string("shutter"),beamNum));

  if (!ShutterPtr)    
    throw ColErr::InContainerError<int>(beamNum,"Shutter Object not found");

  // MODERATOR PLANE
  masterPlane=ShutterPtr->getExitWindow(0,Planes);
  const shutterSystem::BlockShutter* BSPtr=
    dynamic_cast< const shutterSystem::BlockShutter* >(ShutterPtr);

  if (!BSPtr)
    {
      ELog::EM<<"Block shutter not found "<<ELog::endErr;
      return;
    }

  std::vector<Geometry::Vec3D> VOut=
    (faceFlag!=2) ? BSPtr->createFrontViewPoints() : 
    BSPtr->createBackViewPoints();

  if (VOut.size()<4)
    {
      ELog::EM<<"Failed to find B4C in the viewAxis "<<ELog::endErr;
      return;
    }
  Geometry::Vec3D BAxis;
  Geometry::Vec3D BOrigin;
  calcBeamDirection(*ShutterPtr,BOrigin,BAxis);

  attachSystem::applyZAxisRotate(*ShutterPtr,pointZRot,BAxis);

  // The projects the points on the B4C cut into the 
  // forward plan. 
  const Geometry::Vec3D tallyPoint=BOrigin-BAxis*beamDist;
  for(size_t i=0;i<4;i++)
    {
      const Geometry::Vec3D UV=(VOut[i]-tallyPoint).unit();
      VOut[i]=SurInter::lineSurfPoint(tallyPoint,UV,masterPlane,BOrigin);
    }

  const int tNum=System.nextTallyNum(5);
  std::transform(VOut.begin(),VOut.end(),VOut.begin(),
        boost::bind(std::minus<Geometry::Vec3D>(),_1,BAxis*windowOffset));

  addF5Tally(System,tNum,tallyPoint,VOut,timeOffset);
  return;
}


}  // NAMESPACE tallySystem
