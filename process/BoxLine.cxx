/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/BoxLine.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <numeric>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <boost/multi_array.hpp>

#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Triple.h"
#include "NRange.h"
#include "NList.h"
#include "Tally.h"
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
#include "surfDivide.h"
#include "surfDIter.h"
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
#include "KGroup.h"
#include "Source.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "chipDataStore.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "LinearComp.h"
#include "ContainedComp.h"
#include "boxValues.h"
#include "boxUnit.h"
#include "BoxLine.h"

namespace ModelSupport
{

BoxLine::BoxLine(const std::string& Key)  :
  keyName(Key)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

BoxLine::BoxLine(const BoxLine& A) : 
  keyName(A.keyName),ZAxis(A.ZAxis),
  CV(A.CV),Pts(A.Pts),activeFlags(A.activeFlags),
  InitSurf(A.InitSurf)
  /*!
    Copy constructor
    \param A :: BoxLine to copy
  */
{
  copyPUnits(A);
}

BoxLine&
BoxLine::operator=(const BoxLine& A)
  /*!
    Assignment operator
    \param A :: BoxLine to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ZAxis=A.ZAxis;
      CV=A.CV;
      Pts=A.Pts;
      activeFlags=A.activeFlags;
      copyPUnits(A);
      InitSurf=A.InitSurf;
    }
  return *this;
}


BoxLine::~BoxLine() 
 /*!
   Destructor
 */
{
  clearPUnits();
}

void
BoxLine::clearPUnits()
  /*!
    Tidy up the PUnit vector
   */
{
  std::vector<boxUnit*>::iterator vc;
  for(vc=PUnits.begin();vc!=PUnits.end();vc++)
    delete *vc;
  PUnits.clear();
  return;
}

void
BoxLine::copyPUnits(const BoxLine& A)
  /*!
    Copy the PUnit section
    \param A :: BoxLine item to copy
   */
{
  ELog::RegMethod RegA("BoxLine","copyPUnits");
  if (this!=&A)
    {
      clearPUnits();
      // First copy the stuff
      std::vector<boxUnit*>::const_iterator vc;
      for(vc=A.PUnits.begin();vc!=A.PUnits.end();vc++)
	PUnits.push_back(new boxUnit(**vc));
      // Now set the links
      for(size_t i=0;i<PUnits.size();i++)
	{
	  if (i>0)
	    PUnits[i]->connectFrom(PUnits[i-1]);
	  if ( (i+1) <PUnits.size() )
	    PUnits[i]->connectTo(PUnits[i+1]);
	}
    }
  return;
}

void 
BoxLine::setPoints(const std::vector<Geometry::Vec3D>& V)
  /*!
    Set the sequence of points
    \param V :: Vector of points to used
   */
{
  clearPUnits();
  Pts=V;
  activeFlags.resize(Pts.size()-1);
  fill(activeFlags.begin(),activeFlags.end(),0);
  return;
}

void
BoxLine::setInitZAxis(const Geometry::Vec3D& ZA)
  /*!
    Defines the initial Z Axis for the box
    \param ZA :: Z-axis
  */
{
  ELog::RegMethod RegA("BoxLine","setInitZAxis");
  ZAxis=ZA.unit();
  return;
}

void 
BoxLine::addPoint(const Geometry::Vec3D& Pt)
  /*!
    Add an additional point
    \param Pt :: Point to add
   */
{
  Pts.push_back(Pt);
  if (Pts.size()>1)
    activeFlags.push_back(0);
  return;
}

void
BoxLine::setActive(const size_t uIndex,const size_t flag)
  /*!
    Set the activity flag: 
    Goes from inner to outer 
    \param uIndex :: section number
    \param flag :: status of section
  */
{
  ELog::RegMethod RegA("BoxLine","setActive");

  if (uIndex>=activeFlags.size())
    throw ColErr::IndexError<size_t>(uIndex,activeFlags.size(),
				     "uIndex/activeFlags.size()");
  activeFlags[uIndex]=flag;
  return;
}


void 
BoxLine::addSection(const double XS,const double ZS,
		    const int M,const double T)
  /*!
    Add an additional box unit. Currently only 
    sections 
    \param XS :: XSize
    \param YS :: YSize
    \param M :: Material number
    \param T :: Temperature [kelvin]
   */
{
  ELog::RegMethod RegA("BoxLine","addSection");

  CV.push_back(boxValues(4,M,T));
  CV.back().symPlanes(XS,ZS);
  return;
}

int
BoxLine::createUnits(Simulation& System)
  /*!
    Create the individual pipeUnits
    \param System :: Simulation to add vessel to
    \return 0 on success / -ve on error
   */
{
  ELog::RegMethod RegA("BoxLine","createUnits");

  if (Pts.size()<2)
    {
      ELog::EM<<"No points to create boxLine"<<ELog::endCrit;
      return -1;
    }

  clearPUnits();
  // Set the points
  for(size_t i=1;i<Pts.size();i++)
    {
      boxUnit* PU=new boxUnit(keyName+"Unit",i);
      PU->setPoints(Pts[i-1],Pts[i]);
      PUnits.push_back(PU);
    }

  // Set X-Y Axis:
  PUnits.front()->setZUnit(ZAxis);

  for(size_t i=0;i<PUnits.size();i++)
    {
      if (i>0)
	PUnits[i]->connectFrom(PUnits[i-1]);
      if ( (i+1) < PUnits.size() )
	PUnits[i]->connectTo(PUnits[i+1]);
    }
  
  if (!InitSurf.empty())
    PUnits.front()->setInitSurf(InitSurf);

  for(size_t i=0;i<PUnits.size();i++)
    PUnits[i]->createAll(System,activeFlags[i],CV);

  return 0;
}


void
BoxLine::setInitSurfaces(const std::string& Rules)
  /*!
    Initialize the inital surfaces
    \param Rules :: Rule for surfaces
  */
{
  InitSurf=Rules;
  return;
}

void
BoxLine::createAll(Simulation& System)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
  */
{
  ELog::RegMethod RegA("BoxLine","createAll");

  System.populateCells();
  System.validateObjSurfMap();
  createUnits(System);
  return;
}
  
}  // NAMESPACE moderatorSystem
