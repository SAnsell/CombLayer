/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   muon/muBeamWindow.cxx
 *
 * Copyright (c) 2004-2019 by Stuart Ansell/Goran Skoro
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
#include <memory>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "Vec3D.h"
#include "surfRegister.h"
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "ContainedComp.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "muBeamWindow.h"

namespace muSystem
{

muBeamWindow::muBeamWindow(const std::string& Key)  : 
  attachSystem::FixedRotate(Key,4),
  attachSystem::ContainedComp()
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Key to use
  */
{}


muBeamWindow::muBeamWindow(const muBeamWindow& A) : 
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  flCylOutRadius(A.flCylOutRadius),flCylInRadius(A.flCylInRadius),
  flCylThick(A.flCylThick),windowThick(A.windowThick),
  smCylOutRadius(A.smCylOutRadius),smCylThick(A.smCylThick),
  smRadStartCone(A.smRadStartCone),smRadStopCone(A.smRadStopCone),
  smLengthCone(A.smLengthCone),bgOutRadStartCone(A.bgOutRadStartCone),
  bgOutRadStopCone(A.bgOutRadStopCone),bgOutLengthCone(A.bgOutLengthCone),
  bgInRadStartCone(A.bgInRadStartCone),bgInRadStopCone(A.bgInRadStopCone),
  bgInLengthCone(A.bgInLengthCone),endRadCyl(A.endRadCyl),
  endRadStartCone(A.endRadStartCone),endRadStopCone(A.endRadStopCone),
  endLength(A.endLength),bigCylOutRadius(A.bigCylOutRadius),
  bigCylInRadius(A.bigCylInRadius),bigCylThick(A.bigCylThick),
  vessMat(A.vessMat),windowMat(A.windowMat)
  /*!
    Copy constructor
    \param A :: muBeamWindow to copy
  */
{}

muBeamWindow&
muBeamWindow::operator=(const muBeamWindow& A)
  /*!
    Assignment operator
    \param A :: muBeamWindow to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ContainedComp::operator=(A);
      flCylOutRadius=A.flCylOutRadius;
      flCylInRadius=A.flCylInRadius;
      flCylThick=A.flCylThick;
      windowThick=A.windowThick;
      smCylOutRadius=A.smCylOutRadius;
      smCylThick=A.smCylThick;
      smRadStartCone=A.smRadStartCone;
      smRadStopCone=A.smRadStopCone;
      smLengthCone=A.smLengthCone;
      bgOutRadStartCone=A.bgOutRadStartCone;
      bgOutRadStopCone=A.bgOutRadStopCone;
      bgOutLengthCone=A.bgOutLengthCone;
      bgInRadStartCone=A.bgInRadStartCone;
      bgInRadStopCone=A.bgInRadStopCone;
      bgInLengthCone=A.bgInLengthCone;
      endRadCyl=A.endRadCyl;
      endRadStartCone=A.endRadStartCone;
      endRadStopCone=A.endRadStopCone;
      endLength=A.endLength;
      bigCylOutRadius=A.bigCylOutRadius;
      bigCylInRadius=A.bigCylInRadius;
      bigCylThick=A.bigCylThick;
      vessMat=A.vessMat;
      windowMat=A.windowMat;
    }
  return *this;
}

muBeamWindow::~muBeamWindow() 
  /*!
    Destructor
  */
{}

void
muBeamWindow::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Data Base for variables
  */
{
  ELog::RegMethod RegA("muBeamWindow","populate");

  FixedRotate::populate(Control);
     // flange cylinder
  flCylOutRadius=Control.EvalVar<double>(keyName+"FlCylOutRadius");
  flCylInRadius=Control.EvalVar<double>(keyName+"FlCylInRadius"); 
  flCylThick=Control.EvalVar<double>(keyName+"FlCylThick");  
     // window
  windowThick=Control.EvalVar<double>(keyName+"WindowThick"); 
     // small cylinder
  smCylOutRadius=Control.EvalVar<double>(keyName+"SmCylOutRadius");
  smCylThick=Control.EvalVar<double>(keyName+"SmCylThick");  
     // small cone               
  smRadStartCone=Control.EvalVar<double>(keyName+"SmRadStartCone");       
  smRadStopCone=Control.EvalVar<double>(keyName+"SmRadStopCone");  
  smLengthCone=Control.EvalVar<double>(keyName+"SmLengthCone");     
     // big cone out               
  bgOutRadStartCone=Control.EvalVar<double>(keyName+"BgOutRadStartCone");       
  bgOutRadStopCone=Control.EvalVar<double>(keyName+"BgOutRadStopCone");  
  bgOutLengthCone=Control.EvalVar<double>(keyName+"BgOutLengthCone");     
     // big cone in               
  bgInRadStartCone=Control.EvalVar<double>(keyName+"BgInRadStartCone");       
  bgInRadStopCone=Control.EvalVar<double>(keyName+"BgInRadStopCone");  
  bgInLengthCone=Control.EvalVar<double>(keyName+"BgInLengthCone");     
     // end cone              
  endRadCyl=Control.EvalVar<double>(keyName+"EndRadCyl");
  endRadStartCone=Control.EvalVar<double>(keyName+"EndRadStartCone");       
  endRadStopCone=Control.EvalVar<double>(keyName+"EndRadStopCone");  
  endLength=Control.EvalVar<double>(keyName+"EndLength");   
     // big cylinder
  bigCylOutRadius=Control.EvalVar<double>(keyName+"BigCylOutRadius");
  bigCylInRadius=Control.EvalVar<double>(keyName+"BigCylInRadius"); 
  bigCylThick=Control.EvalVar<double>(keyName+"BigCylThick");  
     // materials
  vessMat=ModelSupport::EvalMat<int>(Control,keyName+"VessMat");
  windowMat=ModelSupport::EvalMat<int>(Control,keyName+"WindowMat");
         
  return;
}

void
muBeamWindow::createSurfaces()
  /*!
    Create all the surfaces
  */
{
  ELog::RegMethod RegA("muBeamWindow","createSurface");

  const double angleCsm=180.0*
    atan((smRadStopCone-smRadStartCone)/smLengthCone)/M_PI; 
  const double offsetCsm=
    smRadStartCone/tan(angleCsm*M_PI/180.0);

  const double anglebgOut=
    180.0*atan((bgOutRadStopCone-bgOutRadStartCone)/bgOutLengthCone)/M_PI; 
  const double offsetbgOut=
    bgOutRadStartCone/tan(anglebgOut*M_PI/180.0);

  const double anglebgIn=
    180.0*atan((bgInRadStopCone-bgInRadStartCone)/bgInLengthCone)/M_PI; 
  const double offsetbgIn=
    bgInRadStartCone/tan(anglebgIn*M_PI/180.0);

  const double angleEnd=
    180.0*atan((endRadStopCone-endRadStartCone)/endLength)/M_PI; 
  const double offsetEnd=
    endRadStartCone/tan(angleEnd*M_PI/180.0);

  //  
  //  flange cylinder
  ModelSupport::buildPlane(SMap,buildIndex+1,Origin,Y);
  ModelSupport::buildPlane(SMap,buildIndex+2,Origin+Y*flCylThick,Y);
  ModelSupport::buildCylinder(SMap,buildIndex+7,Origin,Y,flCylOutRadius);
  ModelSupport::buildCylinder(SMap,buildIndex+8,Origin,Y,flCylInRadius);
     // window
  ModelSupport::buildPlane(SMap,buildIndex+11,Origin+Y*(flCylThick-windowThick),Y);     
     // small cylinder - cone         
  ModelSupport::buildPlane(SMap,buildIndex+21,
			   Origin+Y*(flCylThick+smCylThick),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+27,
			      Origin+Y*flCylThick,Y,smCylOutRadius);  
  ModelSupport::buildCone(SMap,buildIndex+28,
			  Origin+Y*(flCylThick-offsetCsm),Y,angleCsm);   
     // big cone         
  ModelSupport::buildPlane(SMap,buildIndex+31,
			   Origin+Y*(flCylThick+smCylThick+bgOutLengthCone),Y);
  ModelSupport::buildCone(SMap,buildIndex+37,
			  Origin+Y*(flCylThick+smCylThick-offsetbgOut),
			  Y,anglebgOut); 

  ModelSupport::buildCone(SMap,buildIndex+38,
			  Origin+Y*(flCylThick+smCylThick-offsetbgIn),
			  Y,anglebgIn);   
     // end cone         
  ModelSupport::buildPlane(SMap,buildIndex+41,
			   Origin+Y*(flCylThick+smCylThick+
				     bgOutLengthCone+endLength),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+47,
			      Origin+Y*(flCylThick+smCylThick+bgOutLengthCone),
			      Y,endRadCyl);  

  ModelSupport::buildCone(SMap,buildIndex+48,
			  Origin+Y*(flCylThick+smCylThick+
				    bgOutLengthCone-offsetEnd),Y,angleEnd);   
  //  big cylinder
  ModelSupport::buildPlane(SMap,buildIndex+51,
			   Origin+Y*(flCylThick+smCylThick+
				     bgOutLengthCone+endLength+bigCylThick),Y);
  ModelSupport::buildCylinder(SMap,buildIndex+57,
			      Origin+Y*(flCylThick+smCylThick+
					bgOutLengthCone+endLength),
			      Y,bigCylOutRadius);

  ModelSupport::buildCylinder(SMap,buildIndex+58,
			      Origin+Y*(flCylThick+smCylThick+
					bgOutLengthCone+endLength),
			      Y,bigCylInRadius);

  return;
}

void
muBeamWindow::createObjects(Simulation& System)
  /*!
    Adds the Cone colimator components
    \param System :: Simulation to create objects in
   */
{
  ELog::RegMethod RegA("muBeamWindow","createObjects");
  
  std::string Out;

     // flange cylinder
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -2 -7 ");
  addOuterSurf(Out);
  addBoundarySurf(Out);
  Out+=ModelSupport::getComposite(SMap,buildIndex,"8 ");  
  System.addCell(MonteCarlo::Object(cellIndex++,vessMat,0.0,Out));

     // window
  Out=ModelSupport::getComposite(SMap,buildIndex,"11 -2 -8 ");
  System.addCell(MonteCarlo::Object(cellIndex++,windowMat,0.0,Out));

    // front void
  Out=ModelSupport::getComposite(SMap,buildIndex,"1 -11 -8 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));

     // small cylinder - cone  
  Out=ModelSupport::getComposite(SMap,buildIndex,"2 -21 -27 ");
  addOuterUnionSurf(Out);
  addBoundaryUnionSurf(Out);
  Out+=ModelSupport::getComposite(SMap,buildIndex,"28 ");  
  System.addCell(MonteCarlo::Object(cellIndex++,vessMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,"2 -21 -28 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  
     // big cone  
  Out=ModelSupport::getComposite(SMap,buildIndex,"21 -31 -37 ");
  addOuterUnionSurf(Out);
  addBoundaryUnionSurf(Out);
  Out+=ModelSupport::getComposite(SMap,buildIndex,"38 ");  
  System.addCell(MonteCarlo::Object(cellIndex++,vessMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,"21 -31 -38 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));    
  
     // end cylinder - cone  
  Out=ModelSupport::getComposite(SMap,buildIndex,"31 -41 -47 ");
  addOuterUnionSurf(Out);
  addBoundaryUnionSurf(Out);
  Out+=ModelSupport::getComposite(SMap,buildIndex,"48 ");  
  System.addCell(MonteCarlo::Object(cellIndex++,vessMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,"31 -41 -48 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out));
  
      // big cylinder
  Out=ModelSupport::getComposite(SMap,buildIndex,"41 -51 -57 ");
  addOuterUnionSurf(Out);
  addBoundaryUnionSurf(Out);
  Out+=ModelSupport::getComposite(SMap,buildIndex,"58 ");  
  System.addCell(MonteCarlo::Object(cellIndex++,vessMat,0.0,Out));

  Out=ModelSupport::getComposite(SMap,buildIndex,"41 -51 -58 ");
  System.addCell(MonteCarlo::Object(cellIndex++,0,0.0,Out)); 
    
  return;
}


void
muBeamWindow::createLinks()
  /*!
    Create links
   */
{
  ELog::RegMethod RegA("muBeamWindow","createLinks");

  const double length(flCylThick+smCylThick+
		      bgOutLengthCone+endLength+bigCylThick);
  FixedComp::setConnect(0,Origin,-Y);
  FixedComp::setConnect(1,Origin+Y*(flCylThick+smCylThick+
				    bgOutLengthCone+endLength+bigCylThick),Y);
  FixedComp::setConnect(2,Origin+Y*length/2.0-Z*flCylOutRadius,-Z);
  FixedComp::setConnect(3,Origin+Y*length/2.0+Z*flCylOutRadius,Z);

  FixedComp::setLinkSurf(0,-SMap.realSurf(buildIndex+1));
  FixedComp::setLinkSurf(1,SMap.realSurf(buildIndex+51));  
  FixedComp::setLinkSurf(2,SMap.realSurf(buildIndex+7));  
  FixedComp::setLinkSurf(3,SMap.realSurf(buildIndex+7));  

  return;
}

void
muBeamWindow::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Create the flange and window
    \param System :: Simulation to process
    \param FC :: Fixed Comp
    \param sideIndex :: Link point
  */
{
  ELog::RegMethod RegA("muBeamWindow","createAll");
  populate(System.getDataBase());
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);
  return;
}
  
}  // NAMESPACE muSystem

