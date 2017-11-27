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
#include <boost/shared_ptr.hpp>

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
#include "surfIndex.h"
#include "Quadratic.h"
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
#include "ContainedComp.h"

#include "BaseMap.h"
#include "CellMap.h"
#include "AttachSupport.h"

#include "OnionCooling.h"


namespace essSystem
{

OnionCooling::OnionCooling(const std::string& Key) :
  attachSystem::ContainedComp(),attachSystem::FixedComp(Key,3),
  onionIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(onionIndex+1)
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
}

OnionCooling::OnionCooling(const OnionCooling& A) :
  attachSystem::ContainedComp(A),
  attachSystem::FixedComp(A),
  onionIndex(A.onionIndex),cellIndex(A.cellIndex),
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
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
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


    // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");
  //  height=Control.EvalVar<double>(keyName+"Height");   
  wallThick=Control.EvalVar<double>(keyName+"WallThick");   
  wallMat=ModelSupport::EvalMat<int>(Control,keyName+"WallMat");   
  wallTemp=Control.EvalVar<double>(keyName+"WallTemp");   

  nRings = Control.EvalVar<size_t>(keyName+"NRings");
  for (size_t i=1;i<=nRings; i++) {
    radius.push_back(Control.EvalVar<double>(StrFunc::makeString(keyName+"Radius", i)));
    gateWidth.push_back(Control.EvalVar<double>(StrFunc::makeString(keyName+"GateWidth", i)));
    gateLength.push_back(Control.EvalVar<double>(StrFunc::makeString(keyName+"GateLength", i)));
  }
  
  return;
}

void
OnionCooling::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
  */
{
  ELog::RegMethod RegA("OnionCooling","createUnitVector");
  attachSystem::FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);
  
  return;
}

void
OnionCooling::createSurfaces()
  /*!
    Create Surfaces for the Be
  */
{
  ELog::RegMethod RegA("OnionCooling","createSurfaces");
      
  //  ModelSupport::buildPlane(SMap,onionIndex+1, Origin-Z*(height/2.0), Z);
  //  ModelSupport::buildPlane(SMap,onionIndex+2, Origin+Z*(height/2.0), Z);

  int SI(onionIndex);
  for (size_t i=0; i<=nRings; i++) {
    ModelSupport::buildCylinder(SMap, SI+3, Origin,Z,radius[i]);
    ModelSupport::buildCylinder(SMap, SI+4,Origin,Z,radius[i]+wallThick);  

    // gate:
    // upper door
    ModelSupport::buildPlane(SMap, SI+5, Origin+Y*(gateWidth[i]/2.0), Y); 
    ModelSupport::buildPlane(SMap, SI+6, Origin+Y*(gateWidth[i]/2.0+wallThick), Y);
    ModelSupport::buildPlane(SMap, SI+7, Origin-X*(radius[i]+gateLength[i]), X);
    ModelSupport::buildPlane(SMap, SI+8, Origin+X*(radius[i]+gateLength[i]), X);

    // upper door
    ModelSupport::buildPlane(SMap, SI+9, Origin-Y*(gateWidth[i]/2.0+wallThick), Y); 
    ModelSupport::buildPlane(SMap, SI+10, Origin-Y*(gateWidth[i]/2.0), Y);

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
   */
{
  ELog::RegMethod RegA("OnionCooling","createObjects");

 const attachSystem::CellMap* CM=
    dynamic_cast<const attachSystem::CellMap*>(&FC);
  MonteCarlo::Object* InnerObj(0);
  int innerCell(0);
  if (CM)
    {
      innerCell=CM->getCell("Inner");
      InnerObj=System.findQhull(innerCell);
    }
  if (!InnerObj)
    throw ColErr::InContainerError<int>
      (innerCell,"Inner Cell not found");

  std::string Out;
  const std::string topBottomStr=FC.getLinkString(7+1)+FC.getLinkString(8+1);
  HeadRule wallExclude;
  
  // [2:1381] There are 2 types of cells: object cells (Monte Carlo objects = MC qhulls)

  int SI(onionIndex);
  for (size_t i=1; i<=nRings; i++) {
    // upper half-ring
    //Out=ModelSupport::getComposite(SMap, SI, onionIndex, " (1M -2M -4) (-1M:2M:3) "); // same: (1M -2M -4 3)
    Out=ModelSupport::getComposite(SMap, SI, onionIndex, " (7 -8 5 -6 3: 6 3 -4) ");
    System.addCell(MonteCarlo::Qhull(cellIndex++, wallMat, wallTemp, Out+topBottomStr));
    addOuterUnionSurf(Out);
    wallExclude.addUnion(Out);

    // bottom half-ring
    Out=ModelSupport::getComposite(SMap, SI, onionIndex, " (7 -8 -10 9 3: -9 3 -4) ");
    System.addCell(MonteCarlo::Qhull(cellIndex++, wallMat, wallTemp, Out+topBottomStr));
    addOuterUnionSurf(Out);
    wallExclude.addUnion(Out);

    SI += 10;
  }

  wallExclude.makeComplement();
  InnerObj->addSurfString(wallExclude.display());

  return; 

}

void OnionCooling::addToInsertChain(attachSystem::ContainedComp& CC) const
  /*!
    Adds this object to the containedComp to be inserted.
    \param CC :: ContainedComp object to add to this
  */
{
  for(int i=onionIndex+1; i<cellIndex; i++)
    CC.addInsertCell(i);
    
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


void OnionCooling::createAll(Simulation& System, const attachSystem::FixedComp& FC)
{
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin

    In our case the reflector has to be built relative to an origin and an axes set. 
    If you take a simple fixed object, then the axes is the axes set of this fixed object and the origin is the origin of this fixed object,
    which does not mean that the reflector and the object have the same origin, that's just the way you start and then you add the next bits.
   */

  ELog::RegMethod RegA("OnionCooling","createAll");
  // the order matters:

  populate(System.getDataBase()); // populate variables
  createUnitVector(FC); // take fixed component, then apply shift and angle rotation (transformation) for this object centre
  createSurfaces();
  createObjects(System,FC);
  createLinks();
  insertObjects(System);       

  return;
}

}  // NAMESPACE instrumentSystem
