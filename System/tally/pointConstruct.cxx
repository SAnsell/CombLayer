/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/pointConstruct.cxx
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
#include <iterator>
#include <memory>

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
#include "Vec3D.h"
#include "Triple.h"
#include "support.h"
#include "stringCombine.h"
#include "NList.h"
#include "NRange.h"
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
#include "LinkSupport.h"
#include "Simulation.h"
#include "inputParam.h"
#include "Line.h"
#include "SurfLine.h"

#include "SpecialSurf.h"
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


  const std::string PType(IParam.getValue<std::string>("tally",Index,1)); 

  const masterRotate& MR=masterRotate::Instance();
  std::string revStr;

  if (PType=="free" || PType=="Free")
    {
      std::vector<Geometry::Vec3D> EmptyVec;
      size_t itemIndex(2);
      Geometry::Vec3D PPoint=IParam.getCntVec3D
	("tally",Index,itemIndex,"Point for point detector");
      const std::string revStr=
	IParam.getDefValue<std::string>("","tally",Index,itemIndex);

      if (revStr=="r" || revStr=="R")
	{
	  PPoint=MR.forceReverseRotate(PPoint);
	  ELog::EM<<"Remapped point == "<<PPoint<<ELog::endDiag;
	}
      processPointFree(System,PPoint,EmptyVec);
    }

  else if (PType=="freeWindow")
    {
      size_t itemIndex(2);
      Geometry::Vec3D PPoint=
	IParam.getCntVec3D("tally",Index,itemIndex,"Point for point detector");
      int flag=IParam.checkItem<std::string>
	("tally",Index,itemIndex,revStr);
      if (flag && (revStr=="r" || revStr=="R"))
	{
	  itemIndex++;
	  PPoint=MR.forceReverseRotate(PPoint);
	}
      
      std::vector<Geometry::Vec3D> WindowPts(4);
      for(size_t i=0;i<4;i++)
	WindowPts[i]=IParam.getCntVec3D
	  ("tally",Index,itemIndex,"Window point "+StrFunc::makeString(i+1));

      revStr=IParam.getDefValue<std::string>("","tally",Index,5);
      if (revStr=="r" || revStr=="R")
	PPoint=MR.forceReverseRotate(PPoint);
      
      processPointFree(System,PPoint,WindowPts);
    }
  else if (PType=="window")
    {
      const std::string place=
	IParam.getValueError<std::string>("tally",Index,2,"position not given");
      const std::string snd=
	IParam.getValueError<std::string>("tally",Index,3,"front/back/side not given");

      const double D=
	IParam.getValueError<double>("tally",Index,4,"Distance not given");


      const double timeStep=
	IParam.getDefValue<double>(0.0,"tally",Index,5);
      const double windowOffset=
	IParam.getDefValue<double>(0.0,"tally",Index,6);
	    

      const long int linkNumber=attachSystem::getLinkIndex(snd);
      processPointWindow(System,place,linkNumber,D,timeStep,windowOffset);
    }

  else if (PType=="object")
    {
      const std::string place=
	IParam.getValueError<std::string>("tally",Index,2,"position not given");
      const std::string snd=
	IParam.getValueError<std::string>("tally",Index,3,"front/back/side not given");
      const double D=
	IParam.getValueError<double>("tally",Index,4,"Distance not given");
      const long int linkNumber=attachSystem::getLinkIndex(snd);
      processPointFree(System,place,linkNumber,D);
    }
  else if (PType=="objOffset")
    {
      const std::string place=
	IParam.getValueError<std::string>("tally",Index,2,"position not given");
      const std::string snd=
	IParam.getValueError<std::string>("tally",Index,3,"front/back/side not give");

      size_t itemIndex(4);
      const Geometry::Vec3D DVec=
	IParam.getCntVec3D("tally",Index,itemIndex,"Offset");
      const long int linkNumber=attachSystem::getLinkIndex(snd);
      
      processPointFree(System,place,linkNumber,DVec);
    }
  else
    {
      ELog::EM<<"Point TallyType "<<PType<<" ignored"<<ELog::endWarn;
    }
  

  return;
}

void
pointConstruct::processPointWindow(Simulation& System,
				   const std::string& FObject,
				   long int linkPt,
				   const double beamDist,
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
  if (linkPt!=0)
    {
      const attachSystem::FixedComp* TPtr=
	OR.getObjectThrow<attachSystem::FixedComp>(FObject,"FixedComp");

      masterPlane=TPtr->getExitWindow(linkPt,Planes);
      orgPoint= TPtr->getLinkPt(linkPt); 
      BAxis= TPtr->getLinkAxis(linkPt);
      TPoint=orgPoint+BAxis*beamDist;
      
      ELog::EM<<"Link point   == "<<orgPoint<<ELog::endDiag;
      ELog::EM<<"Link Axis    == "<<BAxis<<ELog::endDiag;
      ELog::EM<<"Tally Point  == "<<TPoint<<ELog::endDiag;
      ELog::EM<<"TimeStep  == "<<timeStep<<ELog::endDiag;

    }
  // Add tally:
  if (Planes.size()<=4 || !masterPlane)
    {
      ELog::EM<<"Failed to set tally : Object "<<FObject<<ELog::endErr;
      return;
    }
  // Remove dividing plane [not used]
  if (Planes.size()>4) Planes.resize(4);
     
  ELog::EM<<"Plane size == "<<Planes.size()<<ELog::endDiag;
  // CALC Intercept between Moderator boundary
  std::vector<Geometry::Vec3D> Window=
    calcWindowIntercept(masterPlane,Planes,orgPoint);

  std::for_each(begin(Window),end(Window),
		[&,windowOffset,BAxis](Geometry::Vec3D& WVec)
		{ WVec-=BAxis*windowOffset; });

  addF5Tally(System,tNum,TPoint,Window,timeStep);
  return;
}

void
pointConstruct::processPointFree(Simulation& System,
				 const std::string& FObject,
				 const long int linkPt,
				 const double OD) const
  /*!
    Process a point tally in a registered object
    \param System :: Simulation to add tallies
    \param FObject :: Fixed/Twin name
    \param linkPt :: Link point [-ve for beam object]
    \param OD :: Out distance Distance
  */
{
  ELog::RegMethod RegA("pointConstruct","processPointFree(String)");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const attachSystem::FixedComp* TPtr=
    OR.getObjectThrow<attachSystem::FixedComp>(FObject,"FixedComp");
    
  const int tNum=System.nextTallyNum(5);
  Geometry::Vec3D TPoint=TPtr->getLinkPt(linkPt);
  TPoint+=TPtr->getLinkAxis(linkPt)*OD;

  std::vector<Geometry::Vec3D> EmptyVec;
  addF5Tally(System,tNum,TPoint,EmptyVec);
  ELog::EM<<"Adding point at "<<TPoint<<ELog::endDiag;
  return;
}

void
pointConstruct::processPointFree(Simulation& System,
				 const std::string& FObject,
				 const long int linkPt,
				 const Geometry::Vec3D& DVec) const
/*!
  Process a point tally in a registered object
  \param System :: Simulation to add tallies
  \param FObject :: Fixed/Twin name
  \param linkPt :: Link point [-ve for beam object]
  \param DVec :: Out distance Distance
*/
{
  ELog::RegMethod RegA("pointConstruct","processPointFree(Vec)");

  const ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

  const attachSystem::FixedComp* TPtr=
    OR.getObjectThrow<attachSystem::FixedComp>(FObject,"FixedComp");
  
  
  const int tNum=System.nextTallyNum(5);
  Geometry::Vec3D TPoint=TPtr->getLinkPt(linkPt);
  
  Geometry::Vec3D XDir,YDir,ZDir;
  TPtr->calcLinkAxis(linkPt,XDir,YDir,ZDir);
  TPoint+=XDir*DVec[0]+YDir*DVec[1]+ZDir*DVec[2];

  std::vector<Geometry::Vec3D> EmptyVec;
  addF5Tally(System,tNum,TPoint,EmptyVec);
  
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
    \param bPlane :: back plane/ cylinder /sphere etc.
    \param EdgeSurf :: In pairs
    \param viewPoint :: Point to start view
    \return Vector of Points
  */
{
  ELog::RegMethod RegA("pointConstruct","calcWindowIntercept");

  std::vector<Geometry::Vec3D> VList;
  Geometry::Vec3D Out;

  for(size_t i=0;i<3;i++)
    {
      for(size_t j=i+1;j<4;j++)
	{
	  if (ModelSupport::calcVertex
	      (bPlane,EdgeSurf[i],EdgeSurf[j],Out,viewPoint))
	    VList.push_back(Out);
	}
    }
  // remove most distant point
  std::sort(VList.begin(),VList.end(),
	    [&viewPoint](const Geometry::Vec3D& A,
			 const Geometry::Vec3D& B) -> bool
	    { return viewPoint.Distance(A)<viewPoint.Distance(B); }
	    );
  VList.resize(4);
  
  return VList;
}

void
pointConstruct::writeHelp(std::ostream& OX) const
  /*!
    Write out help
    \param OX:: Output stream
  */
{
  OX<<
    "free Vec3D -- point detector at point\n"
    "object ObjName link dist -- point detector at "
        " point relative to object\n"
    "objOffset ObjName link Vec3D -- point detector at "
    " point relative to object\n"
    "freeWindow Vec3D Vec3D Vec3D Vec3D Vec3D -- \n"
    "   Point + four coordinates of the window\n"
    "window Item front/back/side distance\n";
  return;
}


}  // NAMESPACE tallySystem
