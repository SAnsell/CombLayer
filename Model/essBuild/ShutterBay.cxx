/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essBuild/ShutterBay.cxx
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
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "BaseMap.h"
#include "CellMap.h"
#include "ShutterBay.h"

namespace essSystem
{

ShutterBay::ShutterBay(const std::string& Key)  :
  attachSystem::ContainedComp(),attachSystem::FixedOffset(Key,8),
  attachSystem::CellMap(),
  bulkIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(bulkIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

ShutterBay::ShutterBay(const ShutterBay& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedOffset(A),
  attachSystem::CellMap(A),
  bulkIndex(A.bulkIndex),cellIndex(A.cellIndex),
  radius(A.radius),height(A.height),depth(A.depth),
  skin(A.skin),topSkin(A.topSkin),
  mat(A.mat),skinMat(A.skinMat)
  /*!
    Copy constructor
    \param A :: ShutterBay to copy
  */
{}

ShutterBay&
ShutterBay::operator=(const ShutterBay& A)
  /*!
    Assignment operator
    \param A :: ShutterBay to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedOffset::operator=(A);
      attachSystem::CellMap::operator=(A),
      cellIndex=A.cellIndex;
      radius=A.radius;
      height=A.height;
      depth=A.depth;
      skin=A.skin;
      topSkin=A.topSkin;
      mat=A.mat;
      skinMat=A.skinMat;
    }
  return *this;
}

ShutterBay::~ShutterBay() 
  /*!
    Destructor
  */
{}

void
ShutterBay::populate(const FuncDataBase& Control)
 /*!
   Populate all the variables
   \param Control :: Variable data base
 */
{
  ELog::RegMethod RegA("ShutterBay","populate");
  
  FixedOffset::populate(Control);
  radius=Control.EvalVar<double>(keyName+"Radius");
  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  skin=Control.EvalVar<double>(keyName+"Skin");
  topSkin=Control.EvalVar<double>(keyName+"TopSkin");
  cutSkin=Control.EvalVar<double>(keyName+"CutSkin");
  topCut=Control.EvalVar<double>(keyName+"TopCut");
  topRadius=Control.EvalVar<double>(keyName+"TopRadius");

  NCurtain=Control.EvalVar<size_t>(keyName+"NCurtain");
  NCurtain=std::max<size_t>(1,NCurtain);
  double thick(0.0);
  curLayer.push_back(topCut);
  for(size_t i=0;i<NCurtain;i++)
    {
      const std::string NStr=std::to_string(i);
      if (i+1!=NCurtain)
        {
          const double T=Control.EvalVar<double>(keyName+"CurtainThick"+NStr);
          thick+=T;
          if (thick>=topCut)
            throw ColErr::SizeError<double>
              (thick,topCut,"TopCut < thickness at layer:"+NStr);
          curLayer.push_back(topCut-thick);
        }
      const int cMat=
        ModelSupport::EvalMat<int>(Control,keyName+"CurtainMat"+NStr);
      curMat.push_back(cMat);
    }
  
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");
  skinMat=ModelSupport::EvalMat<int>(Control,keyName+"SkinMat");

  return;
}
  
void
ShutterBay::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Linked object
  */
{
  ELog::RegMethod RegA("ShutterBay","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyOffset();

  return;
}
  
void
ShutterBay::createSurfaces()
  /*!
    Create All the surfaces
  */
{
  ELog::RegMethod RegA("ShutterBay","createSurface");

  // rotation of axis:

  // Dividing planes:
  ModelSupport::buildPlane(SMap,bulkIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,bulkIndex+2,Origin,X);

  ModelSupport::buildPlane(SMap,bulkIndex+5,Origin-Z*depth,Z);
  ModelSupport::buildPlane(SMap,bulkIndex+6,Origin+Z*height,Z);
  ModelSupport::buildPlane(SMap,bulkIndex+16,Origin+Z*(topSkin+height),Z);
  ModelSupport::buildPlane(SMap,bulkIndex+106,Origin+Z*(height-topCut),Z);


  int CL(bulkIndex+200);
  for(size_t i=0;i<NCurtain;i++)
    {
      ModelSupport::buildPlane
        (SMap,CL+6,Origin+Z*(height+cutSkin-curLayer[i]),Z);
      CL+=10;
    }
  
  ModelSupport::buildCylinder(SMap,bulkIndex+7,Origin,Z,radius);
  ModelSupport::buildCylinder(SMap,bulkIndex+17,Origin,Z,radius+skin);
  ModelSupport::buildCylinder(SMap,bulkIndex+107,Origin,Z,topRadius);
  ModelSupport::buildCylinder(SMap,bulkIndex+117,Origin,Z,topRadius+skin);
  
  return;
}

void
ShutterBay::createObjects(Simulation& System,
			  const attachSystem::ContainedComp& CC)
  /*!
    Adds the all the components
    \param System :: Simulation to create objects in
    \param CC :: Bulk object
  */
{
  ELog::RegMethod RegA("ShutterBay","createObjects");

  std::string Out;
  Out=ModelSupport::getComposite(SMap,bulkIndex,"5 -106 -7 ");
  Out+=CC.getExclude();  
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  addCell("MainCell",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,bulkIndex," 106 -107 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out));
  addCell("TopCell",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,bulkIndex,"5 -106 7 -17 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,skinMat,0.0,Out));
  addCell("Skin",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,bulkIndex," 106 -206 107 -17 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,skinMat,0.0,Out));
  addCell("Skin",cellIndex-1);
  
  Out=ModelSupport::getComposite(SMap,bulkIndex,"206 -6 107 -117 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,skinMat,0.0,Out));
  addCell("Skin",cellIndex-1);

  Out=ModelSupport::getComposite(SMap,bulkIndex," 6 -16 -117 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,skinMat,0.0,Out));
  addCell("Skin",cellIndex-1);

  int CL(bulkIndex+200);
  for(size_t i=1;i<NCurtain;i++)
    {
      Out=ModelSupport::getComposite
        (SMap,bulkIndex,CL," 117 -17 6M -16M ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,curMat[i-1],0.0,Out));
      addCell("Curtain",cellIndex-1);
      CL+=10;
    }
  Out=ModelSupport::getComposite(SMap,bulkIndex,CL," 117 -17 6M -16 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,curMat.back(),0.0,Out));
  addCell("Curtain",cellIndex-1);

  

  
  Out=ModelSupport::getComposite(SMap,bulkIndex," 5 -16 -17 ");
  addOuterSurf(Out);

  return;
}

void
ShutterBay::createLinks()
  /*!
    Create all the linkes [OutGoing]
  */
{
  ELog::RegMethod RegA("ShutterBay","createLinks");

  FixedComp::setConnect(0,Origin-Y*(radius+skin),-Y);   // outer point
  FixedComp::setConnect(1,Origin+Y*(radius+skin),Y);   // outer point
  FixedComp::setConnect(2,Origin-X*(radius+skin),-X);   // outer point
  FixedComp::setConnect(3,Origin+X*(radius+skin),X);   // outer point
  FixedComp::setConnect(4,Origin-Z*depth,-Z);  // base
  FixedComp::setConnect(5,Origin+Z*(height+topSkin),Z);  // 

  FixedComp::setLinkSurf(0,SMap.realSurf(bulkIndex+17));
  FixedComp::setLinkSurf(1,SMap.realSurf(bulkIndex+17));
  FixedComp::setLinkSurf(2,SMap.realSurf(bulkIndex+17));
  FixedComp::setLinkSurf(3,SMap.realSurf(bulkIndex+17));

  FixedComp::addBridgeSurf(0,-SMap.realSurf(bulkIndex+1));
  FixedComp::addBridgeSurf(1,SMap.realSurf(bulkIndex+1));
  FixedComp::addBridgeSurf(2,-SMap.realSurf(bulkIndex+2));
  FixedComp::addBridgeSurf(3,SMap.realSurf(bulkIndex+2));
  FixedComp::setLinkSurf(4,-SMap.realSurf(bulkIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(bulkIndex+16));

  FixedComp::setLinkSurf(6,SMap.realSurf(bulkIndex+7));
  FixedComp::setLinkSurf(7,SMap.realSurf(bulkIndex+7));

  FixedComp::setConnect(6,Origin-Y*radius,-Y);   // materila point
  FixedComp::setConnect(7,Origin+Y*radius,Y);    // material point


  return;
}

void
ShutterBay::createAll(Simulation& System,
		      const attachSystem::FixedComp& FC,
		      const attachSystem::ContainedComp& CC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Central origin
    \param CC :: Central origin
  */
{
  ELog::RegMethod RegA("ShutterBay","createAll");

  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createLinks();
  createObjects(System,CC);
  insertObjects(System);              

  return;
}

}  // NAMESPACE essSystem
