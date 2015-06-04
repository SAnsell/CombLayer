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
#include "Plane.h"
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
#include "LayerComp.h"
#include "CellMap.h"
#include "ModBase.h"
#include "H2Wing.h"
#include "MidWaterDivider.h"
#include "EdgeWater.h"
#include "ButterflyModerator.h"

namespace essSystem
{

ButterflyModerator::ButterflyModerator(const std::string& Key) :
  constructSystem::ModBase(Key,12),
  flyIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(flyIndex+1),
  LeftUnit(new H2Wing(Key,"LeftLobe",90.0)),
  RightUnit(new H2Wing(Key,"RightLobe",270.0)),
  MidWater(new MidWaterDivider(Key,"MidWater")),
  LeftWater(new EdgeWater(Key+"LeftWater")),
  RightWater(new EdgeWater(Key+"RightWater"))
  /*!
    Constructor
    \param Key :: Name of construction key
  */
{
   ModelSupport::objectRegister& OR=
    ModelSupport::objectRegister::Instance();

   OR.addObject(LeftUnit);
   OR.addObject(RightUnit);
   OR.addObject(MidWater);
   OR.addObject(LeftWater);
   OR.addObject(RightWater);
}

ButterflyModerator::ButterflyModerator(const ButterflyModerator& A) : 
  constructSystem::ModBase(A),
  flyIndex(A.flyIndex),cellIndex(A.cellIndex),
  LeftUnit(A.LeftUnit->clone()),
  RightUnit(A.RightUnit->clone()),
  MidWater(A.MidWater->clone()),
  LeftWater(A.LeftWater->clone()),
  RightWater(A.LeftWater->clone()),
  outerRadius(A.outerRadius)
  /*!
    Copy constructor
    \param A :: ButterflyModerator to copy
  */
{}

ButterflyModerator&
ButterflyModerator::operator=(const ButterflyModerator& A)
  /*!
    Assignment operator
    \param A :: ButterflyModerator to copy
    \return *this
  */
{
  if (this!=&A)
    {
      constructSystem::ModBase::operator=(A);
      cellIndex= A.cellIndex;
      *LeftUnit= *A.LeftUnit;
      *RightUnit= *A.RightUnit;
      *MidWater= *A.MidWater;
      *LeftWater= *A.LeftWater;
      *RightWater= *A.RightWater;
      outerRadius=A.outerRadius;
    }
  return *this;
}

ButterflyModerator*
ButterflyModerator::clone() const
  /*!
    virtual copy constructor
    \return new ButterflyModerator(*this)
  */
{
  return new ButterflyModerator(*this);
}

  
ButterflyModerator::~ButterflyModerator()
  /*!
    Destructor
  */
{}

void
ButterflyModerator::populate(const FuncDataBase& Control)
  /*!
    Populate the variables
    \param Control :: DataBase
   */
{
  ELog::RegMethod RegA("ButterflyModerator","populate");

  ModBase::populate(Control);
  totalHeight=Control.EvalVar<double>(keyName+"TotalHeight");
  return;
}

void
ButterflyModerator::createUnitVector(const attachSystem::FixedComp& axisFC,
				     const attachSystem::FixedComp* orgFC,
				     const long int sideIndex)
  /*!
    Create the unit vectors. This one uses axis from ther first FC
    but the origin for the second. Futher shifting the origin on the
    Z axis to the centre.
    \param axisFC :: FixedComp to get axis [origin if orgFC == 0]
    \param orgFC :: Extra origin point if required
    \param sideIndex :: link point for origin if given
  */
{
  ELog::RegMethod RegA("ButterflyModerator","createUnitVector");
  ModBase::createUnitVector(axisFC,orgFC,sideIndex);
  applyShift(0,0,totalHeight/2.0);
  
  return;
}

  
void
ButterflyModerator::createSurfaces()
  /*!
    Create/hi-jack all the surfaces
  */
{
  ELog::RegMethod RegA("ButterflyModerator","createSurface");
  
  ModelSupport::buildCylinder(SMap,flyIndex+7,Origin,Z,outerRadius);
  ModelSupport::buildPlane(SMap,flyIndex+5,Origin-Z*(totalHeight/2.0),Z);
  ModelSupport::buildPlane(SMap,flyIndex+6,Origin+Z*(totalHeight/2.0),Z);

  return;
}

void
ButterflyModerator::createObjects(Simulation& System)
    /*!
    Adds the main components
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("ButterflyModerator","createObjects");
  
  const std::string Exclude=ContainedComp::getExclude();

  std::string Out;
  Out=ModelSupport::getComposite(SMap,flyIndex," -7 5 -6 ");  
  System.addCell(MonteCarlo::Qhull(cellIndex++,0,0.0,Out+Exclude));

  clearRules();
  addOuterSurf(Out);
  
  return;
}
  

int
ButterflyModerator::getCommonSurf(const size_t) const
  /*!
    Only components have reference values
  */
{
  ELog::RegMethod RegA("ButterflyModerator","getCommonSurf");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

int
ButterflyModerator::getLayerSurf(const size_t,const size_t) const
  /*!
    Only components have reference values
  */
{
  ELog::RegMethod RegA("ButterflyModerator","getLayerSurf");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

std::string
ButterflyModerator::getLayerString(const size_t,const size_t) const
  /*!
    Only components have reference values
  */
{
  ELog::RegMethod RegA("ButterflyModerator","getLayerString");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

Geometry::Vec3D
ButterflyModerator::getSurfacePoint(const size_t,
				    const size_t) const
  /*!
    Given a side and a layer calculate the link point
    \param layerIndex :: layer, 0 is inner moderator [0-6]
    \param sideIndex :: Side [0-3] // mid sides   
    \return Surface point
  */
{
  ELog::RegMethod RegA("ButterflyModerator","getSurfacePoint");
  throw ColErr::AbsObjMethod("Not implemented yet");
}

void
ButterflyModerator::createLinks()
  /*!
    Create linkes but currently incomplete
  */
{
  ELog::RegMethod RegA("ButterflyModerator","createLinks");

  FixedComp::setConnect(0,Origin-Y*outerRadius,-Y);
  FixedComp::setConnect(1,Origin+Y*outerRadius,Y);
  FixedComp::setConnect(2,Origin-X*outerRadius,-X);
  FixedComp::setConnect(3,Origin+X*outerRadius,X);
  FixedComp::setLinkSurf(0,SMap.realSurf(flyIndex+7));
  FixedComp::setLinkSurf(1,SMap.realSurf(flyIndex+7));
  FixedComp::setLinkSurf(2,SMap.realSurf(flyIndex+7));
  FixedComp::setLinkSurf(3,SMap.realSurf(flyIndex+7));

  // copy surface top/bottom from H2Wing and Orign from center
  
  FixedComp::setLinkCopy(4,*LeftUnit,4);
  FixedComp::setLinkCopy(5,*LeftUnit,5);
  const double LowV= LU[4].getConnectPt().Z();
  const double HighV= LU[5].getConnectPt().Z();
  const Geometry::Vec3D LowPt(Origin.X(),Origin.Y(),LowV);
  const Geometry::Vec3D HighPt(Origin.X(),Origin.Y(),HighV);
  FixedComp::setConnect(4,LowPt,-Z);
  FixedComp::setConnect(5,HighPt,Z);
  
  return;
}

  
void
ButterflyModerator::createExternal()
  
{
  ELog::RegMethod RegA("ButterflyModerator","createExternal");

  addOuterUnionSurf(LeftUnit->getCompExclude());
  addOuterUnionSurf(RightUnit->getCompExclude());
  addOuterUnionSurf(MidWater->getCompExclude());
  addOuterUnionSurf(LeftWater->getCompExclude());
  addOuterUnionSurf(RightWater->getCompExclude());
  
  return;
}
  
void
ButterflyModerator::createAll(Simulation& System,
			      const attachSystem::FixedComp& axisFC,
			      const attachSystem::FixedComp* orgFC,
			      const long int sideIndex)
  /*!
    Construct the butterfly components
    \param System :: Simulation 
    \param axisFC :: FixedComp to get axis [origin if orgFC == 0]
    \param orgFC :: Extra origin point if required
    \param sideIndex :: link point for origin if given
   */
{
  ELog::RegMethod RegA("ButterflyModerator","createAll");

  populate(System.getDataBase());
  createUnitVector(axisFC,orgFC,sideIndex);
  createSurfaces();
  
  LeftUnit->createAll(System,*this);
  RightUnit->createAll(System,*this);
  MidWater->createAll(System,*this,*LeftUnit,*RightUnit);
  
  std::string CutString=LeftUnit->getSignedLinkString(2);
  const std::string Exclude=
    ModelSupport::getComposite(SMap,flyIndex," -7 5 -6 ");
  LeftWater->createAll(System,*this,CutString,Exclude);

  CutString=RightUnit->getSignedLinkString(2);
  RightWater->createAll(System,*this,CutString,Exclude);

  Origin=MidWater->getCentre();
  createExternal();  // makes intermediate 


  createObjects(System);
  createLinks();
  
  return;
}




}  // NAMESPACE essSystem
