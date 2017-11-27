/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BunkerQUnit.cxx
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
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "SurfMap.h"
#include "FrontBackCut.h"
#include "MXcards.h"
#include "Zaid.h"
#include "Material.h"
#include "DBMaterial.h"
#include "surfDIter.h"
#include "LayerDivide3D.h"
#include "insertObject.h"
#include "insertPlate.h"
#include "insertCurve.h"
#include "addInsertObj.h"
#include "BunkerQUnit.h"


namespace essSystem
{

BunkerQUnit::BunkerQUnit(const std::string& key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(key,6),
  cutIndex(ModelSupport::objectRegister::Instance().cell(keyName,20000)),
  cellIndex(cutIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param bunkerName :: Name of the bunker object that is building this roof
  */
{}

BunkerQUnit::BunkerQUnit(const BunkerQUnit& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),attachSystem::SurfMap(A),
  cutIndex(A.cutIndex),
  cellIndex(A.cellIndex),xGap(A.xGap),zGap(A.zGap),
  PFlag(A.PFlag),cPts(A.cPts),Radii(A.Radii),yFlag(A.yFlag)
  /*!
    Copy constructor
    \param A :: BunkerQUnit to copy
  */
{}

BunkerQUnit&
BunkerQUnit::operator=(const BunkerQUnit& A)
  /*!
    Assignment operator
    \param A :: BunkerQUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      attachSystem::SurfMap::operator=(A);
      cellIndex=A.cellIndex;
      xGap=A.xGap;
      zGap=A.zGap;
      PFlag=A.PFlag;
      cPts=A.cPts;
      Radii=A.Radii;
      yFlag=A.yFlag;
    }
  return *this;
}


BunkerQUnit::~BunkerQUnit() 
  /*!
    Destructor
  */
{}

void
BunkerQUnit::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BunkerQUnit","populate");

  attachSystem::FixedOffset::populate(Control);

  xGap=Control.EvalVar<double>(keyName+"XGap");
  zGap=Control.EvalVar<double>(keyName+"ZGap");

  Geometry::Vec3D APt,BPt;
  const size_t NPt=Control.EvalVar<size_t>(keyName+"NPoint");

  int flag(1);
  int yF;
  double R;
  for(size_t index=0;index<NPt;index++)
    {
      const std::string IStr=StrFunc::makeString(index);
      flag=Control.EvalDefVar<int>(keyName+"PtFlag"+IStr,flag);
      APt=Control.EvalVar<Geometry::Vec3D>(keyName+"PtA"+IStr);
      yF=Control.EvalDefVar(keyName+"YFlag"+IStr,0);
      R=Control.EvalDefVar(keyName+"Radius"+IStr,-1.0);
      cPts.push_back(APt);
      PFlag.push_back(flag);
      Radii.push_back(R);
      yFlag.push_back(yF);
    }
  return;
}
  
void
BunkerQUnit::createUnitVector(const attachSystem::FixedComp& FC,
                              const long int orgIndex,
                              const long int axisIndex)
  /*!
    Create the unit vectors
    \param FC :: Linked object (bunker )
    \param orgIndex :: origin point [lower part of roof]
    \param axisIndex :: axis index direction
  */
{
  ELog::RegMethod RegA("BunkerQUnit","createUnitVector");

  FixedComp::createUnitVector(FC,axisIndex);
  Origin=FC.getLinkPt(orgIndex);
  applyOffset();

  return;
}

void
BunkerQUnit::modifyPoints()
  /*!
    Modify the points to use the origin (or just the z origin)
   */
{
  for(size_t index=0;index<cPts.size();index++)
    {
      if (PFlag[index]==1)
        {
          cPts[index]+=Z*Origin.Z();
	  cPts[index]+=X*xStep+Y*yStep;   // add step from offset
        }
      else if (PFlag[index]==2)
        {
          cPts[index]=Origin+
            X*cPts[index].X()+Y*cPts[index].Y()+Z*cPts[index].Z();
        }
    }        
  return;
}


void
BunkerQUnit::createObjects(Simulation& System)
  /*!
    Create all the objects
    \param System :: Simulation to use
   */
{
  ELog::RegMethod RegA("BunkerQUnit","createObjects");
  
  for(size_t index=1;index<cPts.size();index++)
    {
      const std::string ItemName(keyName+"Cut"+StrFunc::makeString(index));
      const double radius(Radii[index-1]);
      const int yF(yFlag[index-1]);
      if ((radius<Geometry::zeroTol) || !yF)
        {
          const Geometry::Vec3D YDir((cPts[index]-cPts[index-1]).unit());
          const double yGap=cPts[index].Distance(cPts[index-1]);
          
          constructSystem::addInsertPlateCell
            (System,ItemName,(cPts[index-1]+cPts[index])/2.0+Z*(zGap/2.0),
             YDir,Z,xGap,yGap,zGap,"Void");
        }
      else  // cylinder
        {
          constructSystem::addInsertCurveCell
            (System,ItemName,cPts[index-1]+Z*(zGap/2.0),
             cPts[index]+Z*(zGap/2.0),yF,Z,radius,xGap,zGap,
             "Void");
        }
    }      

  return;
}
 
void
BunkerQUnit::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int orgIndex,
                       const long int axisIndex)

  /*!
    Generic function to initialize everything
    \param System :: Simulation to build object in
    \param FC :: Central origin
    \param orgIndex :: origin point [lower part of roof]
    \param axisIndex :: axis index direction
  */
{
  ELog::RegMethod RegA("BunkerQUnit","createAll");

  populate(System.getDataBase());  
  createUnitVector(FC,orgIndex,axisIndex);
  modifyPoints();
  createObjects(System);
  return;
}

}  // NAMESPACE essSystem
