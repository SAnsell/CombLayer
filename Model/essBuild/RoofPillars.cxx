/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/Bunker.cxx
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
  rodIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(rodIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

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

  radius=Control.EvalVar<double>(keyName+"Radius");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  const size_t nRadius=Control.EvalVar<size_t>(keyName+"NRadius");
  for(size_t i=0;i<nRadius;i++)
    {
      const std::string Num=StrFunc::makeString(i);
      const size_t nSector=Control.EvalPair<size_t>
        (keyName+"NSector"+Num,keyName+"NSector");
      for(size_t j=0;j<nSector;j++)
        {
          const std::string NSec=StrFunc::makeString(j);
          // degrees:
          const double angle=M_PI*Control.EvalVar<double>
            (keyName+"Radius"+Num+"Sector"+NSec)/180.0;
          CentPoint.push_back(Geometry::Vec3D(cos(angle),sin(angle),0.0));
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
RoofPillars::setSimpleSurf(const int FS,const int RS)
  /*!
    Set the roof/base surfaces [simple system]
    \param FS :: Floor surface
    \param RS :: Roof surface
  */
{
  ELog::RegMethod RegA("RoofPillars","setSimpleSurf");
  TopSurf.procString(StrFunc::makeString(FS));
  BaseSurf.procString(StrFunc::makeString(RS));
  return;
}
  
void
RoofPillars::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("RoofPillars","createSurface");

  int RI(rodIndex);
  for(const Geometry::Vec3D& Pt : CentPoint)
    ModelSupport::buildCylinder(SMap,RI+7,Origin+Pt,Z,radius);

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

  const std::string Base=
    TopSurf.display()+BaseSurf.display();
  int RI(rodIndex);
  for(size_t i=0;i<CentPoint.size();i++)
    {
      Out=ModelSupport::getComposite(SMap,RI," -7 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,
                                       Out+Base));
      addCell("Pillar",cellIndex-1);
      RI+=10;
    }
        
  return;
}

void
RoofPillars::insertPillars(Simulation& System,
                           const attachSystem::CellMap& bObj)
  /*!
    Insert the pillers into the bunker void
    \param System :: Simulation 
    \param bObj :: bunker object
   */
{
  ELog::RegMethod RegA("RoofPillars","insertPillars");

  std::string Out;
  int RI(rodIndex);
  for(size_t i=0;i<CentPoint.size();i++)
    {
      Out=ModelSupport::getComposite(SMap,RI," 7 ");
      bObj.insertComponent(System,"mainVoid",Out);
      RI+=10;
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
  createUnitVector(bunkerObj,0);
  createSurfaces();
  createObjects(System);
  insertPillars(System,bunkerObj);
  return;
}

}  // NAMESPACE essSystem
