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
#include <cstring>
#include <algorithm>
#include <boost/array.hpp>
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

#include "sbadDetector.h"

namespace sinbadSystem
{

sbadDetector::sbadDetector(const std::string& Key,const size_t ID) :
  attachSystem::ContainedComp(),
  attachSystem::FixedComp(Key+StrFunc::makeString(ID),0),
  baseName(Key),detID(ID),
  detIndex(ModelSupport::objectRegister::Instance().cell(keyName)),
  cellIndex(detIndex+1),active(0)
  /*!
    Constructor BUT ALL variable are left unpopulated.
    \param Key :: Name for item in search
  */
{}

sbadDetector*
sbadDetector::clone() const
  /*!
    Clone funciton
    \return new(this)
  */
{
  return new sbadDetector(*this);
}

sbadDetector::sbadDetector(const sbadDetector& A) : 
  attachSystem::ContainedComp(A),attachSystem::FixedComp(A),
  baseName(A.baseName),detID(A.detID),detIndex(A.detIndex),
  cellIndex(A.cellIndex),active(A.active),xStep(A.xStep),
  yStep(A.yStep),zStep(A.zStep),xyAngle(A.xyAngle),
  zAngle(A.zAngle),radius(A.radius),length(A.length),
  mat(A.mat)
  /*!
    Copy constructor
    \param A :: sbadDetector to copy
  */
{}

sbadDetector&
sbadDetector::operator=(const sbadDetector& A)
  /*!
    Assignment operator
    \param A :: sbadDetector to copy
    \return *this
  */
{
  if (this!=&A)
    {
      attachSystem::ContainedComp::operator=(A);
      attachSystem::FixedComp::operator=(A);
      cellIndex=A.cellIndex;
      active=A.active;
      xStep=A.xStep;
      yStep=A.yStep;
      zStep=A.zStep;
      xyAngle=A.xyAngle;
      zAngle=A.zAngle;
      radius=A.radius;
      length=A.length;
      mat=A.mat;
    }
  return *this;
}


sbadDetector::~sbadDetector() 
  /*!
    Destructor
  */
{}

void
sbadDetector::populate(const FuncDataBase& Control)
  /*
    Populate all the variables
    \param System :: Simulation to use
  */
{
  ELog::RegMethod RegA("sbadDetector","populate");

  active=Control.EvalVar<int>(keyName+"Active");
  xStep=Control.EvalPair<double>(keyName,baseName,"XStep");
  yStep=Control.EvalPair<double>(keyName,baseName,"YStep");
  zStep=Control.EvalPair<double>(keyName,baseName,"ZStep");
  xyAngle=Control.EvalPair<double>(keyName,baseName,"XYAngle");
  zAngle=Control.EvalPair<double>(keyName,baseName,"ZAngle");

  radius=Control.EvalPair<double>(keyName,baseName,"Radius");
  length=Control.EvalPair<double>(keyName,baseName,"Length");
  radius=Control.EvalPair<double>(keyName,baseName,"Radius");

  mat=ModelSupport::EvalMat<int>(Control,keyName+"Mat",baseName+"Mat");

  return;
}

void
sbadDetector::createUnitVector(const attachSystem::FixedComp& FC)
  /*!
    Create the unit vectors
    \param FC :: FixedComponent for origin
  */
{
  ELog::RegMethod RegA("sbadDetector","createUnitVector");

  FixedComp::createUnitVector(FC);
  applyShift(xStep,yStep,zStep);
  applyAngleRotate(xyAngle,zAngle);

  return;
}


void
sbadDetector::createSurfaces()
  /*!
    Create All the surfaces
   */
{
  ELog::RegMethod RegA("sbadDetector","createSurface");
  
  
  ModelSupport::buildPlane(SMap,detIndex+1,Origin-Y*(length/2.0),Y);
  ModelSupport::buildPlane(SMap,detIndex+2,Origin+Y*(length/2.0),Y);
  ModelSupport::buildCylinder(SMap,detIndex+7,Origin,Y,radius);

  return;
}

void
sbadDetector::createObjects(Simulation& System)
  /*!
    Adds the detector to the simulation
    \param System :: Simulation to create objects in
  */
{
  ELog::RegMethod RegA("sbadDetector","createObjects");

  std::string Out;
 
  Out=ModelSupport::getComposite(SMap,detIndex,"1 -2 -7");
  System.addCell(MonteCarlo::Qhull(cellIndex++,mat,0.0,Out)); 
  addOuterSurf(Out);
  
  return;
}

void
sbadDetector::createLinks()
  /*!
    Create all the links
  */
{

  return;
}

void
sbadDetector::createAll(Simulation& System,
			const attachSystem::FixedComp& FC)
  /*!
    Generic function to create everything
    \param System :: Simulation item
    \param FC :: Fixed Component for origin
  */
{
  ELog::RegMethod RegA("sbadDetector","createAll");

  populate(System.getDataBase());
  createUnitVector(FC);
  createSurfaces();
  createObjects(System);
  insertObjects(System);

  return;
}
  
}  // NAMESPACE sinbadSystem
