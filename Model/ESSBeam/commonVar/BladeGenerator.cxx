/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVar/BladeGenerator.cxx
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
#include <stack>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <numeric>
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
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "BladeGenerator.h"

namespace setVariable
{

BladeGenerator::BladeGenerator() :
  gap(1.0),innerMat("Inconnel"),outerMat("B4C")
  /*!
    Constructor and defaults
  */
{}

BladeGenerator::BladeGenerator(const BladeGenerator& A) : 
  gap(A.gap),thick(A.thick),innerThick(A.innerThick),
  innerMat(A.innerMat),outerMat(A.outerMat),
  CentreAngle(A.CentreAngle),OpenAngle(A.OpenAngle)
  /*!
    Copy constructor
    \param A :: BladeGenerator to copy
  */
{}

BladeGenerator&
BladeGenerator::operator=(const BladeGenerator& A)
  /*!
    Assignment operator
    \param A :: BladeGenerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      gap=A.gap;
      thick=A.thick;
      innerThick=A.innerThick;
      innerMat=A.innerMat;
      outerMat=A.outerMat;
      CentreAngle=A.CentreAngle;
      OpenAngle=A.OpenAngle;
    }
  return *this;
}


  
BladeGenerator::~BladeGenerator() 
 /*!
   Destructor
 */
{}

void
BladeGenerator::setMaterials(const std::string& IM,
			     const std::string& OM)
  /*!
    Set the materials
    \param IM :: Inner material
    \param OM :: Outer material
  */
{
  innerMat=IM;
  outerMat=OM;
  return;
}


void
BladeGenerator::setThick(const std::vector<double>& T)
  /*!
    Set thicknesses
    \param T :: Thickness vector [no checking]
   */
{
  thick=T;
  innerThick.clear();
  resetPhase();
  return;
}

void
BladeGenerator::setInnerThick(const std::vector<double>& T)
  /*!
    Set thicknesses
    \param T :: Thickness vector [no checking]
   */
{
  ELog::RegMethod RegA("BladeGenerator","setInnerThick");
  
  innerThick=T;
  for(size_t i=0;i<thick.size() && i<innerThick.size();i++)
    if (thick[i]-innerThick[i]>Geometry::zeroTol)
      ELog::EM<<"Error with Thickness/InnerThickness["<<i
	      <<"] = "<<thick[i]<<" "<<innerThick[i]<<ELog::endErr;
  return;
}
  
void
BladeGenerator::resetPhase()
  /*!
    Reset all the phase
  */
{
  CentreAngle.clear();
  OpenAngle.clear();
  return;
}
  
void
BladeGenerator::setPhase(const size_t index,
			 const std::vector<double>& CAngle,
			 const std::vector<double>& OAngle)
  /*!
    Add/Set a Phase;
    \param index :: index of blade
    \param CAngle :: List of angles
    \param OAngle :: Open angle of gap
   */
{
  ELog::RegMethod RegA("BladeGenerator","setPhase");
  if (CAngle.size()!=OAngle.size() ||
      OAngle.empty())
    throw ColErr::MisMatch<size_t>(OAngle.size(),CAngle.size(),
				   "Phase/Centres must be equal");
  if (index>CentreAngle.size())
    throw ColErr::IndexError<size_t>
      (index,CentreAngle.size(),"index/CentreAngle");

  if (index==CentreAngle.size())
    {
      CentreAngle.push_back(CAngle);
      OpenAngle.push_back(OAngle);
    }
  else
    {
      CentreAngle[index]=CAngle;
      OpenAngle[index]=OAngle;
    }
  return;
}

void
BladeGenerator::addPhase(const std::vector<double>& CAngle,
			 const std::vector<double>& OAngle)
  /*!
    Simple phase adder
    \param CAngle :: List of angles
    \param OAngle :: Open angle of gap
   */
{
  ELog::RegMethod Rega("BladeGenerator","addPhase");
  
  if (CAngle.size()!=OAngle.size() ||
      OAngle.empty())
    throw ColErr::MisMatch<size_t>(OAngle.size(),CAngle.size(),
				   "Phase/Centres must be equal");

  CentreAngle.push_back(CAngle);
  OpenAngle.push_back(OAngle);
  return;
}
  
void
BladeGenerator::generateBlades(FuncDataBase& Control,
                               const std::string& keyName,
			       const double yStep,
                               const double innerRadius,
			       const double outerRadius) const

  /*!
    Primary function for setting the variables
    \param Control :: Database to add variables 
    \param keyName :: head name for variable
    \param yStep :: y-offset 
    \param innerRadius :: Inner radius
    \param outerRadius :: Outer radius
  */
{
  ELog::RegMethod RegA("BladeGenerator","generatorBlade");

  double realYStep=yStep;
  if (std::abs(yStep)<Geometry::zeroTol && !thick.empty())
    {
      realYStep= -std::accumulate(thick.begin(),thick.end(),0.0);
      realYStep+=thick.front();  // front allowed for
      realYStep-=(static_cast<double>(thick.size())-1.0)*gap;
      realYStep/=2.0;
    }
  
  // Double Blade chopper
  Control.addVariable(keyName+"XStep",0.0);
  Control.addVariable(keyName+"YStep",realYStep);
  Control.addVariable(keyName+"ZStep",0.0);
  Control.addVariable(keyName+"XYangle",0.0);
  Control.addVariable(keyName+"Zangle",0.0);

  Control.addVariable(keyName+"Gap",gap);
  Control.addVariable(keyName+"InnerRadius",innerRadius);
  Control.addVariable(keyName+"OuterRadius",outerRadius);
  Control.addVariable(keyName+"NDisk",thick.size());

  for(size_t i=0;i<innerThick.size();i++)
    Control.addVariable(keyName+StrFunc::makeString(i)+"InnerThick",
                        innerThick[i]);
  
  for(size_t i=0;i<thick.size();i++)
    Control.addVariable(keyName+StrFunc::makeString(i)+"Thick",thick[i]);

  Control.addVariable(keyName+"InnerMat",innerMat);
  Control.addVariable(keyName+"OuterMat",outerMat);
  
  for(size_t index=0;index<CentreAngle.size();index++)
    {
      
      const std::string IndexStr(StrFunc::makeString(index));
      const std::vector<double>& CRef=CentreAngle[index];
      const std::vector<double>& ORef=OpenAngle[index];
      Control.addVariable(keyName+IndexStr+"NBlades",CRef.size());
      for(size_t j=0;j<CRef.size();j++)
	{
	  const std::string jStr(StrFunc::makeString(j));
	  Control.addVariable(keyName+IndexStr+"PhaseAngle"+jStr,CRef[j]);
	  Control.addVariable(keyName+IndexStr+"OpenAngle"+jStr,ORef[j]);
	}
    }

  // Checks:
  if (innerRadius>outerRadius-Geometry::zeroTol)
    ELog::EM<<"Failure outerRadius["<<outerRadius
            <<"] < innerRadius["<<innerRadius<<"]"<<ELog::endErr;
  return;

}

}  // NAMESPACE setVariable
