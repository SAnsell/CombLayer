/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Upgrade/InnerLayer.cxx
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
#include "Quaternion.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "Quadratic.h"
#include "Plane.h"
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
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ModBase.h"
#include "LayerInfo.h"
#include "CH4Layer.h"
#include "InnerLayer.h"

namespace ts1System 
{

InnerLayer::InnerLayer(const std::string& Key,const std::string& LKey) :
  ts1System::CH4Layer(LKey),IKeyName(Key),
  innerIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(innerIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
    \param LKey :: Layer keyName for variables
    
    Note need to register both this name and the layer name
  */
{}

InnerLayer::InnerLayer(const InnerLayer& A) : 
  ts1System::CH4Layer(A),
  IKeyName(A.IKeyName),innerIndex(A.innerIndex),
  cellIndex(A.cellIndex),nPoison(A.nPoison),poisonYStep(A.poisonYStep),
  poisonThick(A.poisonThick),poisonMat(A.poisonMat),
  pCladThick(A.pCladThick),pCladMat(A.pCladMat)
  /*!
    Copy constructor
    \param A :: InnerLayer to copy
  */
{}

InnerLayer&
InnerLayer::operator=(const InnerLayer& A)
  /*!
    Assignment operator
    \param A :: InnerLayer to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ts1System::CH4Layer::operator=(A);
      cellIndex=A.cellIndex;
      nPoison=A.nPoison;
      poisonYStep=A.poisonYStep;
      poisonThick=A.poisonThick;
      poisonMat=A.poisonMat;
      pCladThick=A.pCladThick;
      pCladMat=A.pCladMat;
    }
  return *this;
}

InnerLayer*
InnerLayer::clone() const
 /*!
   Clone constructor
   \return new(this)
 */
{
  return new InnerLayer(*this);
}



InnerLayer::~InnerLayer() 
 /*!
   Destructor
 */
{}

void
InnerLayer::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase 
 */
{
  ELog::RegMethod RegA("InnerLayer","populate");
  
  CH4Layer::populate(Control);

  double value;
  nPoison=Control.EvalVar<size_t>(IKeyName+"NPoison");

  if (nPoison)
    {
      for(size_t i=0;i<nPoison;i++)
	{
	  value=Control.EvalVar<double>
	    (IKeyName+StrFunc::makeString("PYStep",i+1));
	  poisonYStep.push_back(value);
	}
      poisonThick=Control.EvalVar<double>(IKeyName+"PGdThick");
      pCladThick=Control.EvalVar<double>(IKeyName+"PCladThick");

      // Materials
      poisonMat=ModelSupport::EvalMat<int>(Control,IKeyName+"PoisonMat");
      pCladMat=ModelSupport::EvalMat<int>(Control,IKeyName+"PCladMat");
    }

  return;
}
  
void
InnerLayer::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("InnerLayer","createSurface");
  
   int ch4Layer(innerIndex+500);
   for(size_t i=0;i<nPoison;i++)
    {
      ModelSupport::buildPlane(SMap,ch4Layer+1,
			       Origin+Y*(poisonYStep[i]-poisonThick/2.0),Y);
      ModelSupport::buildPlane(SMap,ch4Layer+2,
			       Origin+Y*(poisonYStep[i]+poisonThick/2.0),Y);
      ModelSupport::buildPlane(SMap,ch4Layer+11,
          Origin+Y*(poisonYStep[i]-pCladThick-poisonThick/2.0),Y);
      ModelSupport::buildPlane(SMap,ch4Layer+12,
          Origin+Y*(poisonYStep[i]+pCladThick+poisonThick/2.0),Y);
      ch4Layer+=20;
    }

  CH4Layer::createSurfaces();
  return;
}


void
InnerLayer::createObjects(Simulation& System)
  /*!
    Creates the inner object
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("InnerLayer","createObjects");

  std::string Out;

  // Poison layers [negative side first]:
  const std::string Edge=
    ModelSupport::getComposite(SMap,modIndex," 3 -4 5 -6 ");
  const double ch4Temp=LVec[0].getTemp();
  const int ch4Mat=LVec[0].getMat();
  int nextPoisLayer(innerIndex+500);

  // front / back:
  HeadRule frontX,backX;
  createFrontRule(LVec[0],modIndex,0,frontX);
  createBackRule(LVec[0],modIndex,0,backX);
  for(size_t i=0;i<nPoison;i++)
    {
      if (i)
	Out=ModelSupport::getComposite(SMap,nextPoisLayer-10," 2 -21 ");
      else
	{
	  Out=ModelSupport::getComposite(SMap,nextPoisLayer," -11 ")
	    +frontX.display();
	}
      System.addCell(MonteCarlo::Qhull(cellIndex++,ch4Mat,
				       ch4Temp,Out+Edge));
      innerCells.push_back(cellIndex-1);
      
      // Al 
      Out=ModelSupport::getComposite(SMap,nextPoisLayer," 11 -1 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,pCladMat,
				       ch4Temp,Out+Edge));
      innerCells.push_back(cellIndex-1);
      
      // Poisoning 
      Out=ModelSupport::getComposite(SMap,nextPoisLayer," 1 -2");
      System.addCell(MonteCarlo::Qhull(cellIndex++,poisonMat,
				       ch4Temp,Out+Edge));
      innerCells.push_back(cellIndex-1);
      // Al 
      Out=ModelSupport::getComposite(SMap,nextPoisLayer," 2 -12 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,pCladMat,
				       ch4Temp,Out+Edge));
      innerCells.push_back(cellIndex-1);
      nextPoisLayer+=20;
    }
  // Final (or total segment)
  //  Out=ModelSupport::getComposite(SMap,prevPLayer,innerIndex," 2 -2M ");
  if (nPoison)
    Out=ModelSupport::getComposite
      (SMap,nextPoisLayer-10," 2 ")+backX.display();
  else
    Out=backX.display()+" "+frontX.display();
  
  System.addCell(MonteCarlo::Qhull(cellIndex++,ch4Mat,
				   ch4Temp,Out+Edge));
  innerCells.push_back(cellIndex-1);
  
  CH4Layer::createObjects(System);
  return;
}

Geometry::Vec3D
InnerLayer::getSurfacePoint(const size_t layerIndex,
			    const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \return Surface point
  */
{
  ELog::RegMethod RegA("InnerLayer","getSurfacePoint");

  return CH4Layer::getSurfacePoint(layerIndex,sideIndex);
}

std::string
InnerLayer::getLayerString(const size_t layerIndex,
			   const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-LVec]
    \return Surface string
  */
{
  ELog::RegMethod RegA("InnerLayer","getLayerString");

  return CH4Layer::getLayerString(layerIndex,sideIndex);

}

int
InnerLayer::getLayerSurf(const size_t layerIndex,
			 const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface size
  */
{
  ELog::RegMethod RegA("InnerLayer","getLayerSurf");

  return CH4Layer::getLayerSurf(layerIndex,sideIndex);
}


void
InnerLayer::createAll(Simulation& System,
		      const attachSystem::FixedComp& axisFC,
		      const attachSystem::FixedComp* orgFC,
		      const long int sideIndex)
  /*!
    Extrenal build everything include divided inner
    \param System :: Simulation
    \param axisFC :: FixedComp to get axis [origin if orgFC == 0]
    \param orgFC :: Extra origin point if required
    \param sideIndex :: link point for origin if given
   */
{
  ELog::RegMethod RegA("InnerLayer","createAll");

  populate(System.getDataBase());
  ModBase::createUnitVector(axisFC,orgFC,sideIndex);
  
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  return;
}

  
}  // NAMESPACE ts1System
