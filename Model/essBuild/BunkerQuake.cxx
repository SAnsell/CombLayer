/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/BunkerQuake.cxx
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#include "MXcards.h"
#include "Zaid.h"
#include "Material.h"
#include "DBMaterial.h"
#include "surfDIter.h"
#include "LayerDivide3D.h"
#include "BunkerQuake.h"


namespace essSystem
{

BunkerQuake::BunkerQuake(const std::string& bunkerName) :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(bunkerName+"Quake",6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param bunkerName :: Name of the bunker object that is building this roof
  */
{}


BunkerQuake::~BunkerQuake() 
  /*!
    Destructor
  */
{}

void
BunkerQuake::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable data base
  */
{
  ELog::RegMethod RegA("BunkerQuake","populate");
  xGap=Control.EvalVar<double>(keyName+"XGap");
  zGap=Control.EvalVar<double>(keyName+"ZGap");

  Geometry::Vec3D APt,BPt;
  const size_t NPt=Control.EvalVar<size_t>(keyName+"NPoint");
  for(size_t index=0;index<NPt;index++)
    {
      const std::string IStr=StrFunc::makeString(index);
      APt=Control.EvalVar<Geometry::Vec3D>(keyName+"PtA"+IStr);
      BPt=Control.EvalVar<Geometry::Vec3D>(keyName+"PtB"+IStr);
      APoint.push_back(APt);
      BPoint.push_back(BPt);
    }
  return;
}
  
void
BunkerQuake::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
/*!
    Create the unit vectors
    \param FC :: Linked object (bunker )
    \param sideIndex :: Side for linkage centre (roof)
  */
{
  ELog::RegMethod RegA("BunkerQuake","createUnitVector");

  FixedComp::createUnitVector(FC,sideIndex);
  return;
}

 
void
BunkerQuake::createAll(Simulation& System,
		       const attachSystem::FixedComp& FC,
		       const long int linkIndex)

  /*!
    Generic function to initialize everything
    \param System :: Simulation to build object in
    \param FC :: Central origin
    \param linkIndex :: linkIndex number
  */
{
  ELog::RegMethod RegA("BunkerQuake","createAll");

  populate(System.getDataBase());  
  createUnitVector(FC,linkIndex);

  return;
}

}  // NAMESPACE essSystem
