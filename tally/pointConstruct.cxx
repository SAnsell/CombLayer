/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tally/pointConstruct.cxx
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
#include "PositionSupport.h"
#include "Simulation.h"
#include "inputParam.h"
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


namespace tallySystem
{

pointConstruct::pointConstruct() 
  /// Constructor
{}

pointConstruct::pointConstruct(const pointConstruct&) 
  /// Copy Constructor
{}

pointConstruct&
pointConstruct::operator=(const pointConstruct&) 
  /// Assignment operator
{
  return *this;
}

void
pointConstruct::processPoint(Simulation& System,
			     const mainSystem::inputParam& IParam,
			     const size_t Index) const
  /*!
    Add point tally (s) as needed
    \param System :: Simulation to add tallies
    \param IParam :: Main input parameters
    \param Index :: index of the -T card
   */
{
  ELog::RegMethod RegA("pointConstruct","processPoint");

  const size_t NItems=IParam.itemCnt("tally",Index);
  if (NItems<2)
    throw ColErr::IndexError<size_t>(NItems,2,
				     "Insufficient items for tally");


  const std::string PType(IParam.getCompValue<std::string>("tally",Index,1)); 

  if (PType=="help")  // 
    {
      ELog::EM<<
	"free Vec3D -- point detector at point\n"
	"object ObjName link dist -- point detector at "
	" point relative to object\n"
	"freeWindow Vec3D Vec3D Vec3D Vec3D Vec3D -- \n"
	"   Point + four coordinates of the window\n"
	"window Item front/back/side distance\n"
	<<ELog::endBasic;
      return;
    }
  
  const masterRotate& MR=masterRotate::Instance();

  std::string revStr;

  if (PType=="free")
    {
      std::vector<Geometry::Vec3D> EmptyVec;
      Geometry::Vec3D PPoint=
	inputItem<Geometry::Vec3D>(IParam,Index,2,"Point for point detector");

      const int flag=checkItem<std::string>(IParam,Index,5,revStr);
      if (!flag || revStr!="r")
	PPoint=MR.reverseRotate(PPoint);
      processPointFree(System,PPoint,EmptyVec);
      return;
    }
  if (PType=="freeWindow")
    {
      size_t windowIndex(6);
      Geometry::Vec3D PPoint=
	inputItem<Geometry::Vec3D>(IParam,Index,2,"Point for point detector");
      int flag=checkItem<std::string>(IParam,Index,5,revStr);
      if (!flag || revStr!="r")
	{
	  PPoint=MR.reverseRotate(PPoint);
	  windowIndex--;
	}
      
      std::vector<Geometry::Vec3D> WindowPts(4);
      for(size_t i=0;i<4;windowIndex+=3,i++)
	WindowPts[i]=
	  inputItem<Geometry::Vec3D>(IParam,Index,windowIndex,"Window point");
      
      flag=checkItem<std::string>(IParam,Index,5,revStr);
      if (!flag || revStr!="r")
	PPoint=MR.reverseRotate(PPoint);
      
      processPointFree(System,PPoint,WindowPts);
      return;
    }
  if (PType=="window")
    {
      const std::string place=
	inputItem<std::string>(IParam,Index,2,"position not given");
      const std::string snd=
	inputItem<std::string>(IParam,Index,3,"front/back/side not give");

      const double D=
	inputItem<double>(IParam,Index,4,"Distance not given");

      double timeStep(0.0);
      double windowOffset(0.0);

      checkItem<double>(IParam,Index,5,timeStep);
      checkItem<double>(IParam,Index,6,windowOffset);
      const int linkNumber=getLinkIndex(snd);
      processPointWindow(System,place,linkNumber,D,timeStep,windowOffset);

      return;
    }

  if (PType=="object")
    {
      const std::string place=
	inputItem<std::string>(IParam,Index,2,"position not given");
      const std::string snd=
	inputItem<std::string>(IParam,Index,3,"front/back/side not give");
      const double D=
	inputItem<double>(IParam,Index,4,"Distance not given");
      const int linkNumber=getLinkIndex(snd);
      processPointFree(System,place,linkNumber,D);

      return;
    }

  return;
}

void
pointConstruct::processPointWindow(Simulation& System,
				   const std::string& FObject,
				   const int linkPt,const double beamDist,
				   const double timeStep,
				   const double windowOffset) const
  /*!
    Process a point tally in a registered object
    \param System :: Simulation to add tallies
    \param FObject :: Fixed/Twin name
    \param linkPt :: Link point [-ve for beam object]
    \param beamDist :: Out distance Distance
    \param timeStep ::  Forward step [view type modification]
    \param windowOffset :: Distance to move window towards tally point
  */
{
  ELog::RegMethod RegA("pointConstruct","processPointWindow");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::vector<int> Planes;
  const int tNum=System.nextTallyNum(5);
  Geometry::Vec3D TPoint;
  Geometry::Vec3D orgPoint;
  Geometry::Vec3D BAxis;
  int masterPlane(0);
  if (linkPt>0)
    {
      const attachSystem::FixedComp* TPtr=
	OR.getObject<attachSystem::FixedComp>(FObject);

      if (!TPtr)
	throw ColErr::InContainerError<std::string>
	  (FObject,"Fixed Object not found");
      const size_t iLP=static_cast<size_t>(linkPt-1);

      masterPlane=TPtr->getExitWindow(iLP,Planes);
      orgPoint= TPtr->getLinkPt(iLP); 
      BAxis= -TPtr->getLinkAxis(iLP);
      TPoint=orgPoint-BAxis*(beamDist+timeStep);
      ELog::EM<<"Link point   == "<<orgPoint<<ELog::endDiag;
      ELog::EM<<"Link Axis    == "<<BAxis<<ELog::endDiag;
      ELog::EM<<"Tally Point  == "<<TPoint<<ELog::endDiag;

    }
  // Add tally:
  if (Planes.size()<=4 || !masterPlane)
    {
      ELog::EM<<"Failed to set tally : Object "<<FObject<<ELog::endErr;
      return;
    }


  // CALC Intercept between Moderator boundary
  std::vector<Geometry::Vec3D> Window=
    calcWindowIntercept(masterPlane,Planes,orgPoint);

  std::transform(Window.begin(),Window.end(),Window.begin(),
        boost::bind(std::minus<Geometry::Vec3D>(),_1,BAxis*windowOffset));

  addF5Tally(System,tNum,TPoint,Window,timeStep);
  return;
}

void
pointConstruct::processPointFree(Simulation& System,
				 const std::string& FObject,
				 const int linkPt,const double OD) const
/*!
  Process a point tally in a registered object
  \param System :: Simulation to add tallies
    \param FObject :: Fixed/Twin name
    \param linkPt :: Link point [-ve for beam object]
    \param OD :: Out distance Distance
   */
{
  ELog::RegMethod RegA("pointConstruct","processPointFree(String)");

  ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  std::vector<int> Planes;
  const int tNum=System.nextTallyNum(5);
  Geometry::Vec3D TPoint;
  int masterPlane(0);
  if (linkPt>0)
    {
      const attachSystem::FixedComp* TPtr=
	OR.getObject<attachSystem::FixedComp>(FObject);

      if (!TPtr)
	throw ColErr::InContainerError<std::string>
	  (FObject,"Fixed Object not found");
      size_t iLP=static_cast<size_t>(linkPt-1);
      masterPlane=
	TPtr->getExitWindow(static_cast<size_t>(iLP),Planes);
      ELog::EM<<"Link point == "<<TPtr->getLinkPt(iLP)<<ELog::endDebug;
      ELog::EM<<"Link Axis  == "<<TPtr->getLinkAxis(iLP)<<ELog::endDebug;
      TPoint=TPtr->getLinkPt(iLP)+TPtr->getLinkAxis(iLP)*OD;
      std::vector<Geometry::Vec3D> EmptyVec;
      addF5Tally(System,tNum,TPoint,EmptyVec);
    }

  return;
}

void
pointConstruct::processPointFree(Simulation& System,
		    	 const Geometry::Vec3D& Point,
		    const std::vector<Geometry::Vec3D>& VList) const
  /*!
    Processes a grid tally : Requires variables and informaton 
    \param System :: Simulation to add tallies
    \param Point :: Point deterctor Point
    \param VList :: Window vectors
  */
{

  ELog::RegMethod RegA("pointConstruct","processPointFree(Vec)");
 
  const int tNum=System.nextTallyNum(5);
  addF5Tally(System,tNum,Point,VList);
  return;
}



std::vector<Geometry::Vec3D>
pointConstruct::calcWindowIntercept(const int bPlane,
				    const std::vector<int> EdgeSurf,
				    const Geometry::Vec3D& viewPoint)
  /*!
    Calculate the window for the point tally
    \param System :: Simulation item
    \param bPlane :: back plane/ cylinder /sphere etc.
    \param EdgeSurf :: In pairs
    \param viewPoint :: Point to start view
    \return Vector of Points
  */
{
  ELog::RegMethod RegA("pointConstruct","calcWindowIntercept");

  std::vector<Geometry::Vec3D> VList;
  std::vector<Geometry::Vec3D> Out;

  for(size_t i=0;i<4;i++)
    {
      const size_t j((i%3) ? 3 : 2);
      ModelSupport::calcVertex(bPlane,EdgeSurf[i % 2],
			       EdgeSurf[j],Out,viewPoint);
      if (Out.empty())
        {
	  ELog::EM<<"Unable to calculate intercept \n";
	  ELog::EM<<"Planes == "<<bPlane<<" "<<EdgeSurf[i%2]
		  <<" "<<EdgeSurf[j]
		  <<" ("<<viewPoint<<")"<<ELog::endErr;
	  return VList;
	}
      VList.push_back(Out[0]);
    }
  return VList;
}


void 
pointConstruct::addBasicPointTally(Simulation& System,
				   const attachSystem::FixedComp& FC,
				   const size_t FCpoint,
				   const double YStep) const
  /*!
    Adds a beamline tally to the system
    \param System :: Simulation system
    \param FC :: Guide unit to create tally after
    \param FCpoint :: Point surface
    \param YStep :: distance to step
  */
{
  ELog::RegMethod RegA("pointConstruct","addBasicPointTally");

  const masterRotate& MR=masterRotate::Instance();

  const int tNum=System.nextTallyNum(5);
  // Guide back point
  Geometry::Vec3D Pt=FC.getLinkPt(FCpoint);
  const Geometry::Vec3D TVec=FC.getLinkAxis(FCpoint);
  Pt+=TVec*YStep;      // Add so not on boundary
  ELog::EM<<"Tally "<<tNum<<" (point) = "
	  <<MR.calcRotate(Pt)<<ELog::endDiag;
  tallySystem::addF5Tally(System,tNum,Pt,			      
			  std::vector<Geometry::Vec3D>());
  return;
}

void
pointConstruct::calcBeamDirection(const attachSystem::FixedComp& FC,
				  Geometry::Vec3D& BOrigin,
				  Geometry::Vec3D& BAxis)
  /*!
    Calculate the beam direction and origin given a shutter component
    \parma FC :: Component that might be TwinComp
    \parma BOrigin :: Output for Origin
    \parma BAxis :: Output for Axis
   */
{
  ELog::RegMethod RegA("pointConstruct","calcBeamDirection");

  const attachSystem::TwinComp* TwinPtr=
    dynamic_cast<const attachSystem::TwinComp*>(&FC);
  BAxis=(TwinPtr) ?  TwinPtr->getBY()*-1.0 : FC.getLinkAxis(0);
  
  BOrigin=(TwinPtr) ?
    TwinPtr->getBeamStart() : FC.getLinkPt(0); 
  
  return;
}



}  // NAMESPACE tallySystem
