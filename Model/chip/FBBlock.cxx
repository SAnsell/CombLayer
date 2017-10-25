/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   chip/FBBlock.cxx
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
#include "surfDivide.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Line.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "SimProcess.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "FixedGroup.h"
#include "ContainedComp.h"
#include "SecondTrack.h"
#include "TwinComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "boxValues.h"
#include "boxUnit.h"
#include "BoxLine.h"
#include "HoleUnit.h"
#include "PreCollimator.h"
#include "ColBox.h"
#include "Table.h"
#include "beamBlock.h"
#include "BeamStop.h"
#include "Hutch.h"
#include "FBBlock.h"

#include "Debug.h"

namespace hutchSystem
{

FBBlock::FBBlock(const std::string& Key)  :
  attachSystem::FixedComp(Key,0),nFeed(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param N :: Number of items
  */
{}
  
FBBlock::FBBlock(const FBBlock& A) : 
  attachSystem::FixedComp(A),
  nFeed(A.nFeed),fbLength(A.fbLength),
  Offset(A.Offset),FBcent(A.FBcent),FBsize(A.FBsize)
  /*!
    Copy constructor
    \param A :: FBBlock to copy
  */
{}

FBBlock&
FBBlock::operator=(const FBBlock& A)
  /*!
    Assignment operator
    \param A :: FBBlock to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      nFeed=A.nFeed;
      fbLength=A.fbLength;
      Offset=A.Offset;
      FBcent=A.FBcent;
      FBsize=A.FBsize;
    }
  return *this;
}

FBBlock::~FBBlock() 
  /*!
    Destructor
  */
{}
  
void
FBBlock::populate(const Simulation& System)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("FBBlock","populate");
  
  const FuncDataBase& Control=System.getDataBase();  
  
  nFeed=Control.EvalVar<size_t>(keyName+"NFeed");
  fbLength=Control.EvalVar<double>(keyName+"Len");
  
  FBcent.clear();
  FBsize.clear();
  for(size_t i=0;i<nFeed;i++)
    {  
      const std::string keyIndex(StrFunc::makeString(keyName,i));
      FBcent.push_back(Control.EvalVar<Geometry::Vec3D>
		       (StrFunc::makeString(keyName+"Cent",i)));
      FBsize.push_back(Control.EvalVar<double>
		       (StrFunc::makeString(keyName+"Size",i)));
    }
  return;
}
  
void
FBBlock::createUnitVector(const attachSystem::FixedComp& CUnit)
  /*!
    Create the unit vectors
    - Y Points towards WISH
    - X Across the moderator
    - Z up (towards the target)
    \param CUnit :: Fixed unit that it is connected to 
  */
{
  ELog::RegMethod RegA("FBBlock","createUnitVector");
  FixedComp::createUnitVector(CUnit);
  return;
}
  

void 
FBBlock::insertBlock(Simulation& System,
		     const ChipIRHutch& Hut)
  /*!
    Add a feed pipe to the collimator area
    \param System :: Simulation to add pipe to
    \param Hut :: Hutch item for outside surface intersect
  */
{
  ELog::RegMethod RegA("FBBlock","insertColl");
  Offset=Hut.calcDoorPoint();

  for(size_t i=0;i<FBsize.size();i++)
    {
      // ELog::EM<<"Creating box item "<<i<<" "
      // 	      <<MR.calcRotate(Offset)<<":"
      // 	      <<MR.calcRotate(Offset-X*FBcent[i].X()-Z*FBcent[i].Z())
      // 	      <<ELog::endTrace;
      ModelSupport::BoxLine FTrack(StrFunc::makeString(keyName+"Item",i));
      
      FTrack.addPoint(Offset-X*FBcent[i].X()-Z*FBcent[i].Z());
      FTrack.addPoint(Offset-X*FBcent[i].X()-
		      Z*FBcent[i].Z()-Y*(fbLength+5.0));
      FTrack.addSection(FBsize[i],FBsize[i],0,0.0);
      FTrack.setInitZAxis(Z);
      FTrack.createAll(System);
    }
  
  return;
}
  
void
FBBlock::createAll(Simulation& System,
		       const ChipIRHutch& HutUnit)
  /*!
    Generic function to create everything
    \param System :: Simulation to create objects in
    \param HutUnit :: Hutch point
  */
{
  ELog::RegMethod RegA("FBBlock","createAll");
  
  populate(System);
  createUnitVector(HutUnit);
  insertBlock(System,HutUnit); 

  //  insertPipes(System);       
  
  return;
}
  
}  // NAMESPACE moderatorSystem
