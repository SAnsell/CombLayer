/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   build/TS1build.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <algorithm>
#include <iterator>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Surface.h"
#include "Rules.h"
#include "Code.h"
#include "varList.h"
#include "FuncDataBase.h"
#include "KGroup.h"
#include "Source.h"
#include "SimProcess.h"
#include "SpecialSurf.h"
#include "Simulation.h"

namespace TS1Build
{

void
registerPlane(std::set<int>& setGrp,const double DV,
	      const int PN,const Geometry::Vec3D& Norm,
	      const Geometry::Vec3D& InterceptPt)
  /*!
    Register a plane to the system.
    \param setGrp :: list of planes added
    \param DV :: deciding value
    \param PN :: Plane number 
    \param Norm :: Normal of plane
    \param InterceptPt :: Intercept point on surface
   */
{
  if (DV>0.0)
    {
      setGrp.insert(PN);
      ModelSupport::addPlane(PN,Norm,InterceptPt);
    }
  return;
}

std::string
createBox(const std::set<int>& setGrp,const std::vector<int>& Master,
	  const std::vector<int>& Inner)
  /*!
    System for creating the string for a box.
    e.g. inner + outer
    \param setGrp :: set surfaces 
    \param Master :: Master numbers
    \param Inner :: Inner numbers
    \return String 
  */
{
  const int SS=static_cast<int>(setGrp.size());
  if (SS<1) return ""; 

  std::ostringstream cx;
  const size_t iSize=Inner.size();

  // HANDLE ONE EDGE
  if (SS==1)
    {
      for(size_t i=0;i<iSize;i++)
	{
	  if (setGrp.find(abs(Inner[i]))!=setGrp.end())
	    {
	      const size_t mM= (i==0) ? iSize-1 : i-1;
	      const size_t mP= (i==iSize-1) ? 0 : i+1;
	      cx<<Inner[i]<<" "<<Master[i]<<" "<<Master[mM]<<" "<<Master[mP];
	      return cx.str();
	    }
	}
    }
  // PAIR/OPPOSITE EDGES
  std::vector<size_t> Index;
  for(size_t i=0;i<iSize;i++)
    {
      if (setGrp.find(abs(Inner[i]))!=setGrp.end())        // GOT FIRST POINT
	Index.push_back(i);
    }
  if (Index.empty())
    return "";

  std::set<size_t> Item;
  std::vector<size_t>::const_iterator vc;
  for(vc=Index.begin();vc!=Index.end();vc++)
    {
      Item.insert(*vc);
      Item.insert((*vc==0) ? iSize-1 : *vc-1);
      Item.insert((*vc==iSize-1) ? 0 : *vc+1);
      cx<<Inner[*vc]<<" ";
    }
  std::set<size_t>::const_iterator mc;
  for(mc=Item.begin();mc!=Item.end();mc++)
    cx<<Master[*mc]<<" ";

  return cx.str();
}

void
addMethaneFlightLines(Simulation& System)
  /*!
    Add flight lines 
    \param System :: Simulation object
   */
{
  ELog::RegMethod RegA("TS1build","addMethantFlightLines");
  FuncDataBase& Control=System.getDataBase();


  const Geometry::Vec3D X(1,0,0);     

  std::vector<Geometry::Vec3D> OutA;
  std::vector<Geometry::Vec3D> OutB;

 
  const double AlFlightA=Control.EvalVar<double>("MethFlightAlFar");
  const double AlFlightB=Control.EvalVar<double>("MethFlightAlNear");
  //  const double AlFlightC=Control.EvalVar<double>("H2FlightAlFar");
  //  const double AlFlightD=Control.EvalVar<double>("H2FlightAlOut");
  //  const double AlFlightE=Control.EvalVar<double>("H2FlightAlNear");

  const double AlFlightRoof=Control.EvalVar<double>("MethFlightAlRoof");
  const double AlFlightFloor=Control.EvalVar<double>("MethFlightAlFloor");


  const double CdFlightA=Control.EvalVar<double>("MethFlightCdFar");
  const double CdFlightB=Control.EvalVar<double>("MethFlightCdNear");
  //  const double CdFlightC=Control.EvalVar<double>("H2FlightCdFar");
  //  const double CdFlightD=Control.EvalVar<double>("H2FlightCdOut");
  //  const double CdFlightE=Control.EvalVar<double>("H2FlightCdNear");

  const double CdFlightRoof=Control.EvalVar<double>("MethFlightCdRoof");
  const double CdFlightFloor=Control.EvalVar<double>("MethFlightCdFloor");



  // METHANE flight 
  Control.Parse("coldCentre-coldViewHeight");
  const double baseD=Control.Eval();
  Control.Parse("coldCentre+coldViewHeight");
  const double roofD=Control.Eval();
  const double MA=Control.EvalVar<double>("methAngleHSide");
  //  const double MM=Control.EvalVar<double>("methAngleFar");
  const double hSep=Control.EvalVar<double>("methViewHorr")/2.0;
   
  //  const int cdMat=7; 
 
  // -------------------------------------------------------------------
  //                           FAR SIDE
  // -------------------------------------------------------------------
  // Normal on far side
  std::set<int> CdWalls;           ///< Keep a list of cd walls
  std::set<int> AlWalls;           ///< Keep a list of al walls
  std::vector<int> Be;
  std::vector<int> InnerAl;
  std::vector<int> InnerCd;
  // 
  // Push back indexes
  // 
  Be.push_back(-5001); InnerCd.push_back(-5101);  InnerAl.push_back(-5201);
  Be.push_back(5005); InnerCd.push_back(5105);  InnerAl.push_back(5205);
  Be.push_back(5002); InnerCd.push_back(5102);  InnerAl.push_back(5202);
  Be.push_back(-5006); InnerCd.push_back(-5106);  InnerAl.push_back(-5206);

  Geometry::Vec3D Nvec=Geometry::Vec3D(0.0,cos(MA*M_PI/180.0),-sin(MA*M_PI/180.0));
  // Now get Intercept points on outside edge and creep in
  if (!ModelSupport::calcVertex(10001,10003,202,OutA,0) ||
      !ModelSupport::calcVertex(10001,10004,202,OutB,0) )
    {
      ELog::EM<<"Error finding intercept for 10001 10003/4 202"<<ELog::endErr;
      exit(1);
    }
  Geometry::Vec3D Diff=(OutB[0]-OutA[0])/2.0;
  const double S=Diff.abs();
  Geometry::Vec3D Mid=(OutA[0]+OutB[0])/2.0;

  const Geometry::Vec3D FMsplit=Diff*(hSep/S);
  ModelSupport::addPlane(5001,Nvec,Mid-FMsplit);
  registerPlane(CdWalls,CdFlightB,5101,Nvec,Mid-FMsplit-Nvec*(AlFlightB+CdFlightB));
  registerPlane(AlWalls,AlFlightB,5201,Nvec,Mid-FMsplit-Nvec*AlFlightB);

  ModelSupport::addPlane(5002,Nvec,Mid+FMsplit);
  registerPlane(CdWalls,CdFlightA,5102,Nvec,Mid+FMsplit+Nvec*(AlFlightA+CdFlightA));
  registerPlane(AlWalls,AlFlightA,5202,Nvec,Mid+FMsplit+Nvec*AlFlightA);
  

  ModelSupport::addPlane(5005,X,X*baseD);
  registerPlane(CdWalls,CdFlightFloor,5205,X,X*(baseD+CdFlightFloor+AlFlightFloor));
  registerPlane(AlWalls,AlFlightFloor,5105,X,X*(baseD+AlFlightFloor));

  ModelSupport::addPlane(5006,X,X*roofD);
  registerPlane(CdWalls,CdFlightFloor,5205,X,X*(roofD+CdFlightRoof+AlFlightFloor));
  registerPlane(AlWalls,AlFlightFloor,5105,X,X*(roofD+AlFlightRoof));

  /*
  Geometry::Vec3D CP(coldCentre-coldVei
  // 5005 
  registerPlane(CdWalls,CdFlightA,5105,X,Mid+FMsplit+Nvec*(AlFlightA+CdFlightA));
  registerPlane(AlWalls,AlFlightA,5205,Nvec,Mid+FMsplit+Nvec*AlFlightA);
  */
  ELog::EM<<"Al string "<<createBox(CdWalls,Be,InnerCd)<<ELog::endDiag;
  //  exit(1);
//   //  System.addCell(MonteCarlo::Qhull(61001,cdMat,0.0,""));  
  
//   Nvec=Geometry::Vec3D(0.0,cos(MM*M_PI/180.0),-sin(MM*M_PI/180.0));
//   ModelSupport::addPlane(5002,Nvec,Mid+FMsplit);
// //  ModelSupport::addPlane(5001,Nvec,10001,10003,202);




// //  ModelSupport::addPlane(5002,Nvec,10001,10004,202);
  
//   //
//   // Methane other Flight
//   //
//   // METHANE flight
//   const double HA=Control.EvalVar<double>("HMAngleHSide");
//   const double HM=Control.EvalVar<double>("HMAngleFarSide");

//   if (!System.calcVertex(10002,10003,202,OutA) ||
//       !System.calcVertex(10002,10004,202,OutB) )
//     {
//       ELog::EM.error("Error finding intercept for 10002 10003/4 202");
//       exit(1);
//     }

//   Diff=(OutB[0]-OutA[0])/2.0;
//   S=Diff.abs();
//   Mid=(OutA[0]+OutB[0])/2.0;

//   // Create Flight Edge:
//   Nvec=Geometry::Vec3D(0.0,cos(HA*M_PI/180.0),-sin(HA*M_PI/180.0));  
//   ModelSupport::addPlane(5011,Nvec,Mid-Diff*(hSep/S));
//   ModelSupport::addPlane(5111,Nvec,Mid-Diff*(hSep/S)-Nvec*AlFlightD);
//   ModelSupport::addPlane(5211,Nvec,Mid-Diff*(hSep/S)-Nvec*(AlFlightD+CdFlightD));
//   Nvec=Geometry::Vec3D(0.0,cos(HM*M_PI/180.0),-sin(HM*M_PI/180.0));
//   ModelSupport::addPlane(5012,Nvec,Mid+Diff*(hSep/S));
//   ModelSupport::addPlane(5112,Nvec,Mid+Diff*(hSep/S)+Nvec*AlFlightE);
//   ModelSupport::addPlane(5212,Nvec,Mid+Diff*(hSep/S)+Nvec*(AlFlightE+CdFlightE));
 
//   // Hydrogen flight

//   Control.Parse("HAngleSide");
//   const double HX=Control.Eval();
//   Nvec=Geometry::Vec3D(0.0,cos(HX*M_PI/180.0),-sin(HX*M_PI/180.0));
//   ModelSupport::addPlane(5021,Nvec,20002,20003,202);

  return;
}

} // NAMESPACE TS1build
