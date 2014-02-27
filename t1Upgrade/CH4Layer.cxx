/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   t1Upgrade/CH4Layer.cxx
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
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "LayerComp.h"
#include "t1Reflector.h"
#include "ModBase.h"
#include "LayerInfo.h"
#include "CH4Layer.h"

namespace ts1System
{

CH4Layer::CH4Layer(const std::string& Key)  :
  constructSystem::ModBase(Key,6),nPoison(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

CH4Layer::CH4Layer(const CH4Layer& A) : 
  ModBase(A),
  LVec(A.LVec),nPoison(A.nPoison),poisonYStep(A.poisonYStep),
  poisonThick(A.poisonThick),poisonMat(A.poisonMat),
  pCladThick(A.pCladThick),pCladMat(A.pCladMat)
  /*!
    Copy constructor
    \param A :: CH4Layer to copy
  */
{}

CH4Layer&
CH4Layer::operator=(const CH4Layer& A)
  /*!
    Assignment operator
    \param A :: CH4Layer to copy
    \return *this
  */
{
  if (this!=&A)
    {
      ModBase::operator=(A);
      LVec=A.LVec;
      nPoison=A.nPoison;
      poisonYStep=A.poisonYStep;
      poisonThick=A.poisonThick;
      poisonMat=A.poisonMat;
      pCladThick=A.pCladThick;
      pCladMat=A.pCladMat;
    }
  return *this;
}


CH4Layer*
CH4Layer::clone() const
 /*!
   Clone constructor
   \return new(this)
 */
{
  return new CH4Layer(*this);
}

CH4Layer::~CH4Layer() 
 /*!
   Destructor
 */
{}

double
CH4Layer::checkUnit(const FuncDataBase& Control,
		    const size_t Index,const std::string& A,
		    const double AScale,const std::string& B,
		    const double BScale,const std::string& C,
		    const double CScale) const
  /*!
    Checks a given value exists in the data base
    and returns the scaled component
    \param Control :: DataBase
    \param Index :: Index value
    \param A :: Primary string
    \param AScale :: Scale factor for [if found]
    \param B :: Primary string
    \param BScale :: Scale factor for [if found]
    \param C :: Primary string
    \param CScale :: Scale factor for [if found]
    \return Var(A/B)*Scale
   */
{
  ELog::RegMethod RegA("CH4Layer","checkUnit");

  const std::string keyA=keyName+StrFunc::makeString(A,Index);
  if (Control.hasVariable(keyA))
    return Control.EvalVar<double>(keyA)*AScale;
  const std::string keyB=keyName+StrFunc::makeString(B,Index);
  if (Control.hasVariable(keyB))
    return Control.EvalVar<double>(keyB)*BScale;
  const std::string keyC=keyName+StrFunc::makeString(C,Index);
  // Allow this to throw
  return Control.EvalVar<double>(keyC)*CScale;

}

void
CH4Layer::populate(const Simulation& System)
 /*!
   Populate all the variables
   \param System :: Simulation to use
 */
{
  ELog::RegMethod RegA("CH4Layer","populate");
  
  const FuncDataBase& Control=System.getDataBase();

  // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  const size_t nLayer=Control.EvalVar<size_t>(keyName+"NLayer");
  if (nLayer<1)
    throw ColErr::IndexError<size_t>(0,1,"nLayer");
  double Front,Back,Left,Right,Top,Base,temp;
  int mat;
  for(size_t i=0;i<nLayer;i++)
    {
      Top=checkUnit(Control,i+1,"Top",1.0,"Height",0.5,"Layer",1.0);
      Base=checkUnit(Control,i+1,"Base",1.0,"Height",0.5,"Layer",1.0);
      Left=checkUnit(Control,i+1,"Left",1.0,"Width",0.5,"Layer",1.0);
      Right=checkUnit(Control,i+1,"Right",1.0,"Width",0.5,"Layer",1.0);
      Front=checkUnit(Control,i+1,"Front",1.0,"Depth",0.5,"Layer",1.0);
      Back=checkUnit(Control,i+1,"Back",1.0,"Depth",0.5,"Layer",1.0);

      mat=ModelSupport::EvalMat<int>
	(Control,keyName+StrFunc::makeString("Mat",i+1));
      temp=Control.EvalDefVar<double>
	(keyName+StrFunc::makeString("Temp",i+1),0.0);
      
      LayerInfo IDX(Front,Back,Left,Right,Base,Top);
      IDX.setMat(mat,temp);
      if (i)
	IDX+=LVec.back();
      LVec.push_back(IDX);
    }

  double value;
  nPoison=Control.EvalVar<size_t>(keyName+"NPoison");

  if (nPoison)
    {
      for(size_t i=0;i<nPoison;i++)
	{
	  value=Control.EvalVar<double>
	    (keyName+StrFunc::makeString("PYStep",i+1));
	  poisonYStep.push_back(value);
	}
      poisonThick=Control.EvalVar<double>(keyName+"PGdThick");
      pCladThick=Control.EvalVar<double>(keyName+"PCladThick");

      // Materials
      poisonMat=ModelSupport::EvalMat<int>(Control,keyName+"PoisonMat");
      pCladMat=ModelSupport::EvalMat<int>(Control,keyName+"PCladMat");
    }

  return;
}
  
void
CH4Layer::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    - Y Down the beamline
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("CH4Layer","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);

  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,0);
  return;
}

void
CH4Layer::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("CH4Layer","createSurface");

  const Geometry::Vec3D XYZ[6]={-Y,Y,-X,X,-Z,Z};
  int ch4Layer(modIndex+1);

  for(size_t i=0;i<LVec.size();i++)
    {
      // 1+j+j%2 gives 1,1,3,3,5,5 as j iterates
      for(size_t j=0;j<6;j++)
	ModelSupport::buildPlane(SMap,ch4Layer+static_cast<int>(j),
				 Origin+XYZ[j]*LVec[i].Item(j),XYZ[1+j-j%2]);

      ch4Layer+=10;
    }

  for(size_t j=0;j<6;j++)
    {
      FixedComp::setConnect(j,Origin+XYZ[j]*LVec.back().Item(j),
			    XYZ[j]);
    }
  ch4Layer=modIndex+500;
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

  return;
}

void
CH4Layer::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("CH4Layer","createObjects");

  std::string Out;

  int ch4Layer(modIndex);
  // Poison layers [negative side first]:
  const std::string 
    Edge=ModelSupport::getComposite(SMap,ch4Layer," 3 -4 5 -6 ");
  const double ch4Temp=LVec[0].getTemp();
  const int ch4Mat=LVec[0].getMat();
  int prevPLayer(modIndex-1);        // deal with the 2M effect
  int nextPLayer(modIndex+500);

  for(size_t i=0;i<nPoison;i++)
    {
      Out=ModelSupport::getComposite
	(SMap,prevPLayer,nextPLayer," 2 -11M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,ch4Mat,
				       ch4Temp,Out+Edge));

      // Al 
      Out=ModelSupport::getComposite(SMap,nextPLayer," 11 -1 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,pCladMat,
				       ch4Temp,Out+Edge));

      // Poisoning 
      Out=ModelSupport::getComposite(SMap,nextPLayer," 1 -2");
      System.addCell(MonteCarlo::Qhull(cellIndex++,poisonMat,
				       ch4Temp,Out+Edge));
      // Al 
      Out=ModelSupport::getComposite(SMap,nextPLayer," 2 -12 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,pCladMat,
				       ch4Temp,Out+Edge));
      prevPLayer=nextPLayer+10;   // outer layer is +10
      nextPLayer+=20;
    }
  // Final (or total segment)
  Out=ModelSupport::getComposite(SMap,prevPLayer,modIndex," 2 -2M ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,ch4Mat,
				   ch4Temp,Out+Edge));

  std::string Exclude=
    ModelSupport::getComposite(SMap,modIndex," (-1:2:-3:4:-5:6) ");
  // Outer layers:
  ch4Layer+=10;
  for(size_t i=1;i<LVec.size();i++)
    {
      Out=ModelSupport::getComposite(SMap,ch4Layer," 1 -2 3 -4 5 -6 ");
      System.addCell(MonteCarlo::Qhull
		     (cellIndex++,LVec[i].getMat(),
		      LVec[i].getTemp(),Out+Exclude));
      Exclude=ModelSupport::getComposite(SMap,ch4Layer," (-1:2:-3:4:-5:6) ");
      ch4Layer+=10;
    }
  
  addOuterSurf(Out);
  addBoundarySurf(Out);

  return;
}

Geometry::Vec3D
CH4Layer::getSurfacePoint(const size_t layerIndex,
			  const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link point
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \return Surface point
  */
{
  ELog::RegMethod RegA("CylModerator","getSurfacePoint");

  if (sideIndex>5) 
    throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");
  if (layerIndex>=LVec.size()) 
    throw ColErr::IndexError<size_t>(layerIndex,LVec.size(),"layerIndex");

  const Geometry::Vec3D XYZ[6]={-Y,Y,-X,X,-Z,Z};
  return Origin+XYZ[sideIndex]*LVec[layerIndex].Item(sideIndex);
}

std::string
CH4Layer::getLayerString(const size_t sideIndex,
			 const size_t layerIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-LVec]
    \return Surface string
  */
{
  ELog::RegMethod RegA("CH4Layer","getLayerString");

  if (layerIndex>=LVec.size()) 
    throw ColErr::IndexError<size_t>(layerIndex,LVec.size(),"layerIndex");
  if (sideIndex>5)
    throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");

  const int SI(modIndex+static_cast<int>(layerIndex*10+sideIndex+1));
  std::ostringstream cx;
  const int signValue((sideIndex % 2) ? 1 : -1);
  cx<<" "<<signValue*SMap.realSurf(SI)<<" ";
  return cx.str();
}

int
CH4Layer::getLayerSurf(const size_t layerIndex,
		       const size_t sideIndex) const
  /*!
    Given a side and a layer calculate the link surf
    \param sideIndex :: Side [0-5]
    \param layerIndex :: layer, 0 is inner moderator [0-4]
    \return Surface string
  */
{
  ELog::RegMethod RegA("H2Moderator","getLayerSurf");

  if (layerIndex>=LVec.size()) 
    throw ColErr::IndexError<size_t>(layerIndex,LVec.size(),"layerIndex");
  if (sideIndex>5)
    throw ColErr::IndexError<size_t>(sideIndex,5,"sideIndex ");

  const int SI(modIndex+static_cast<int>(layerIndex*10+sideIndex+1));
  return SMap.realSurf(SI);
}


void
CH4Layer::createLinks()
  /*!
    Creates a full attachment set
  */
{
  // set Links:
  ELog::RegMethod RegA("CH4Layer","createLinks");

  const int ch4Layer(modIndex+
		     (static_cast<int>(LVec.size())-1)*10);
  FixedComp::setLinkSurf(0,-SMap.realSurf(ch4Layer+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(ch4Layer+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(ch4Layer+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(ch4Layer+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(ch4Layer+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(ch4Layer+6));

  return;
}

void
CH4Layer::createAll(Simulation& System,
		    const attachSystem::FixedComp& FC)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("CH4Layer","createAll");
  populate(System);


  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  return;
}

  
}  // NAMESPACE ts1System
