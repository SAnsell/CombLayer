/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   t1Upgrade/SplitInner.cxx
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; plwithout even the implied warranty of
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
#include "SplitInner.h"

namespace ts1System
{

SplitInner::SplitInner(const std::string& Key,const std::string& LKey) :
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

SplitInner::SplitInner(const SplitInner& A) : 
  ts1System::CH4Layer(A),
  IKeyName(A.IKeyName),innerIndex(A.innerIndex),
  cellIndex(A.cellIndex),innerNLayer(A.innerNLayer),
  thick(A.thick),mat(A.mat),temp(A.temp)
  /*!
    Copy constructor
    \param A :: SplitInner to copy
  */
{}

SplitInner&
SplitInner::operator=(const SplitInner& A)
  /*!
    Assignment operator
    \param A :: SplitInner to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ts1System::CH4Layer::operator=(A);
      cellIndex=A.cellIndex;
      innerNLayer=A.innerNLayer;
      thick=A.thick;
      mat=A.mat;
      temp=A.temp;
    }
  return *this;
}

SplitInner*
SplitInner::clone() const
 /*!
   Clone constructor
   \return new(this)
 */
{
  return new SplitInner(*this);
}

SplitInner::~SplitInner() 
 /*!
   Destructor
 */
{}

void
SplitInner::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: DataBase 
 */
{
  ELog::RegMethod RegA("SplitInner","populate");
  
  CH4Layer::populate(Control);

  innerNLayer=Control.EvalVar<size_t>(IKeyName+"NLayers");

  double T,tval;
  int matN;
  const double TInner = LVec[0].front()+LVec[0].back();
  double TFront = -LVec[0].front();
  for(size_t i=0;i<innerNLayer;i++)
    {
      if (i+1==innerNLayer)
	{
	  T=TInner-TFront;
	  if (T<0)
	    ELog::EM<<"Sum of thickness too large:"
		    <<TFront+LVec[0].front()<<" "
		    <<TInner<<ELog::endErr;
	}
      else 
	{
	  T=Control.EvalVar<double>
	    (IKeyName+StrFunc::makeString("Thick",i+1));
	}
      
      tval=Control.EvalVar<double>
	(IKeyName+StrFunc::makeString("Temp",i+1));
      matN=ModelSupport::EvalMat<int>(Control,
	    IKeyName+StrFunc::makeString("Mat",i+1));
      TFront+=T;
      thick.push_back(TFront);
      temp.push_back(tval);
      mat.push_back(matN);
    }
  return;
}
  
void
SplitInner::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("SplitInner","createSurface");
  
  int iLayer(innerIndex);

  for(size_t i=0;i<innerNLayer;i++)
    {
      ModelSupport::buildPlane(SMap,iLayer+1,Origin+Y*thick[i],Y);
      iLayer+=10;
    }

  CH4Layer::createSurfaces();
  return;
}


void
SplitInner::createObjects(Simulation& System)
  /*!
    Creates the inner objects
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("SplitInner","createObjects");

  int iLayer(innerIndex);

  std::string Edge=
    ModelSupport::getComposite(SMap,modIndex," 3 -4 5 -6 ");
  // front / back:
  HeadRule frontX,backX;
  createFrontRule(LVec[0],modIndex,0,frontX);
  createBackRule(LVec[0],modIndex,0,backX);
  std::string prevLayer=frontX.display();
  std::string Out;
  for(size_t i=0;i<innerNLayer;i++)
    {
      if (i+1!=innerNLayer)
	Out=ModelSupport::getComposite(SMap,iLayer," -1 ");
      else
      	Out=backX.display();

      Out+=Edge+prevLayer;
      System.addCell(MonteCarlo::Qhull(cellIndex++,mat[i],temp[i],Out));
      prevLayer=ModelSupport::getComposite(SMap,iLayer," 1 ");
      iLayer+=10;
    }
  
  CH4Layer::createObjects(System);
  return;
}

Geometry::Vec3D
SplitInner::getSurfacePoint(const size_t layerIndex,
			    const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \return Surface point
  */
{
  ELog::RegMethod RegA("SplitInner","getSurfacePoint");

  return CH4Layer::getSurfacePoint(layerIndex,sideIndex);
}

std::string
SplitInner::getLayerString(const size_t layerIndex,
			   const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-LVec]
    \return Surface string
  */
{
  ELog::RegMethod RegA("SplitInner","getLayerString");

  return CH4Layer::getLayerString(layerIndex,sideIndex);
}

int
SplitInner::getLayerSurf(const size_t layerIndex,
			 const long int sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface size
  */
{
  ELog::RegMethod RegA("SplitInner","getLayerSurf");

  return CH4Layer::getLayerSurf(layerIndex,sideIndex);
}


void
SplitInner::createAll(Simulation& System,
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
  ELog::RegMethod RegA("SplitInner","createAll");

  populate(System.getDataBase());
  ModBase::createUnitVector(axisFC,orgFC,sideIndex);

  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  return;
}
  
}  // NAMESPACE ts1System
