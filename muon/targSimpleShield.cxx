/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   muon/targSimpleShield.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell/Goran Skoro
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
#include <iterator>
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
#include "SimProcess.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "targSimpleShield.h"

namespace muSystem
{

targSimpleShield::targSimpleShield(const std::string& Key)  : 
  attachSystem::FixedComp(Key,6),attachSystem::ContainedComp(),
  targShieldIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(targShieldIndex+1)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}

targSimpleShield::targSimpleShield(const targSimpleShield& A) : 
  attachSystem::FixedComp(A),attachSystem::ContainedComp(A),
  targShieldIndex(A.targShieldIndex),cellIndex(A.cellIndex),
  xStep(A.xStep),yStep(A.yStep),zStep(A.zStep),
  xyAngle(A.xyAngle),height(A.height),depth(A.depth),
  width(A.width),baseThick(A.baseThick),forwThick(A.forwThick),
  backThick(A.backThick),muonThick(A.muonThick),
  japThick(A.japThick),mat(A.mat)
  /*!
    Copy constructor
    \param A :: targSimpleShield to copy
  */
{}

targSimpleShield&
targSimpleShield::operator=(const targSimpleShield& A)
  /*!
    Assignment operator
    \param A :: targSimpleShield to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedComp::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      cellIndex=A.cellIndex;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      height=A.height;
      depth=A.depth;
      width=A.width;
      baseThick=A.baseThick;
      forwThick=A.forwThick;
      backThick=A.backThick;
      muonThick=A.muonThick;
      japThick=A.japThick;
      mat=A.mat;
    }
  return *this;
}


targSimpleShield::~targSimpleShield() 
  /*!
    Destructor
  */
{}

void
targSimpleShield::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("targSimpleShield","populate");

  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYAngle");

  height=Control.EvalVar<double>(keyName+"Height");
  depth=Control.EvalVar<double>(keyName+"Depth");
  width=Control.EvalVar<double>(keyName+"Width");
  
  baseThick=Control.EvalVar<double>(keyName+"BaseThick");
  forwThick=Control.EvalVar<double>(keyName+"ForwThick"); 
  backThick=Control.EvalVar<double>(keyName+"BackThick");
  muonThick=Control.EvalVar<double>(keyName+"MuonThick");
  japThick=Control.EvalVar<double>(keyName+"JapThick");    
     
  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat");    
       
  return;
}

void
targSimpleShield::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed Unit for origin						
  */
{
  ELog::RegMethod RegA("targSimpleShield","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,0);    
  
  return;
}

void
targSimpleShield::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("targSimpleShield","createSurface");

  // outer layer
  ModelSupport::buildPlane(SMap,targShieldIndex+1,Origin-Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,targShieldIndex+2,Origin+Y*depth/2.0,Y);
  ModelSupport::buildPlane(SMap,targShieldIndex+3,Origin-X*width/2.0,X);
  ModelSupport::buildPlane(SMap,targShieldIndex+4,Origin+X*width/2.0,X);
  ModelSupport::buildPlane(SMap,targShieldIndex+5,Origin-Z*height/2.0,Z);
  ModelSupport::buildPlane(SMap,targShieldIndex+6,Origin+Z*height/2.0,Z);
  
  // shield layer
  ModelSupport::buildPlane(SMap,targShieldIndex+11,Origin-Y*(depth/2.0-backThick),Y);
  ModelSupport::buildPlane(SMap,targShieldIndex+12,Origin+Y*(depth/2.0-forwThick),Y);
  ModelSupport::buildPlane(SMap,targShieldIndex+13,Origin-X*(width/2.0-japThick),X);
  ModelSupport::buildPlane(SMap,targShieldIndex+14,Origin+X*(width/2.0-muonThick),X);
  ModelSupport::buildPlane(SMap,targShieldIndex+15,Origin-Z*(height/2.0-baseThick),Z);
//  ModelSupport::buildPlane(SMap,targShieldIndex+16,Origin+Z*(height/2.0-shieldThick),Z);  


  return;
}

void
targSimpleShield::addToInsertChain(attachSystem::ContainedComp& CC) const
  /*!
    Adds this object to the containedComp to be inserted.
    \param CC :: ContainedComp object to add to this
  */
{
  for(int i=targShieldIndex+1;i<cellIndex;i++)
    CC.addInsertCell(i);
  return;
}

void
targSimpleShield::createObjects(Simulation& System)
  /*!
    Adds the Chip guide components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("targSimpleShield","createObjects");
  
  std::string Out;
  std::string Out1;

    // shield
  Out=ModelSupport::getComposite(SMap,targShieldIndex,"1 -2 3 -4 5 -6 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);
  Out1=ModelSupport::getComposite(SMap,targShieldIndex,"(-11:12:-13:14:-15) ");  
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out+Out1));

    // hole
  Out=ModelSupport::getComposite(SMap,targShieldIndex,"11 -12 13 -14 15 -6 ");
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out));
  
  return;
}


void
targSimpleShield::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("targSimpleShield","createLinks");

  FixedComp::setLinkSurf(0,-SMap.realSurf(targShieldIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(targShieldIndex+2));
  FixedComp::setLinkSurf(2,-SMap.realSurf(targShieldIndex+3));
  FixedComp::setLinkSurf(3,SMap.realSurf(targShieldIndex+4));
  FixedComp::setLinkSurf(4,-SMap.realSurf(targShieldIndex+5));
  FixedComp::setLinkSurf(5,SMap.realSurf(targShieldIndex+6));

  return;
}

void
targSimpleShield::createAll(Simulation& System,
			    const attachSystem::FixedComp& FC)

  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param FC :: Fixed Component to place object within
  */
{
  ELog::RegMethod RegA("targSimpleShield","createAll");
  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE shutterSystem
