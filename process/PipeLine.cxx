/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   process/PipeLine.cxx
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
#include "Quaternion.h"
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
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
#include "Simulation.h"
#include "ModelSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "pipeUnit.h"
#include "PipeLine.h"

#include "debugMethod.h"

namespace ModelSupport
{

PipeLine::PipeLine(const std::string& Key)  :
  keyName(Key),nCylinder(0),nAngle(6)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

PipeLine::PipeLine(const PipeLine& A) : 
  keyName(A.keyName),nCylinder(A.nCylinder),CV(A.CV),
  Pts(A.Pts),layerSurf(A.layerSurf),commonSurf(A.commonSurf),
  activeFlags(A.activeFlags)
  /*!
    Copy constructor
    \param A :: PipeLine to copy
  */
{}

PipeLine&
PipeLine::operator=(const PipeLine& A)
  /*!
    Assignment operator
    \param A :: PipeLine to copy
    \return *this
  */
{
  if (this!=&A)
    {
      nCylinder=A.nCylinder;
      CV=A.CV;
      Pts=A.Pts;
      layerSurf=A.layerSurf;
      commonSurf=A.commonSurf;
      activeFlags=A.activeFlags;
      clearPUnits();
    }
  return *this;
}

PipeLine::~PipeLine() 
 /*!
   Destructor
 */
{
  clearPUnits();
}

void
PipeLine::clearPUnits()
  /*!
    Tidy up the PUnit vector
   */
{
  std::vector<pipeUnit*>::iterator vc;
  for(vc=PUnits.begin();vc!=PUnits.end();vc++)
    delete *vc;
  PUnits.clear();
  return;
}

void 
PipeLine::setPoints(const std::vector<Geometry::Vec3D>& V)
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
PipeLine::addPoint(const Geometry::Vec3D& Pt)
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
PipeLine::addSurfPoint(const Geometry::Vec3D& Pt,const std::string& surfStr)
  /*!
    Add an additional point
    \param Pt :: Point to add
    \param surfStr :: Outgoing surface string
   */
{ 
  ELog::RegMethod RegA("PipeLine","addSurfPoint");
  addSurfPoint(Pt,surfStr,std::string());
  return;
}

void 
PipeLine::addSurfPoint(const Geometry::Vec3D& Pt,
		       const std::string& surfStr,
		       const std::string& commonStr)
  /*!
    Add an additional point
    \param Pt :: Point to add
    \param surfStr :: Outgoing surface string
    \param commonSurf :: common surface
   */
{ 
  ELog::RegMethod RegA("PipeLine","addSurfPoint");

  HeadRule LSurf,CSurf;

  if (LSurf.procString(surfStr)!=1)
    throw ColErr::InvalidLine("surfStr",surfStr,0);

  layerSurf.insert(std::map<size_t,HeadRule>::value_type(Pts.size(),LSurf));
  if (!commonStr.empty())
    {
      if (CSurf.procString(commonStr)!=1)
	throw ColErr::InvalidLine("commonStr",commonStr,0);
    }
  // insert empty rule if needed
  commonSurf.insert(std::map<size_t,HeadRule>::value_type(Pts.size(),CSurf));

  Pts.push_back(Pt);
  if (Pts.size()>1)
    activeFlags.push_back(0);

  return;
}

void
PipeLine::setActive(const size_t uIndex,const size_t flag)
  /*!
    Set the activity flag: 
    Goes from inner to outer 
    \param uIndex :: point layer (between uIndex and uIndex+1)
    \param flag :: 0 -- all, bits in order from inner point to outer
   */
{
  ELog::RegMethod RegA("PipeLine","setActive");

  if (activeFlags.size()<=uIndex)
    throw ColErr::IndexError<size_t>(uIndex,activeFlags.size(),"uIndex");
  activeFlags[uIndex]=flag;
  return;
}


void 
PipeLine::addRadius(const double R,const int M,const double T)
  /*!
    Add an additional radius
    \param R :: Radius
    \param M :: Material number
    \param T :: Temperature [kelvin]
   */
{
  ELog::RegMethod RegA("PipeLine","addRadius");
  nCylinder++;
  if (!CV.empty() && CV.back().CRadius>R)
    ELog::EM<<"Radius for pipes must in increasing order"<<ELog::endErr;
   
  CV.push_back(cylValues(R,M,T));

  return;
}
 
int
PipeLine::createUnits(Simulation& System)
  /*!
    Create the individual pipeUnits
    \param System :: Simulation to add pipes to
    \return 0 on success / -ve on error
   */
{
  ELog::RegMethod RegA("PipeLine","createUnits");

  if (Pts.size()<2)
    {
      ELog::EM<<"No points to create pipeLine"<<ELog::endCrit;
      return -1;
    }
  
  // Set the points
  HeadRule PtRule;
  for(size_t i=1;i<Pts.size();i++)
    {
      pipeUnit* PU=new pipeUnit(keyName,i);
      PU->setPoints(Pts[i-1],Pts[i]);
      if (layerSurf.find(i-1)!=layerSurf.end())
	{
	  PtRule=layerSurf[i-1];
      	  PtRule.addIntersection(commonSurf[i-1]);
	  PU->setASurf(PtRule);
	}
      // Complementary object only for modified surface
      if (layerSurf.find(i)!=layerSurf.end())
	{
	  PtRule=layerSurf[i];
	  PtRule.makeComplement();
	  PtRule.addIntersection(commonSurf[i]);
	  PU->setBSurf(PtRule);
	}
      PUnits.push_back(PU);
    } 
  for(size_t i=0;i<PUnits.size();i++)
    {
      if (i>0)
	PUnits[i]->connectFrom(PUnits[i-1]);
      if (i<PUnits.size()-1)
	PUnits[i]->connectTo(PUnits[i+1]);
    }
  // Actually build the units
  
  for(size_t i=0;i<PUnits.size();i++)
    {
      PUnits[i]->setNAngle(nAngle);
      PUnits[i]->createAll(System,activeFlags[i],CV);
    }
  return 0;
}

void
PipeLine::setNAngle(const size_t NA)
  /*!
    Set the angles in the pipeline
    \parma NA :: Number of angle segments
   */
{
  nAngle=NA;
  return;
}

void
PipeLine::createAll(Simulation& System)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
  */
{
  ELog::RegMethod RegA("PipeLine","createAll");
  System.populateCells();
  System.validateObjSurfMap();  
  createUnits(System);

  return;
}

  
}  // NAMESPACE moderatorSystem
