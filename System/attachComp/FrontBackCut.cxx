/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attach/FrontBackCut.cxx
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
#include <array>

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
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
#include "Cylinder.h"
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
#include "generateSurf.h"
#include "LinkUnit.h"  
#include "FixedComp.h"
#include "FrontBackCut.h"

namespace attachSystem
{

FrontBackCut::FrontBackCut() :
  activeFront(0),activeBack(0)
  /*!
    Constructor [default]
  */
{}

FrontBackCut::FrontBackCut(const FrontBackCut& A) : 
  activeFront(A.activeFront),frontCut(A.frontCut),
  frontDivider(A.frontDivider),activeBack(A.activeBack),
  backCut(A.backCut),backDivider(A.backDivider)
  /*!
    Copy constructor
    \param A :: FrontBackCut to copy
  */
{}

FrontBackCut&
FrontBackCut::operator=(const FrontBackCut& A)
  /*!
    Assignment operator
    \param A :: FrontBackCut to copy
    \return *this
  */
{
  if (this!=&A)
    {
      activeFront=A.activeFront;
      frontCut=A.frontCut;
      frontDivider=A.frontDivider;
      activeBack=A.activeBack;
      backCut=A.backCut;
      backDivider=A.backDivider;
    }
  return *this;
}
  

FrontBackCut::~FrontBackCut()
  /*!
    Destructor
  */
{}


void
FrontBackCut::setFront(const attachSystem::FixedComp& WFC,
                       const long int sideIndex)
  /*!
    Set a front wall
    \param WFC :: Front line
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("FrontBackCut","setFront");

  // FixedComp::setLinkSignedCopy(0,FC,sideIndex);
  frontCut=WFC.getSignedFullRule(sideIndex);
  frontDivider=WFC.getSignedCommonRule(sideIndex);
  frontCut.populateSurf();
  frontDivider.populateSurf();
  activeFront=1;
  return;
}

void
FrontBackCut::setBack(const attachSystem::FixedComp& WFC,
                      const long int sideIndex)
  /*!
    Set a back wall
    \param WFC :: Back component
    \param sideIndex :: link point
  */
{
  ELog::RegMethod RegA("FrontBackCut","setBack");

  // FixedComp::setLinkSignedCopy(0,FC,sideIndex);
  backCut=WFC.getSignedMainRule(sideIndex);
  backDivider=WFC.getSignedCommonRule(sideIndex);
  backCut.populateSurf();
  backDivider.populateSurf();
  activeBack=1;
  return;
}


void
FrontBackCut::setFrontDivider(const HeadRule& HR)
  /*!
    Set the divider
    \param HR :: Divider surface
   */
{
  frontDivider=HR;
  return;
}

void
FrontBackCut::setBackDivider(const HeadRule& HR)
  /*!
    Set the divider
    \param HR :: Divider surface
   */
{
  backDivider=HR;
  return;
}

std::string
FrontBackCut::frontRule() const
  /*!
    Accessor  to front rule
    \return frontRule with divider
  */
{
  return (activeFront) ?
    frontCut.display()+frontDivider.display() : "" ;    
}

std::string
FrontBackCut::backRule() const
  /*!
    Accessor to back rule
    \return backRule with divider
  */
{
  return (activeBack) ?
    backCut.display()+backDivider.display() : "";    
}

  
  
}  // NAMESPACE attachSystem
