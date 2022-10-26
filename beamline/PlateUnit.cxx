/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   beamline/PlateUnit.cxx 
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#include <functional>
#include <memory>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "polySupport.h"
#include "surfRegister.h"
#include "generateSurf.h"
#include "ModelSupport.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ExternalCut.h"
#include "FrontBackCut.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "Vert2D.h"
#include "Convex2D.h"
#include "GuideUnit.h"

#include "PlateUnit.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Plane.h"

namespace beamlineSystem
{

PlateUnit::PlateUnit(const std::string& key) :
  GuideUnit(key),
  frontCV(new Geometry::Convex2D),
  backCV(new Geometry::Convex2D)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param LS :: Layer separation
    \param ON :: offset number
  */
{}

PlateUnit::PlateUnit(const PlateUnit& A) : 
  GuideUnit(A),
  APts(A.APts),BPts(A.BPts),
  frontCV(new Geometry::Convex2D(*A.frontCV)),
  backCV(new Geometry::Convex2D(*A.backCV))

  /*!
    Copy constructor
    \param A :: PlateUnit to copy
  */
{}

PlateUnit&
PlateUnit::operator=(const PlateUnit& A)
  /*!
    Assignment operator
    \param A :: PlateUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      GuideUnit::operator=(A);
      APts=A.APts;
      BPts=A.BPts;
      *frontCV=*A.frontCV;
      *backCV=*A.backCV;
    }
  return *this;
}

PlateUnit::~PlateUnit() 
  /*!
    Destructor
   */
{}

PlateUnit*
PlateUnit::clone() const 
  /*!
    Clone funciton
    \return *this
   */
{
  return new PlateUnit(*this);
}
  
void 
PlateUnit::setFrontPoints(const std::vector<Geometry::Vec3D>& PVec) 
  /*!
    Calculate the real point based on the offset Origin
    \param PVec :: Points
   */
{
  APts=PVec;
  return;
}
  
void 
PlateUnit::setBackPoints(const std::vector<Geometry::Vec3D>& PVec) 
  /*!
    Set the back points (relative to Origin impact point at O+Y*length)
    \param PVec :: Points
   */
{
  BPts=PVec;
  return;
}

Geometry::Vec3D
PlateUnit::calcFrontPoint(const Geometry::Vec3D& Pt) const
  /*!
    Giving a point return the point in the FixedComp Frame
    \param Pt :: Point on convex
    \return Point in FixedComp Frame
  */
{
  static const Geometry::Vec3D zero(0,0,0);
  return Origin+Pt.getInBasis(X,zero,Z);
}
  
Geometry::Vec3D
PlateUnit::calcBackPoint(const Geometry::Vec3D& Pt) const
  /*!
    Giving a point return the point in the FixedComp Frame
    \param Pt :: Point on convex
    \return Point in FixedComp Frame
  */
{
  static const Geometry::Vec3D zero(0,0,0);

  return Origin+(Y*length)+Pt.getInBasis(X,zero,Z);
}
  
  
void
PlateUnit::populate(const FuncDataBase& Control)
  /*!
    Sets the appropiate APts/BPtrs based on the type of
    guide needed
    \param Control :: DataBase of varaibels
   */
{
  ELog::RegMethod RegA("PlateUnit","populate");

  GuideUnit::populate(Control);
  
  const std::string typeID= 
    Control.EvalVar<std::string>(keyName+"TypeID");
  if (typeID=="Rectangle")   
    {
      APts.clear();
      BPts.clear();
      const double H=Control.EvalVar<double>(keyName+"Height");
      const double W=Control.EvalVar<double>(keyName+"Width");
      APts.push_back(Geometry::Vec3D(-W/2.0,0,-H/2.0));
      APts.push_back(Geometry::Vec3D(W/2.0,0,-H/2.0));
      APts.push_back(Geometry::Vec3D(W/2.0,0,H/2.0));
      APts.push_back(Geometry::Vec3D(-W/2.0,0,H/2.0));
      BPts=APts;
    }
  else if (typeID=="Tapper" || typeID=="Taper")   
    {
      APts.clear();
      BPts.clear();
      const double HA=Control.EvalVar<double>(keyName+"HeightStart");
      const double WA=Control.EvalVar<double>(keyName+"WidthStart");
      const double HB=Control.EvalVar<double>(keyName+"HeightEnd");
      const double WB=Control.EvalVar<double>(keyName+"WidthEnd");
      
      APts.push_back(Geometry::Vec3D(-WA/2.0,0.0,-HA/2.0));
      BPts.push_back(Geometry::Vec3D(-WB/2.0,0.0,-HB/2.0));
      APts.push_back(Geometry::Vec3D(WA/2.0,0.0,-HA/2.0));
      BPts.push_back(Geometry::Vec3D(WB/2.0,0.0,-HB/2.0));
      APts.push_back(Geometry::Vec3D(WA/2.0,0.0,HA/2.0));
      BPts.push_back(Geometry::Vec3D(WB/2.0,0.0,HB/2.0));
      APts.push_back(Geometry::Vec3D(-WA/2.0,0.0,HA/2.0));
      BPts.push_back(Geometry::Vec3D(-WB/2.0,0.0,HB/2.0));
    }
  else if (typeID=="Octagon")   
    {
      APts.clear();
      BPts.clear();
      const double SA=Control.EvalVar<double>(keyName+"WidthStart");
      const double SB=Control.EvalVar<double>(keyName+"WidthEnd");
      const double aA=sqrt(2.0)*SA/(2.0+sqrt(2.0));
      const double aB=sqrt(2.0)*SB/(2.0+sqrt(2.0));
      APts.push_back(Geometry::Vec3D(SA/2.0,0.0,aA/2.0));
      BPts.push_back(Geometry::Vec3D(SB/2.0,0.0,aB/2.0));
      APts.push_back(Geometry::Vec3D(aA/2.0,0.0,SA/2.0));
      BPts.push_back(Geometry::Vec3D(aB/2.0,0.0,SB/2.0));
      APts.push_back(Geometry::Vec3D(-aA/2.0,0.0,SA/2.0));
      BPts.push_back(Geometry::Vec3D(-aB/2.0,0.0,SB/2.0));
      APts.push_back(Geometry::Vec3D(-SA/2.0,0.0,aA/2.0));
      BPts.push_back(Geometry::Vec3D(-SB/2.0,0.0,aB/2.0));
      APts.push_back(Geometry::Vec3D(-SA/2.0,0.0,-aA/2.0));
      BPts.push_back(Geometry::Vec3D(-SB/2.0,0.0,-aB/2.0));
      APts.push_back(Geometry::Vec3D(-aA/2.0,0.0,-SA/2.0));
      BPts.push_back(Geometry::Vec3D(-aB/2.0,0.0,-SB/2.0));
      APts.push_back(Geometry::Vec3D(aA/2.0,0.0,-SA/2.0));
      BPts.push_back(Geometry::Vec3D(aB/2.0,0.0,-SB/2.0));
      APts.push_back(Geometry::Vec3D(SA/2.0,0.0,-aA/2.0));
      BPts.push_back(Geometry::Vec3D(SB/2.0,0.0,-aB/2.0));
    }
  else if (typeID!="Free")
    {
      throw ColErr::InContainerError<std::string>
	(typeID,"TypeID no known");
    }
  return;
}

void
PlateUnit::createSurfaces()
  /*!
    Build the surfaces for the track
   */
{
  ELog::RegMethod RegA("PlateUnit","createSurfaces");


  // First construct hull
  frontCV->setPoints(APts);
  backCV->setPoints(BPts);
  frontCV->constructHull();
  backCV->constructHull();

  if (keyName=="odinFH")
    ELog::EM<<"Hx == "<<Origin<<" "<<yStep<<ELog::endDiag;
  if (keyName=="odinFWall")
    ELog::EM<<"Wx == "<<Origin<<" "<<yStep<<ELog::endDiag;

  if (!isActive("front"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
      setCutSurf("front",SMap.realSurf(buildIndex+1));
    }
  if (!isActive("back"))
    {
      ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*length,Y);
      setCutSurf("back",-SMap.realSurf(buildIndex+2));
    }
  
  double T(0.0);	
  for(size_t i=0;i<layerMat.size();i++)
    {
      int SN(buildIndex+static_cast<int>(i+1)*20+1);  

      // Care here because frontPts/backPts are within
      // APts convex

      const std::vector<Geometry::Vec3D> frontPts=
	frontCV->scalePoints(T);
      const std::vector<Geometry::Vec3D> backPts=
	backCV->scalePoints(T);

      for(size_t j=0;j<APts.size();j++)
	{
	  const size_t jPlus=(j+1) % APts.size();
	  const Geometry::Vec3D PA=calcFrontPoint(frontPts[j]);
	  const Geometry::Vec3D PB=calcFrontPoint(frontPts[jPlus]);
	  const Geometry::Vec3D BA=calcBackPoint(backPts[j]);

	  const Geometry::Vec3D Norm=Origin-PA;
	  Geometry::Plane* PPtr=
	    ModelSupport::buildPlane(SMap,SN,PA,PB,BA,Norm);
	  SN++;
	}
      T+=layerThick[i];
    }   
  return;
}

void
PlateUnit::createObjects(Simulation& System)
  /*!
    Create the objects
    \param System :: Simulation to use
   */
{
  ELog::RegMethod RegA("PlateUnit","createObject");

  const HeadRule fbHR=getFrontRule()*getBackRule();

  HeadRule HR;
  HeadRule innerHR;
  for(size_t i=0;i<layerThick.size();i++)
    {
      int SN(buildIndex+static_cast<int>(i+1)*20+1);
      HR.reset();
      for(size_t j=0;j<APts.size();j++)
	{
	  HR*=HeadRule(SMap,SN);
	  SN++;
	}
      makeCell("Layer"+std::to_string(i),System,
	       cellIndex++,layerMat[i],0.0,HR*fbHR*innerHR);
      innerHR=HR.complement();
    }
  
  addOuterSurf(HR*fbHR);
  return;
  
}
  
void
PlateUnit::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
/*!
    Construct a Bender unit
    \param System :: Simulation to use
    \param FC :: FixedComp to use for basis set
    \param sideIndex :: side link point
   */
{
  ELog::RegMethod RegA("PlateUnit","createAll");

  populate(System.getDataBase());

  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}

}  // NAMESPACE beamlineSystem
