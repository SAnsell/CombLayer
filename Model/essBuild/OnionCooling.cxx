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
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "surfRegister.h"
#include "Vec3D.h"
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
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedRotate.h"
#include "ContainedComp.h"
#include "ExternalCut.h"
#include "BaseMap.h"
#include "CellMap.h"

#include "OnionCooling.h"


namespace essSystem
{

OnionCooling::OnionCooling(const std::string& Key) :
  attachSystem::FixedRotate(Key,3),
  attachSystem::ContainedComp(),
  attachSystem::ExternalCut(),
  attachSystem::CellMap()
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{}

OnionCooling::OnionCooling(const OnionCooling& A) :
  attachSystem::FixedRotate(A),
  attachSystem::ContainedComp(A),
  attachSystem::ExternalCut(A),
  attachSystem::CellMap(A),
  wallThick(A.wallThick),
  wallMat(A.wallMat),wallTemp(A.wallTemp),
  nRings(A.nRings),
  radius(A.radius),
  gateWidth(A.gateWidth),gateLength(A.gateLength)
  /*!
    Copy constructor
    \param A :: DiskPreMod to copy
  */
{}

  
OnionCooling&
OnionCooling::operator=(const OnionCooling& A)
  /*!
    Assignment operator
    \param A :: DiskPreMod to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedRotate::operator=(A);
      attachSystem::ExternalCut::operator=(A);
      attachSystem::CellMap::operator=(A);
      wallThick=A.wallThick;
      wallMat=A.wallMat;
      wallTemp=A.wallTemp;
      nRings=A.nRings;
      radius=A.radius;
      gateWidth=A.gateWidth;
      gateLength=A.gateLength;
   }
  return *this;
}

  
OnionCooling*
OnionCooling::clone() const
  /*!
    Clone self
    \return new (this)
   */
{
  return new OnionCooling(*this);
}


OnionCooling::~OnionCooling()
  /*!
    Destructor
   */
{}

void
OnionCooling::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("OnionCooling","populate");

  FixedRotate::populate(Control);

  //  height=Control.EvalVar<double>(keyName+"Height");   
  wallThick=Control.EvalVar<double>(keyName+"WallThick");   
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");   
  wallTemp=Control.EvalVar<double>(keyName+"WallTemp");   

  nRings = Control.EvalVar<size_t>(keyName+"NRings");
  
  for (size_t i=1;i<=nRings; i++)
    {
      const std::string indexStr=std::to_string(i);
      radius.push_back(Control.EvalVar<double>(keyName+"Radius"+indexStr));
      gateWidth.push_back
	(Control.EvalVar<double>(keyName+"GateWidth"+indexStr));
      gateLength.push_back
	(Control.EvalVar<double>(keyName+"GateLength"+indexStr));
    }
  
  return;
}

void
OnionCooling::createSurfaces()
  /*!
    Create Surfaces for the Be
  */
{
  ELog::RegMethod RegA("OnionCooling","createSurfaces");
      
  //  ModelSupport::buildPlane(SMap,buildIndex+1, Origin-Z*(height/2.0), Z);
  //  ModelSupport::buildPlane(SMap,buildIndex+2, Origin+Z*(height/2.0), Z);

  int SI(buildIndex);
  for (size_t i=0; i<=nRings; i++)
    {
      ModelSupport::buildCylinder(SMap, SI+3, Origin,Z,radius[i]);
      ModelSupport::buildCylinder(SMap, SI+4,Origin,Z,radius[i]+wallThick);  
      
    // gate:
    // upper door
      ModelSupport::buildPlane(SMap, SI+5,
			       Origin+Y*(gateWidth[i]/2.0), Y); 
      ModelSupport::buildPlane(SMap, SI+6,
			       Origin+Y*(gateWidth[i]/2.0+wallThick), Y);
      ModelSupport::buildPlane(SMap, SI+7,
			       Origin-X*(radius[i]+gateLength[i]), X);
      ModelSupport::buildPlane(SMap, SI+8,
			       Origin+X*(radius[i]+gateLength[i]), X);

    // upper door
      ModelSupport::buildPlane(SMap, SI+9,
			       Origin-Y*(gateWidth[i]/2.0+wallThick), Y); 
      ModelSupport::buildPlane(SMap, SI+10,
			       Origin-Y*(gateWidth[i]/2.0), Y);

      SI += 10;
    }
  return; 
}


void
OnionCooling::createObjects(Simulation& System,
			    const attachSystem::FixedComp& FC)
  /*!
    Create the onion piping
    \param System :: Simulation to add results
    \param FC :: Fixd unit ot get cell map from
   */
{
  ELog::RegMethod RegA("OnionCooling","createObjects");

  const int innerCell=getCell("Inner");
  MonteCarlo::Object* InnerObj=System.findObject(innerCell);
  if (!InnerObj)
    throw ColErr::InContainerError<int>
      (innerCell,"Inner Cell not found");

  HeadRule HR;
  const HeadRule tbHR=getRule("Top")*getRule("Base");
  HeadRule wallExclude;
  
  int SI(buildIndex);
  for (size_t i=1; i<=nRings; i++)
    {
      // upper half-ring
      HR=ModelSupport::getHeadRule
	(SMap, SI, buildIndex,"7 -8 5 -6 (3 : 6) 3 -4");
      System.addCell(cellIndex++,wallMat,wallTemp,HR*tbHR);
      addOuterUnionSurf(HR);
      wallExclude.addUnion(HR);
      
      // bottom half-ring
      HR=ModelSupport::getHeadRule
	(SMap, SI, buildIndex,"7 -8 -10 9 (3: -9) 3 -4");
      System.addCell(cellIndex++, wallMat, wallTemp, HR*tbHR);
      addOuterUnionSurf(HR);
      wallExclude.addUnion(HR);
      SI += 10;
    }

  wallExclude.makeComplement();
  InnerObj->addIntersection(wallExclude);

  return; 

}

void
OnionCooling::createLinks()
  /*!
    Creates a full attachment set
    Links/directions going outwards true.
  */
{
  return;
}


void OnionCooling::createAll(Simulation& System,
			     const attachSystem::FixedComp& FC,
			     const long int sideIndex)
{
  /*!  Extrenal build everything 

    In our case the reflector has to be built relative to an origin 
    and an axes set.  If you take a simple
    fixed object, then the axes is the axes set of this fixed object
    and the origin is the origin of this fixed object, which does not
    mean that the reflector and the object have the same origin,
    that's just the way you start and then you add the next bits.

    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \parma sideIndex :: lin point
   */

  ELog::RegMethod RegA("OnionCooling","createAll");
  // the order matters:

  populate(System.getDataBase()); // populate variables
  createUnitVector(FC,sideIndex); 
  createSurfaces();
  createObjects(System,FC);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE ess<System
