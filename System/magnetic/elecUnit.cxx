/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   magnetic/elecUnit.cxx
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
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "Vec3D.h"
#include "stringCombine.h"
#include "writeSupport.h"
#include "phitsWriteSupport.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h" 
#include "FixedComp.h"
#include "FixedRotate.h"
#include "elecUnit.h"

namespace magnetSystem
{

elecUnit::elecUnit(const std::string& Key) :
  attachSystem::FixedRotate(Key,0),
  zeroField(1),index(0)
  /*!
    Constructor (without index)
    \param Key :: Name of construction key
  */
{}

elecUnit::elecUnit(const std::string& Key,
		       const size_t I) :
  attachSystem::FixedRotate(Key+std::to_string(I),0),
  zeroField(1),index(I)
  /*!
    Constructor
    \param Key :: Name of construction key
    \param I :: Index number
  */
{}

elecUnit::elecUnit(const elecUnit& A) : 
  attachSystem::FixedRotate(A),
  zeroField(A.zeroField),index(A.index),
  extent(A.extent),activeCells(A.activeCells)

  /*!
    Copy constructor
    \param A :: elecUnit to copy
  */
{}

elecUnit&
elecUnit::operator=(const elecUnit& A)
  /*!
    Assignment operator
    \param A :: elecUnit to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      zeroField=A.zeroField;
      index=A.index;
      extent=A.extent;
      activeCells=A.activeCells;
    }
  return *this;
}


elecUnit::~elecUnit()
  /*!
    Destructor
   */
{}


void
elecUnit::populate(const FuncDataBase& Control)
  /*!
    If the object is created as a normal object populate
    variables
    \param Control :: DataBase for variaibles
   */
{
  ELog::RegMethod RegA("elecUnit","populate");
  
  FixedRotate::populate(Control);

  
  const double length=Control.EvalDefVar<double>(keyName+"Length",extent[0]);
  const double height=Control.EvalDefVar<double>(keyName+"Height",extent[1]);
  const double width=Control.EvalDefVar<double>(keyName+"Width",extent[2]);
  extent=Geometry::Vec3D(width,length,height);

  double sum(0.0);
  zeroField = (sum>Geometry::zeroTol) ? 0 : 1;
  return;
}

void
elecUnit::setExtent(const double EX,
		    const double EY,
		    const double EZ)
  /*!
    Set the extent based on the vectorized distances
    \param EX :: Width
    \param EY :: Length
    \param EZ :: Height
  */
{
  if (EX>Geometry::zeroTol &&
      EY>Geometry::zeroTol &&
      EZ>Geometry::zeroTol)
    extent=Geometry::Vec3D(EX,EY,EZ);
  else
    extent=Geometry::Vec3D(0,0,0);

  return;
}
  
void
elecUnit::addCell(const int CN)
  /*!
    Add cell to system
    \param CN :: Cell number
   */
{
  activeCells.insert(CN);
  return;
}
		   
void
elecUnit::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const long int sideIndex)
  /*!
    If the object is created as a normal object populate
    variables
    \param System :: Simulation system
    \param FC :: FixedComp for origin / axis
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("elecUnit","createAll");
  
  populate(System.getDataBase());
  elecUnit::createUnitVector(FC,sideIndex);

  return;
}

void
elecUnit::createAll(Simulation& System,
		    const Geometry::Vec3D& OG,
		    const Geometry::Vec3D& AY,
		    const Geometry::Vec3D& AZ,
		    const Geometry::Vec3D& Ext,
		    const std::vector<double>& kValue)
  /*!
    If the object is created as a normal object populate
    variables. 
    \param System :: Simulation system
    \param OG :: New origin
    \param AY :: Y Axis
    \param AZ :: Z Axis [reothorgalizd]
    \param Ext :: XYZ Extent distance [0 in an dimestion for all space]
    \param kValue :: K Value of dipole/quad/hex ...
  */
{
  ELog::RegMethod RegA("elecUnit","createAll");

  this->populate(System.getDataBase());
  elecUnit::createUnitVector(OG,AY,AZ);
  setExtent(Ext[0],Ext[1],Ext[2]); 
  
  double sum(0.0);
  zeroField = (sum>Geometry::zeroTol) ? 0 : 1;
  
  return;
}
  
void
elecUnit::writePHITS(std::ostream& OX) const
  /*!
    Write out the magnetic unit
    Structure is :
    - 0 x y z :: [origin]
    - 2 x1 x2 x3 :: [x axis]
    - 3 y1 y2 y3 :: [y axis]
    - 4 z1 z2 x3 :: [z axis]
    - 5 extX extY extZ :: [Extent vector [optional]]
    - 6 kValue :: [kValue]
    
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("elecUnit","writePHITS");
  
  const std::string magKey="Elec"+std::to_string(index);
  if (zeroField)
    {
      OX<<"# Elec Field: "<<keyName<<" ZERO Field"<<std::endl;
      return;
    }
  /*
  StrFunc::writePHITS(OX,2,"MagID",index);
  StrFunc::writePHITS(OX,2,"Centre",Origin);
  StrFunc::writePHITS(OX,2,"Extent",extent);
  StrFunc::writePHITS(OX,2,"X-Axis",X);
  StrFunc::writePHITS(OX,2,"Y-Axis",Y);
  StrFunc::writePHITS(OX,2,"Z-Axis",Z);

  std::ostringstream cx;
  for(const double kv : KFactor)
    cx<<kv<<" ";
  StrFunc::writePHITS(OX,2,"K-Values",cx.str());
  */
  return;
}



void
elecUnit::writeFLUKA(std::ostream& OX) const
  /*!
    Write out the magnetic unit
    Structure is :
    - 0 x y z :: [origin]
    - 2 x1 x2 x3 :: [x axis]
    - 3 y1 y2 y3 :: [y axis]
    - 4 z1 z2 x3 :: [z axis]
    - 5 extX extY extZ :: [Extent vector [optional]]
    - 6 kValue :: [kValue]
    
    \param OX :: Output stream
   */
{
  ELog::RegMethod RegA("elecUnit","writeFLUKA");

  const std::string elecKey="Elec"+std::to_string(index);
  std::ostringstream cx;

  if (zeroField)
    {
      OX<<"* ElecField: "<<keyName<<" ZERO Field"<<std::endl;
      return;
    }

  OX<<"* Elec Field: "<<keyName<<std::endl;
  cx<<"USRICALL 0 "<<StrFunc::makeString(Origin)<<" - - "<<elecKey;

  StrFunc::writeFLUKA(cx.str(),OX);
  /*
  cx.str("");
  cx<<"USRICALL 1 "<<StrFunc::makeString(X)<<" - - "<<magKey;
  StrFunc::writeFLUKA(cx.str(),OX);

  cx.str("");
  cx<<"USRICALL 2 "<<StrFunc::makeString(Y)<<" - - "<<magKey;
  StrFunc::writeFLUKA(cx.str(),OX);

  cx.str("");
  cx<<"USRICALL 3 "<<StrFunc::makeString(Z)<<" - - "<<magKey;
  StrFunc::writeFLUKA(cx.str(),OX);

  
  if (magExtent[0]>Geometry::zeroTol &&
      magExtent[1]>Geometry::zeroTol &&
      magExtent[2]>Geometry::zeroTol)
    {
      cx.str("");
      cx<<"USRICALL 4 "<<StrFunc::makeString(magExtent)<<" - - "<<magKey;
      StrFunc::writeFLUKA(cx.str(),OX);
    }
  else
    ELog::EM<<"Magnet "<<keyName<<" has no valid extent"<<ELog::endErr;
  cx.str("");
  cx<<"USRICALL 5 ";

  for(size_t i=0;i<4;i++)
    cx<<StrFunc::makeString(KFactor[i])<<" ";
  cx<<" - "<<magKey;

  StrFunc::writeFLUKA(cx.str(),OX,1e-5);  
  */      
 
  return;
}
  

}  // NAMESPACE magnetSystem
