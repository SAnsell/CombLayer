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
#include "ButterflyModerator.h"

namespace essSystem
{

ButterflyModerator::ButterflyModerator(const std::string& Key) :
  constructSystem::ModBase(Key,12),
  LeftUnit(new H2Wing("LeftLobe")),
  RightUnit(new H2Wing("RightLobe")),
  MidWater(new MidWaterDivider("MidWater"))
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
}

ButterflyModerator::ButterflyModerator(const ButterflyModerator& A) : 
  constructSystem::ModBase(A),
  LeftUnit(A.LeftUnit),RightUnit(A.RightUnit)
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
      LeftUnit=A.LeftUnit;
      RightUnit=A.RightUnit;
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
ButterflyModerator::createExternal()
  
{
  ELog::RegMethod RegA("ButterflyModerator","createExternal");

  addOuterUnionSurf(LeftUnit->getCompExclude());
  addOuterUnionSurf(RightUnit->getCompExclude());
  addOuterUnionSurf(MidWater->getCompExclude());
  
  return;
}
  
void
ButterflyModerator::createAll(Simulation& System,
			      const attachSystem::FixedComp& FC)
  /*!
    Construct the butterfly components
    \param System :: Simulation 
    \param FC :: FixedComp to get origin from 
   */
{
  ELog::RegMethod RegA("ButterflyModerator","createAll");

  ModBase::populate(System.getDataBase());
  ModBase::createUnitVector(FC);

  LeftUnit->createAll(System,*this);
  RightUnit->createAll(System,*this);
  MidWater->createAll(System,*this,*LeftUnit,*RightUnit);
  createExternal();
  
  
  return;
}




}  // NAMESPACE essSystem
