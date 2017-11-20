/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/IradCylinder.cxx
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
#include "surfRegister.h"
#include "objectRegister.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Surface.h"
#include "Quadratic.h"
#include "Cylinder.h"
#include "surfIndex.h"
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
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ContainedComp.h"

#include "IradCylinder.h"

namespace essSystem
{

IradCylinder::IradCylinder(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6),
  attachSystem::CellMap(),
  iradIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(iradIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

IradCylinder::IradCylinder(const IradCylinder& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  iradIndex(A.iradIndex),cellIndex(A.cellIndex),
  sampleActive(A.sampleActive),radius(A.radius),
  length(A.length),wallThick(A.wallThick),temp(A.temp),
  mat(A.mat),wallMat(A.wallMat),sampleX(A.sampleX),
  sampleY(A.sampleY),sampleZ(A.sampleZ)
  /*!
    Copy constructor
    \param A :: IradCylinder to copy
  */
{}

IradCylinder&
IradCylinder::operator=(const IradCylinder& A)
  /*!
    Assignment operator
    \param A :: IradCylinder to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A);
      cellIndex=A.cellIndex;
      sampleActive=A.sampleActive;
      radius=A.radius;
      length=A.length;
      wallThick=A.wallThick;
      temp=A.temp;
      mat=A.mat;
      wallMat=A.wallMat;
      sampleX=A.sampleX;
      sampleY=A.sampleY;
      sampleZ=A.sampleZ;
    }
  return *this;
}

  
IradCylinder*
IradCylinder::clone() const
 /*!
   Clone self 
   \return new (this)
 */
{
  return new IradCylinder(*this);
}

IradCylinder::~IradCylinder()
  /*!
    Destructor
  */
{}
  
void
IradCylinder::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("IradCylinder","populate");

  FixedOffset::populate(Control);
  
  radius=Control.EvalVar<double>(keyName+"Radius");
  length=Control.EvalVar<double>(keyName+"Length");
  const size_t NLayer=Control.EvalVar<size_t>(keyName+"NLayer");
  double WT;
  int WM;
  for(size_t i=0;i<NLayer;i++)
    {
      const std::string Num(StrFunc::makeString(i));
      WT=Control.EvalPair<double>(keyName+"WallThick"+Num,
                                         keyName+"WallThick");
      WM=ModelSupport::EvalMat<int>(Control,
                                    keyName+"WallMat"+Num,
                                    keyName+"WallMat");
      wallThick.push_back(WT);
      wallMat.push_back(WM);      
    }
      
  temp=Control.EvalVar<double>(keyName+"Temp");
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");

  // INNER SAMPLES:
  sampleActive=Control.EvalDefVar<int>(keyName+"SampleActive", 1);
  if (sampleActive)
    {
      sampleX=Control.EvalVar<double>(keyName+"SampleX");
      sampleY=Control.EvalVar<double>(keyName+"SampleY");
      sampleZ=Control.EvalVar<double>(keyName+"SampleZ");
    }
  return;
}

void
IradCylinder::createUnitVector(const attachSystem::FixedComp& FC,
				      const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Centre for object
    \param sideIndex :: Inner link point
  */
{
  ELog::RegMethod RegA("IradCylinder","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC,sideIndex);
  applyOffset();
  return;
}

void
IradCylinder::createInnerObjects(Simulation& System)
  /*!
    Create the inner material units
    \param System :: Simulation                                                
   */
{
  ELog::RegMethod RegA("IradCylinder","createInnerObjects");

  // CREATE Simple cell if samples are not active:
  if (!sampleActive)
    {
      std::string Out=ModelSupport::getComposite(SMap,iradIndex," -7 1 -2 ");

      System.addCell(MonteCarlo::Qhull(cellIndex++,mat,temp,Out));
      addCell("Samples",cellIndex-1);
      return;
    }


  // CREATE divided cell if sample Active

  const size_t NX(2*static_cast<size_t>(radius/sampleX));
  const size_t NY(static_cast<size_t>(length/sampleY));
  const size_t NZ(2*static_cast<size_t>(radius/sampleZ));

  int DI(iradIndex+1003);
  double posX(-sampleX*(static_cast<double>(NX)/2));
  for(size_t i=0;i<=NX;i++)
    {
      ModelSupport::buildPlane(SMap,DI,Origin+X*posX,X);
      posX+=sampleX;
      DI+=10;
    }
  double posY(-sampleY*(static_cast<double>(NY)/2));
  DI=iradIndex+1001;
  for(size_t i=0;i<=NY;i++)
    {
      ModelSupport::buildPlane(SMap,DI,Origin+Y*posY,Y);
      posY+=sampleY;
      DI+=10;
    }
  double posZ(-sampleZ*(static_cast<double>(NZ)/2));
  DI=iradIndex+1005;
  for(size_t i=0;i<=NZ;i++)
    {
      ModelSupport::buildPlane(SMap,DI,Origin+Z*posZ,Z);
      posZ+=sampleZ;
      DI+=10;
    }

  std::string XSurfA,XSurfB;
  std::string YSurfA,YSurfB;
  std::string ZSurfA,ZSurfB;

  DI=iradIndex+1000;
  for(size_t i=0;i<NX;i++)
    {
      XSurfA=(i) ?
        ModelSupport::getComposite(SMap,DI," 3 ") :
        ModelSupport::getComposite(SMap,iradIndex," -7 ");
      XSurfB=(i!=NX-1) ?
        ModelSupport::getComposite(SMap,DI+10," -3 ") :
        ModelSupport::getComposite(SMap,iradIndex," -7 ");
      DI+=10;

      int DJ(iradIndex+1000);
      for(size_t j=0;j<NY;j++)
        {
	  YSurfA=(j) ?
	    ModelSupport::getComposite(SMap,DJ," 1 ") :
	    ModelSupport::getComposite(SMap,iradIndex," 1 ");
	  YSurfB=(j!=NY-1) ?
	    ModelSupport::getComposite(SMap,DJ+10," -1 ") :
	    ModelSupport::getComposite(SMap,iradIndex," -2 ");
	  DJ+=10;
          
          int DK(iradIndex+1000);
          for(size_t k=0;k<NZ;k++)
            {
	      ZSurfA=(k) ?
		ModelSupport::getComposite(SMap,DK," 5 ") :
		ModelSupport::getComposite(SMap,iradIndex," -7 ");
	      ZSurfB=(k!=NZ-1) ?
		ModelSupport::getComposite(SMap,DK+10," -5 ") :
		ModelSupport::getComposite(SMap,iradIndex," -7 ");
	      DK+=10;

              std::string Out=XSurfB+YSurfB+ZSurfB+
		XSurfA+YSurfA+ZSurfA;
	      HeadRule unit(Out);
	      unit.removeCommon();
              System.addCell(MonteCarlo::Qhull(cellIndex++,mat,temp,
					       unit.display()));
              addCell("Samples",cellIndex-1);
            }
        }
    }
  
  return;
}

void
IradCylinder::createSurfaces()
  /*!
    Create planes for the inner structure iside DiskPreMod
  */
{
  ELog::RegMethod RegA("IradCylinder","createSurfaces");

  ModelSupport::buildPlane(SMap,iradIndex+1,Origin-Y*length/2.0,Y);
  ModelSupport::buildPlane(SMap,iradIndex+2,Origin+Y*length/2.0,Y);

  ModelSupport::buildCylinder(SMap,iradIndex+7,Origin,Y,radius);
  ModelSupport::buildSphere(SMap,iradIndex+8,
                            Origin-Y*(length/2.0),radius);
  ModelSupport::buildSphere(SMap,iradIndex+9,
                            Origin+Y*(length/2.0),radius);

  int IR(iradIndex);
  double WThick(0.0);
  for(size_t index=0;index<wallThick.size();index++)
    {
      WThick+=wallThick[index];
      ModelSupport::buildCylinder(SMap,IR+17,Origin,Y,radius+WThick);
      ModelSupport::buildSphere(SMap,IR+18,
                                Origin-Y*(length/2.0),radius+WThick);
      ModelSupport::buildSphere(SMap,IR+19,
                            Origin+Y*(length/2.0),radius+WThick);
      IR+=10;
    }

  return; 
}

void
IradCylinder::createObjects(Simulation& System)
/*!
    Create the objects
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("IradCylinder","createObjects");

  std::string Out;
  // Centre
  // Out=ModelSupport::getComposite(SMap,iradIndex," 1 -2 -7 ");
  // System.addCell(MonteCarlo::Qhull(cellIndex++,mat,temp,Out));
  // addCell("Centre",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,iradIndex," -1 -8 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,temp,Out));
  addCell("End",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,iradIndex," 2 -9 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,temp,Out));
  addCell("End",cellIndex-1);


  int IR(iradIndex);
  for(size_t index=0;index<wallThick.size();index++)
    {
      Out=ModelSupport::getComposite(SMap,iradIndex,IR," 1 -2 -17M 7M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat[index],temp,Out));
      addCell("Wall",cellIndex-1);
      
      Out=ModelSupport::getComposite(SMap,iradIndex,IR," -1 -18M 8M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat[index],temp,Out));
      addCell("Wall",cellIndex-1);

      Out=ModelSupport::getComposite(SMap,iradIndex,IR," 2 -19M 9M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat[index],temp,Out));
      addCell("Wall",cellIndex-1);
      IR+=10;
    }

  IR-=10;
  Out=ModelSupport::getComposite(SMap,iradIndex,IR," (1:-18M) (-2:-19M) -17M ");
  addOuterSurf(Out);
  
  return; 
}
  
void
IradCylinder::createLinks()
  /*!
    Creates a full attachment set
  */
{  
  ELog::RegMethod RegA("IradCylinder","createLinks");

  const double TThick=
    std::accumulate(wallThick.begin(),wallThick.end(),0.0);
  const int IR(iradIndex+static_cast<int>(wallThick.size())*10);
    
  FixedComp::setConnect(0,Origin-Y*(length+radius+TThick),-Y);
  FixedComp::setLinkSurf(0,SMap.realSurf(IR+8));
  FixedComp::setBridgeSurf(0,-SMap.realSurf(iradIndex+1));

  FixedComp::setConnect(1,Origin+Y*(length+radius+TThick),Y);
  FixedComp::setLinkSurf(1,SMap.realSurf(IR+9));
  FixedComp::setBridgeSurf(1,SMap.realSurf(iradIndex+2));

  FixedComp::setConnect(2,Origin-X*(radius+TThick),-X);
  FixedComp::setLinkSurf(2,SMap.realSurf(IR+7));

  FixedComp::setConnect(3,Origin+X*(radius+TThick),X);
  FixedComp::setLinkSurf(3,SMap.realSurf(IR+7));

  FixedComp::setConnect(4,Origin-Z*(radius+TThick),-Z);
  FixedComp::setLinkSurf(4,SMap.realSurf(IR+7));

  FixedComp::setConnect(5,Origin+Z*(radius+TThick),Z);
  FixedComp::setLinkSurf(5,SMap.realSurf(IR+7));


  return;
}

void
IradCylinder::createAll(Simulation& System,
                        const attachSystem::FixedComp& FC,
                        const long int sideIndex)
  /*!
    External build everything
    \param System :: Simulation
    \param FC :: Attachment point - NOTE care with rotation
    \param sideIndex :: index for point
  */
{
  ELog::RegMethod RegA("IradCylinder","createAll");
  
  
  populate(System.getDataBase()); 
  createUnitVector(FC,sideIndex);
  
  createSurfaces();
  createObjects(System);
  createInnerObjects(System);
  createLinks();
  
  insertObjects(System);       
  return;
}
  
}  // NAMESPACE essSystem
