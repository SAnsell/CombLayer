/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaMagnetic/magnetDipole.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "stringCombine.h"
#include "writeSupport.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Quadratic.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "magnetUnit.h"
#include "magnetDipole.h"

namespace flukaSystem
{

magnetDipole::magnetDipole(const std::string& Key,
		       const size_t I) :
  magnetUnit(Key,I),KFactor(0.0),
  Radius(0.0)
  /*!
    Constructor
    \param Key :: Name of construction key
    \param I :: Index number
  */
{}

magnetDipole::magnetDipole(const magnetDipole& A) : 
  magnetUnit(A),
  KFactor(A.KFactor),Radius(A.Radius)
  /*!
    Copy constructor
    \param A :: magnetDipole to copy
  */
{}

magnetDipole&
magnetDipole::operator=(const magnetDipole& A)
  /*!
    Assignment operator
    \param A :: magnetDipole to copy
    \return *this
  */
{
  if (this!=&A)
    {
      magnetUnit::operator=(A);
      KFactor=A.KFactor;
      Radius=A.Radius;
    }
  return *this;
}

magnetDipole::~magnetDipole()
  /*!
    Destructor
  */
{}

void
magnetDipole::populate(const FuncDataBase& Control)
  /*!
    If the object is created as a normal object populate
    variables
    \param Control :: DataBase for variaibles
   */
{
  ELog::RegMethod RegA("magnetDipole","populate");


  magnetUnit::populate(Control);
  
  KFactor=Control.EvalDefVar<double>(keyName+"KFactor",KFactor);
  Radius=Control.EvalDefVar<double>(keyName+"Radius",Radius);
  return;
}

void
magnetDipole::createAll(Simulation& System,
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
  ELog::RegMethod RegA("magnetDipole","createAll");
  
  populate(System.getDataBase());
  magnetUnit::createUnitVector(FC,sideIndex);

  return;
}

void
magnetDipole::createAll(Simulation& System,
		      const Geometry::Vec3D& OG,
		      const Geometry::Vec3D& AY,
		      const Geometry::Vec3D& AZ,
		      const Geometry::Vec3D& extent,
		      const double kValue,const double R)
  /*!
    If the object is created as a normal object populate
    variables
    \param System :: Simulation system
    \param OG :: New origin
    \param AY :: Y Axis
    \param AZ :: Z Axis [reothorgalizd]
    \param extent :: XYZ Extent distance [0 in an dimestion for all space]
    \param kValue :: K Value of quadrupole
  */
{
  ELog::RegMethod RegA("magnetDipole","createAll");

  this->populate(System.getDataBase());
  magnetUnit::createUnitVector(OG,AY,AZ);
  setExtent(extent[0],extent[1],extent[2]);
  
  KFactor=kValue;
  Radius=R;
  
  return;
}

void
magnetDipole::writeFLUKA(std::ostream& OX) const
  /*!
    Write out the magnetic unit
    \param OX :: Output stream
  */
{
  ELog::RegMethod RegA("magnetDipole","writeFLUKA");
  magnetUnit::writeFLUKA(OX);

  std::ostringstream cx;
  cx<<"USRICALL 5 "<<KFactor<<" "<<Radius<<" - - - "<<keyName;
  StrFunc::writeFLUKA(cx.str(),OX);

  return;
}
  

}  // NAMESPACE xraySystem
