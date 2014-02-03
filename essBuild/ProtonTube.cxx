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
#include "localRotate.h"
#include "masterRotate.h"
#include "Surface.h"
#include "surfIndex.h"
#include "surfRegister.h"
#include "objectRegister.h"
#include "surfEqual.h"
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
#include "SimProcess.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "ContainedComp.h"
#include "ContainedGroup.h"

#include "ProtonTube.h"

namespace essSystem
{

ProtonTube::ProtonTube(const std::string& Key) :
  attachSystem::ContainedGroup(),attachSystem::FixedComp(Key,3),
  ptIndex(ModelSupport::objectRegister::Instance().cell(Key)),
  cellIndex(ptIndex+1)
  /*!
    Constructor
  */
{}

ProtonTube::~ProtonTube()
  /*!
    Destructor
   */
{}

void
ProtonTube::populate(const FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("ProtonTube","populate");

    // Master values
  xStep=Control.EvalVar<double>(keyName+"XStep");
  yStep=Control.EvalVar<double>(keyName+"YStep");
  zStep=Control.EvalVar<double>(keyName+"ZStep");
  xyAngle=Control.EvalVar<double>(keyName+"XYangle");
  zAngle=Control.EvalVar<double>(keyName+"Zangle");

  
  nSec=Control.EvalVar<size_t>(keyName+"NSection");   
  double R,L(0.0),C,T;
  int Imat,Wmat;

  for(size_t i=0;i<nSec;i++)
    {
     R=Control.EvalVar<double>
	(StrFunc::makeString(keyName+"Radius",i+1));   
     L+=Control.EvalVar<double>
	(StrFunc::makeString(keyName+"Length",i+1));   
     C=Control.EvalVar<double>
	(StrFunc::makeString(keyName+"Zcut",i+1));   
     T=Control.EvalVar<double>
       (StrFunc::makeString(keyName+"WallThick",i+1));     
     Imat=ModelSupport::EvalMat<int>
       (Control,StrFunc::makeString(keyName+"InnerMat",i+1));   
     Wmat=ModelSupport::EvalMat<int>
       (Control,StrFunc::makeString(keyName+"WallMat",i+1));   
     
      radius.push_back(R);
      length.push_back(L);
      zCut.push_back(C);
      thick.push_back(T);
      inMat.push_back(Imat);
      wallMat.push_back(Wmat);
    }

  return;
}

void
ProtonTube::createUnitVector(const attachSystem::FixedComp& FC,
			     const long int sideIndex)
  /*!
    Create the unit vectors
    \param FC :: Fixed Component
    \param sideIndex :: link point on target [signed]
  */
{
  ELog::RegMethod RegA("ProtonTube","createUnitVector");

  attachSystem::FixedComp::createUnitVector(FC,sideIndex);

  applyAngleRotate(xyAngle,zAngle);
  applyShift(xStep,yStep,zStep);

  return;
}

void
ProtonTube::createSurfaces()
  /*!
    Create all the surface
  */
{
  ELog::RegMethod RegA("ProtonTube","createSurfaces");
 

  int PT(ptIndex);
  for(size_t i=0;i<nSec;i++)
    {
     ModelSupport::buildCylinder(SMap,PT+7,Origin,Y,radius[i]);  
     ModelSupport::buildCylinder(SMap,PT+17,Origin,Y,radius[i]+thick[i]); 

     ModelSupport::buildPlane(SMap,PT+2,Origin+Y*length[i],Y);  
     if (zCut[i]>0.0)
       {
	 ModelSupport::buildPlane(SMap,PT+5,Origin-Z*(radius[i]-zCut[i]),Z);
	 ModelSupport::buildPlane(SMap,PT+6,Origin+Z*(radius[i]-zCut[i]),Z);  
       }
     PT+=100;
    }
  return; 
}

void
ProtonTube::createObjects(Simulation& System, 
			  const std::string& TargetSurfBoundary,
			  const std::string& outerSurfBoundary)
  /*!
    Adds the components
    \param System :: Simulation to create objects in
    \param TargetSurfBoundary :: boundary layer [expect to be target edge]
    \param outerSurfBoundary :: boundary layer [expect to be reflector/Bulk edge]
  */
{
  ELog::RegMethod RegA("ProtonTube","createObjects");

  std::string Out,EndCap,FrontCap;
  int PT(ptIndex);
  attachSystem::ContainedGroup::addKey("Full");
  for(size_t i=0;i<nSec;i++)
    {
      const std::string SName=StrFunc::makeString("Sector",i);
      FrontCap=(!i) ? TargetSurfBoundary : ModelSupport::getComposite(SMap,PT-100, " 2 ");
      EndCap=(i+1 == nSec) ? outerSurfBoundary : ModelSupport::getComposite(SMap,PT, " -2 ");
      Out=ModelSupport::getSetComposite(SMap,PT, " -7 5 -6 ");
      System.addCell(MonteCarlo::Qhull(cellIndex++,inMat[i],0.0,Out+FrontCap+EndCap));
      Out=ModelSupport::getSetComposite(SMap,PT, " 7 -17 5 -6");
      System.addCell(MonteCarlo::Qhull(cellIndex++,wallMat[i],0.0,Out+FrontCap+EndCap));

      Out=ModelSupport::getSetComposite(SMap,PT, " -17 5 -6 ");
      attachSystem::ContainedGroup::addKey(SName);
      addOuterSurf(SName,Out+EndCap+FrontCap);
      addOuterUnionSurf("Full",Out+EndCap+FrontCap);

      PT+=100;
    }
  return;
}


void
ProtonTube::createLinks()
  /*!
    Creates a full attachment set
  */
{ 
  ELog::RegMethod RegA("ProtonTube","createLinks");
  
  FixedComp::setNConnect(nSec+2);
  int PT(ptIndex);
  for(size_t i=0;i<nSec;i++)
    {
      FixedComp::setConnect(i+2,Origin+Y*length[i]/2.0,-X);
      FixedComp::setLinkSurf(i+2,-SMap.realSurf(PT+7));
      PT+=100;
    } 
  return;
}

void
ProtonTube::createAll(Simulation& System,
		      const attachSystem::FixedComp& TargetFC,
		      const long int tIndex,
		      const attachSystem::FixedComp& BulkFC,
		      const long int bIndex)
  /*!
    Global creation of the hutch
    \param System :: Simulation to add vessel to
    \param TargetFC :: FixedComp for origin and target outer surf
    \param tIndex :: Target plate surface [signed]
    \param BulkFC :: FixedComp for origin and target outer surf
    \param bIndex :: Target plate surface [signed]
  */
{
  ELog::RegMethod RegA("ProtonTube","createAll");
  //  populate(System);
  populate(System.getDataBase());

  createUnitVector(TargetFC,tIndex);
  createSurfaces();
  std::string TSurf,BSurf;
  
  if (tIndex)
    {
      TSurf=(tIndex>0) ? 
	TargetFC.getLinkString(static_cast<size_t>(tIndex-1)) : 
	TargetFC.getBridgeComplement(static_cast<size_t>(-(tIndex+1)));
      if (tIndex<0)
	FixedComp::setLinkComponent(0,TargetFC,
				    static_cast<size_t>(-(tIndex-1)));
      else
	FixedComp::setLinkComponent(0,TargetFC,static_cast<size_t>(tIndex-1));	
    }
  if (bIndex)
    {
      const size_t lIndex(static_cast<size_t>(abs(bIndex))-1);
      BSurf=(bIndex>0) ?
	BulkFC.getLinkString(lIndex) : BulkFC.getBridgeComplement(lIndex) ;
      FixedComp::setLinkComponent(0,BulkFC,lIndex);
    }
  createObjects(System,TSurf,BSurf);
  createLinks();
  insertObjects(System); 
  return;
}

}  // NAMESPACE instrumentSystem
