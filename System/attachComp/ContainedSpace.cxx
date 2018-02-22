/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   attachComp/ContainedSpace.cxx
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#include <deque>
#include <string>
#include <algorithm>
#include <iterator>
#include <memory>
#include <functional>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "writeSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "SurInter.h"
#include "Rules.h"
#include "HeadRule.h"
#include "Object.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Qhull.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "Simulation.h"
#include "AttachSupport.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedSpace.h"


namespace attachSystem
{

ContainedSpace::ContainedSpace()  :
  ContainedComp(),nDirection(8),
  primaryCell(0),LCutters(2)
  /*!
    Constructor 
  */
{}

ContainedSpace::~ContainedSpace()
  /*!
    Deletion operator
  */
{}

void
ContainedSpace::setPrimaryCell(const int CN)
  /*!
    Set primary cell [is this enough?]
    \param CN :: Cell number
  */
{
  primaryCell=CN;
  return;
}

  
void
ContainedSpace::setConnect(const size_t Index,
			     const Geometry::Vec3D& C,
			     const Geometry::Vec3D& A)
 /*!
   Set the axis of the linked component
   \param Index :: Link number
   \param C :: Centre coordinate
   \param A :: Axis direciton
 */
{
  ELog::RegMethod RegA("ContainedComp","setConnect");
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),"LU.size/index");

  LCutters[Index].setConnectPt(C);
  LCutters[Index].setAxis(A);
  return;
}

void
ContainedSpace::setLinkSurf(const size_t Index,
			    const HeadRule& HR) 
  /*!
    Set a surface to output
    \param Index :: Link number
    \param HR :: HeadRule to add
  */
{
  ELog::RegMethod RegA("ContainedSpace","setLinkSurf(HR)");
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),"LU size/Index");

  LCutters[Index].setLinkSurf(HR);
  return;
}

void
ContainedSpace::setLinkSurf(const size_t Index,
			    const int SN) 
  /*!
    Set a surface to output
    \param Index :: Link number
    \param SN :: surf to add
  */
{
  ELog::RegMethod RegA("ContainedSpace","setLinkSurf(int)");
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),"LU size/Index");

  LCutters[Index].setLinkSurf(SN);
  return;
}

void
ContainedSpace::setLinkCopy(const size_t Index,
			    const FixedComp& FC,
			    const long int sideIndex)
  /*!
    Copy the opposite (as if joined) link surface 
    Note that the surfaces are complemented
    \param Index :: Link number
    \param FC :: Other Fixed component to copy object from
    \param sideIndex :: link unit of other object
  */
{
  ELog::RegMethod RegA("ContainedSpace","setUSLinkCopy");
  
  if (Index>=LCutters.size())
    throw ColErr::IndexError<size_t>(Index,LCutters.size(),
				     "LCutters size/index");
  LCutters[Index]=FC.getSignedLU(sideIndex);
  
  return;
}
  
void
ContainedSpace::calcBoundary(Simulation& System,const int cellN,
			    const size_t NDivide)
  /*!
    Boundary calculator
    \param System :: Simulation
    \param cellN :: Cell number
    \param NDivide :: Number to divide
  */
{
  ELog::RegMethod RegA("ContainedSpace","calcBoundary");
  
  const MonteCarlo::Qhull* outerObj=System.findQhull(cellN);
  if (!outerObj)
    throw ColErr::InContainerError<int>(cellN,"cellN on found");

  std::set<int> surfN;
  for(const LinkUnit& LU : LCutters)
    {

      const Geometry::Vec3D& OPt=LU.getConnectPt();
      const Geometry::Vec3D& YY=LU.getAxis();
      const Geometry::Vec3D XX=YY.crossNormal();
      const Geometry::Vec3D ZZ=YY*XX;


      if (!outerObj->isValid(OPt))
	{
	  ELog::EM<<"Object == "<<*outerObj<<ELog::endDiag;
	  ELog::EM<<"Point == "<<OPt<<ELog::endDiag;
	  throw ColErr::InContainerError<Geometry::Vec3D>
	    (OPt,"Point out of object");
	}
      const HeadRule& objHR=outerObj->getHeadRule();


      const double angleStep=2.0*M_PI/static_cast<double>(NDivide);
      double angle(0.0);
      for(size_t i=0;i<NDivide;i++)
	{
	  const Geometry::Vec3D Axis=XX*cos(angle)+ZZ*sin(angle);
	  double D;
	  // Outgoing sign
	  const int SN=objHR.trackSurf(OPt,Axis,D);
	  surfN.insert(SN);
	  angle+=angleStep;
	}
    }
  // Check for no negative repeats:
  BBox.reset();
  for(const int SN : surfN)
    {
      if (SN==0 || (SN<0 && surfN.find(-SN)!=surfN.end()))
	throw ColErr::InContainerError<int>(SN,"Surf repeated");
      
      BBox.addIntersection(-SN);
    }
  return;
}
  
void
ContainedSpace::insertObjects(Simulation& System)
  /*!
    Assumption that we can put this object into a space
    Makes the object bigger 
    Then constructs the outer boundary as if that object is 
    the outer boundary!
    \param System :: simulation system
  */
{
  ELog::RegMethod RegA("ContainedSpace","insertObject");

  if (primaryCell)
    {
      calcBoundary(System,primaryCell,nDirection);

      MonteCarlo::Qhull* outerObj=System.findQhull(primaryCell); 
      const int matN=outerObj->getMat();
      const double matTemp=outerObj->getTemp();
      outerObj->addSurfString(BBox.complement().display());
      // Now construct new cell

      
      // keep copy and reset
      HeadRule Out(outerSurf);
      outerSurf.reset();
      
      // cutters point outwards:
      for(const LinkUnit& LU : LCutters)
	outerSurf.addUnion(LU.getMainRule());

      Out.addIntersection(BBox);   // bbox inwards
      Out.addIntersection(outerSurf.complement());
      System.addCell(cellIndex++,matN,matTemp,Out.display()); 

      primaryCell=0;
    }
  ContainedComp::insertObjects(System);
  return;
}

}  // NAMESPACE attachSystem
