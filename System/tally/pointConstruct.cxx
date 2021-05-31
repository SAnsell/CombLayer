/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tally/pointConstruct.cxx
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
#include <iterator>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "Vec3D.h"
#include "TallyCreate.h"
#include "surfRegister.h"
#include "HeadRule.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "SimMCNP.h"
#include "inputParam.h"

#include "SpecialSurf.h"
#include "pointConstruct.h" 


namespace tallySystem
{

void
pointConstruct::processPoint(SimMCNP& System,
			     const mainSystem::inputParam& IParam,
			     const size_t Index)
  /*!
    Add point tally (s) as needed
    \param System :: SimMCNP to add tallies
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

  if (PType=="free" || PType=="Free")
    {
      std::vector<Geometry::Vec3D> EmptyVec;
      size_t itemIndex(2);
      Geometry::Vec3D PPoint=IParam.getCntVec3D
	("tally",Index,itemIndex,"Point for point detector");
      processPointFree(System,PPoint,EmptyVec);
    }

  else if (PType=="freeWindow")
    {
      size_t itemIndex(2);
      Geometry::Vec3D PPoint=
	IParam.getCntVec3D("tally",Index,itemIndex,"Point for point detector");
      
      std::vector<Geometry::Vec3D> WindowPts(4);
      for(size_t i=0;i<4;i++)
	WindowPts[i]=IParam.getCntVec3D
	  ("tally",Index,itemIndex,"Window point "+std::to_string(i+1));
      
      processPointFree(System,PPoint,WindowPts);
    }
  else if (PType=="window")
    {
      const std::string fcName=
	IParam.getValueError<std::string>("tally",Index,2,"position not given");
      const std::string linkPoint=
	IParam.getValueError<std::string>("tally",Index,3,"front/back/side not given");

      const double D=
	IParam.getValueError<double>("tally",Index,4,"Distance not given");


      const double timeStep=
	IParam.getDefValue<double>(0.0,"tally",Index,5);
      const double windowOffset=
	IParam.getDefValue<double>(0.0,"tally",Index,6);
	    

      processPointWindow(System,fcName,linkPoint,D,timeStep,windowOffset);
    }

  else if (PType=="object")
    {
      const std::string place=
	IParam.getValueError<std::string>("tally",Index,2,"position not given");
      const std::string linkName=
	IParam.getValueError<std::string>("tally",Index,3,"front/back/side not given");
      const double D=
	IParam.getValueError<double>("tally",Index,4,"Distance not given");
      processPointFree(System,place,linkName,D);
    }
  else if (PType=="objOffset")
    {
      const std::string place=
	IParam.getValueError<std::string>("tally",Index,2,"position not given");
      const std::string linkName=
	IParam.getValueError<std::string>("tally",Index,3,"front/back/side not give");

      size_t itemIndex(4);
      const Geometry::Vec3D DVec=
	IParam.getCntVec3D("tally",Index,itemIndex,"Offset");
      
      processPointFree(System,place,linkName,DVec);
    }
  else
    {
      ELog::EM<<"Point TallyType "<<PType<<" ignored"<<ELog::endWarn;
    }
  

  return;
}

void
pointConstruct::processPointWindow(SimMCNP& System,
				   const std::string& FObject,
				   const std::string& linkIndex,
				   const double beamDist,
				   const double timeStep,
				   const double windowOffset)
  /*!
    Process a point tally in a registered object
    \param System :: SimMCNP to add tallies
    \param FObject :: Fixed/Twin name
    \param linkIndex :: Link point 
    \param beamDist :: Out distance Distance
    \param timeStep ::  Forward step [view type modification]
    \param windowOffset :: Distance to move window towards tally point
  */
{
  ELog::RegMethod RegA("pointConstruct","processPointWindow");

  std::vector<int> Planes;
  const int tNum=System.nextTallyNum(5);
  Geometry::Vec3D TPoint;
  Geometry::Vec3D orgPoint;
  Geometry::Vec3D BAxis;
  int masterPlane(0);
  const attachSystem::FixedComp* TPtr=
    System.getObjectThrow<attachSystem::FixedComp>(FObject,"FixedComp");

  const long int linkPt=TPtr->getSideIndex(linkIndex);
  if (linkPt!=0)
    {
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
pointConstruct::processPointFree(SimMCNP& System,
				 const std::string& FObject,
				 const std::string& linkName,
				 const double OD)
  /*!
    Process a point tally in a registered object
    \param System :: SimMCNP to add tallies
    \param FObject :: Fixed/Twin name
    \param linkNAme :: Link point 
    \param OD :: Out distance Distance
  */
{
  ELog::RegMethod RegA("pointConstruct","processPointFree(String)");

  const attachSystem::FixedComp* TPtr=
    System.getObjectThrow<attachSystem::FixedComp>(FObject,"FixedComp");
  const long int linkPt=TPtr->getSideIndex(linkName);
  
  const int tNum=System.nextTallyNum(5);
  Geometry::Vec3D TPoint=TPtr->getLinkPt(linkPt);
  TPoint+=TPtr->getLinkAxis(linkPt)*OD;

  std::vector<Geometry::Vec3D> EmptyVec;
  addF5Tally(System,tNum,TPoint,EmptyVec);
  ELog::EM<<"Adding point at "<<TPoint<<ELog::endDiag;
  return;
}

void
pointConstruct::processPointFree(SimMCNP& System,
				 const std::string& FObject,
				 const std::string& linkName,
				 const Geometry::Vec3D& DVec)
/*!
  Process a point tally in a registered object
  \param System :: SimMCNP to add tallies
  \param FObject :: Fixed/Twin name
  \param linkName :: Link point 
  \param DVec :: Out distance Distance
*/
{
  ELog::RegMethod RegA("pointConstruct","processPointFree(Vec)");

  const attachSystem::FixedComp* TPtr=
    System.getObjectThrow<attachSystem::FixedComp>(FObject,"FixedComp");
  
  const long int linkPt=TPtr->getSideIndex(linkName);
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
pointConstruct::processPointFree(SimMCNP& System,
				 const Geometry::Vec3D& Point,
				 const std::vector<Geometry::Vec3D>& VList)
  /*!
    Processes a grid tally : Requires variables and informaton 
    \param System :: SimMCNP to add tallies
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
pointConstruct::writeHelp(std::ostream& OX) 
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
