/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/RoofPillars.cxx
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
#include "stringCombine.h"
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
#include "inputParam.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ReadFunctions.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FrontBackCut.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"

#include "World.h"
#include "Bunker.h"
#include "RoofPillars.h"


namespace essSystem
{

RoofPillars::RoofPillars(const std::string& Key)  :
  attachSystem::FixedComp(Key,0),
  attachSystem::CellMap(),
  attachSystem::FrontBackCut(),
  rodIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(rodIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

RoofPillars::RoofPillars(const RoofPillars& A) : 
  attachSystem::FixedComp(A),attachSystem::CellMap(A),
  attachSystem::FrontBackCut(A),
  rodIndex(A.rodIndex),cellIndex(A.cellIndex),
  CentName(A.CentName),CentPoint(A.CentPoint),
  width(A.width),depth(A.depth),thick(A.thick),mat(A.mat)
  /*!
    Copy constructor
    \param A :: RoofPillars to copy
  */
{}

RoofPillars&
RoofPillars::operator=(const RoofPillars& A)
  /*!
    Assignment operator
    \param A :: RoofPillars to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::FrontBackCut::operator=(A);
      cellIndex=A.cellIndex;
      CentName=A.CentName;
      CentPoint=A.CentPoint;
      width=A.width;
      depth=A.depth;
      thick=A.thick;
      mat=A.mat;
    }
  return *this;
}

RoofPillars::~RoofPillars() 
  /*!
    Destructor
  */
{}

void
RoofPillars::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("RoofPillars","populate");

  width=Control.EvalVar<double>(keyName+"Width");
  depth=Control.EvalVar<double>(keyName+"Depth");
  thick=Control.EvalVar<double>(keyName+"Thick");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  topFootHeight=Control.EvalDefVar<double>(keyName+"TopFootHeight",0.0);
  topFootDepth=Control.EvalDefVar<double>(keyName+"TopFootDepth",0.0);
  topFootWidth=Control.EvalDefVar<double>(keyName+"TopFootWidth",0.0);
  topFootThick=Control.EvalDefVar<double>(keyName+"TopFootThick",0.0);
  topFootGap=Control.EvalDefVar<double>(keyName+"TopFootGap",0.0);

  const size_t nRadius=Control.EvalVar<size_t>(keyName+"NRadius");
  for(size_t i=0;i<nRadius;i++)
    {
      const std::string Num=StrFunc::makeString(i);
      const size_t nSector=Control.EvalPair<size_t>
        (keyName+"NSector"+Num,keyName+"NSector");
      const double rotRadius=Control.EvalPair<double>
        (keyName+"R"+Num,keyName+"R");
      
      for(size_t j=0;j<nSector;j++)
        {
          const std::string NSec=StrFunc::makeString(j);
          const int active=Control.EvalDefPair<int>
            (keyName+"R_"+Num+"S_"+NSec+"Active",
             keyName+"RS_"+NSec+"Active",1);

          if (active)
            {
              // degrees:
              const double angle=M_PI*Control.EvalPair<double>
                (keyName+"R_"+Num+"S_"+NSec,keyName+"RS_"+NSec)/180.0;
              CentName.push_back("R_"+Num+"S_"+NSec);
              CentPoint.push_back
                (Geometry::Vec3D(rotRadius*sin(angle),
                                 rotRadius*cos(angle),0.0));
              AxisY.push_back
                (Geometry::Vec3D(sin(angle),cos(angle),0.0));
            }
        }          
    }
  
  return;
}
  
void
RoofPillars::createUnitVector(const attachSystem::FixedComp& MainCentre,
			      const long int sideIndex)
  /*!
    Create the unit vectors
    \param MainCentre :: Main rotation centre
    \param sideIndex :: Side for linkage centre
  */
{
  ELog::RegMethod RegA("RoofPillars","createUnitVector");

  FixedComp::createUnitVector(MainCentre,sideIndex);
  for(Geometry::Vec3D& Pt : CentPoint)
    Pt=X*Pt.X()+Y*Pt.Y()+Z*Pt.Z();

  return;
}
  
void
RoofPillars::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("RoofPillars","createSurface");

  if (topFootHeight>Geometry::zeroTol)
    {
      int RI(rodIndex);
      const std::set<int> FS=  topFoot.getSurfSet();
      for(const int& SNum : FS)
        {
          int footIndex(RI);  // in case multiple surfaces
          const Geometry::Surface* SPtr=SMap.realSurfPtr(SNum);
          const Geometry::Plane* PPtr=
            dynamic_cast<const Geometry::Plane*>(SPtr);
          if (PPtr)
            {
              // top plate
              if (SNum>0)
                ModelSupport::buildShiftedPlane
                    (SMap,footIndex+6,PPtr,topFootHeight-topFootThick);
              else
                ModelSupport::buildShiftedPlaneReversed
                  (SMap,footIndex+6,PPtr,topFootHeight-topFootThick);
              topFootPlate.addIntersection(SMap.realSurf(footIndex+6));
              
              // Full height
              if (SNum>0)
                ModelSupport::buildShiftedPlane
                    (SMap,footIndex+16,PPtr,topFootHeight);
              else
                ModelSupport::buildShiftedPlaneReversed
                  (SMap,footIndex+16,PPtr,topFootHeight);
              topFoot.addIntersection(SMap.realSurf(footIndex+16));
              footIndex++;
            }
        }
      RI+=50;
    }
  
  int RI(rodIndex);
  for(size_t index=0;index<CentPoint.size();index++)
    {
      
      const Geometry::Vec3D CP(Origin+CentPoint[index]);
      const Geometry::Vec3D YAxis=AxisY[index];
      const Geometry::Vec3D XAxis=Z*YAxis;
      double WT(width/2.0);
      double DT(depth/2.0);      
      ModelSupport::buildPlane(SMap,RI+1,CP-XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+2,CP+XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+3,CP-YAxis*DT,YAxis);
      ModelSupport::buildPlane(SMap,RI+4,CP+YAxis*DT,YAxis);

      WT+=thick;
      DT+=thick;      
      ModelSupport::buildPlane(SMap,RI+11,CP-XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+12,CP+XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+13,CP-YAxis*DT,YAxis);
      ModelSupport::buildPlane(SMap,RI+14,CP+YAxis*DT,YAxis);

      WT=topFootWidth/2.0;
      DT=topFootDepth/2.0;
      ModelSupport::buildPlane(SMap,RI+21,CP-XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+22,CP+XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+23,CP-YAxis*DT,YAxis);
      ModelSupport::buildPlane(SMap,RI+24,CP+YAxis*DT,YAxis);

      
      
      WT+=topFootGap;
      DT+=topFootGap;
      ModelSupport::buildPlane(SMap,RI+31,CP-XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+32,CP+XAxis*WT,XAxis);
      ModelSupport::buildPlane(SMap,RI+33,CP-YAxis*DT,YAxis);
      ModelSupport::buildPlane(SMap,RI+34,CP+YAxis*DT,YAxis);
      
      RI+=50;
    }
      
  return;
}

void
RoofPillars::createObjects(Simulation& System)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("RoofPillars","createObjects");

  std::string Out;

  const std::string Base=frontRule()+backRule();
  int RI(rodIndex);
  for(size_t i=0;i<CentPoint.size();i++)
    {
      Out=ModelSupport::getComposite(SMap,RI," 1 -2 3 -4 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+Base));
      addCell("Pillar"+CentName[i],cellIndex-1);
      Out=ModelSupport::getComposite(SMap,RI," 11 -12 13 -14 (-1:2:-3:4) ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+Base));
      addCell("Pillar"+CentName[i],cellIndex-1);

      RI+=50;
    }
        
  return;
}

void
RoofPillars::insertPillars(Simulation& System,
                           const attachSystem::CellMap& bObj)
  /*!
    Insert the pillers into the bunker void
    \param System :: Simulation 
    \param bObj :: Bunker void [cells]
   */
{
  ELog::RegMethod RegA("RoofPillars","insertPillars");

  const std::string Base=frontRule()+backRule();    
  std::string Out;
  int RI(rodIndex);
  for(size_t i=0;i<CentPoint.size();i++)
    {
      Out=ModelSupport::getComposite(SMap,RI," 11 -12 13 -14 ");
      HeadRule HR(Out);
      HR.makeComplement();
      bObj.insertComponent(System,"MainVoid",HR);
      RI+=50;
    }
  return;
}

  
void
RoofPillars::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("RoofPillars","createLinks");

  return;
}
  
void
RoofPillars::createAll(Simulation& System,
                       const Bunker& bunkerObj)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param bunkerObj :: Bunker object 
  */
{
  ELog::RegMethod RegA("RoofPillars","createAll");

  populate(System.getDataBase());
  createUnitVector(bunkerObj,7);

  setFront(bunkerObj,11);    // floor
  setBack(bunkerObj,12);     // roof [inner]

  createSurfaces();
  createObjects(System);
  insertPillars(System,bunkerObj);
  return;
}

}  // NAMESPACE essSystem
