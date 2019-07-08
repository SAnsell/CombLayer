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
#include "varList.h"
#include "Code.h"
#include "FuncDataBase.h"
#include "HeadRule.h"
#include "Object.h"
#include "groupRange.h"
#include "objectGroups.h"
#include "Simulation.h"
#include "ModelSupport.h"
#include "MaterialSupport.h"
#include "generateSurf.h"
#include "support.h"
#include "stringCombine.h"
#include "LinkUnit.h"
#include "FixedComp.h"
#include "FixedOffset.h"
#include "ContainedComp.h"
#include "F5Calc.h"
#include "F5Collimator.h"

namespace essSystem
{

F5Collimator::F5Collimator(const std::string& Key) :
  attachSystem::ContainedComp(),
  attachSystem::FixedOffset(Key,6)
  /*!
     Constructor
     \param Key :: Name of construction key
  */
{}


F5Collimator::~F5Collimator()
/*!
  Destructor
*/
{}

void
F5Collimator::populate(FuncDataBase& Control)
  /*!
    Populate all the variables
    \param Control :: Variable table to use
  */
{
  ELog::RegMethod RegA("F5Collimator","populate");
  FixedOffset::populate(Control);
  
  length=Control.EvalVar<double>(keyName+"Length"); // along x
  wall=Control.EvalDefVar<double>(keyName+"WallThick", 0.5);
  
  GluePoint = Control.EvalDefVar<int>(keyName+"GluePoint", -1);
  
  Geometry::Vec3D gC,gB,gB2;
  
  if (GluePoint>=0)
    {
      std::ifstream essdat; // currently used by collimators
      essdat.open(".ess.dat", std::ios::in);
      double F[12], L[13];
      while (!essdat.eof())
	{
	  std::string str;
	  std::getline(essdat, str);
	  std::stringstream ss(str);
	  std::string header; // F: or L: point title
	  ss >> header;
	  int i=0;
	  if (header == "F:")
	    while(ss >> F[i]) i++;
	  else if (header == "L:")
	    while(ss >> L[i]) i++;
	  
	  int gpshift = GluePoint*3;
	  if (F[2]>0)   // top moderator;
	    { 
	      Control.setVariable<double>(keyName+"XB", F[gpshift+0]);
	      Control.setVariable<double>(keyName+"YB", F[gpshift+1]);
	      Control.setVariable<double>(keyName+"ZB", F[gpshift+2]);
	      
	      Control.setVariable<double>(keyName+"XC", L[gpshift+0]);
	      Control.setVariable<double>(keyName+"YC", L[gpshift+1]);
	      Control.setVariable<double>(keyName+"ZC", L[gpshift+2]);
	      
	      Control.setVariable<double>(keyName+"ZG", L[12]);
	    }
	} 
    essdat.close();
  }
  
  gB[0]=Control.EvalDefVar<double>(keyName+"XB", 0);
  gB[1]=Control.EvalDefVar<double>(keyName+"YB", 0);
  gB[2]=Control.EvalDefVar<double>(keyName+"ZB", 0);
  
  gC[0]=Control.EvalDefVar<double>(keyName+"XC", 0);
  gC[1]=Control.EvalDefVar<double>(keyName+"YC", 0);
  gC[2]=Control.EvalDefVar<double>(keyName+"ZC", 0);

  gB2[0] = gB[0];
  gB2[1] = gB[1];
  gB2[2] = Control.EvalDefVar<double>(keyName+"ZG", 0);
  
  
  SetTally(xStep, yStep, zStep);
  SetPoints(gB, gC, gB2);
  SetLength(length);
  xyAngle = GetXYAngle();
  zAngle  = GetZAngle();
  width = 2*GetHalfSizeX();
  height = 2*GetHalfSizeZ();
  
  return;
}


void
F5Collimator::createSurfaces()
  /*!
    Create Surfaces for the F5 collimator
  */
{
  ELog::RegMethod RegA("F5Collimator","createSurfaces");
  
  ModelSupport::buildPlane(SMap,buildIndex+1, Origin-X*1.0, X);
  ModelSupport::buildPlane(SMap,buildIndex+11, Origin-X*(1.0+wall), X);
  ModelSupport::buildPlane(SMap,buildIndex+2, Origin+X*length, X);
  
  ModelSupport::buildPlane(SMap,buildIndex+3, Origin-Y*(width/2.0), Y);
  ModelSupport::buildPlane(SMap,buildIndex+13, Origin-Y*(width/2.0+wall), Y);
  ModelSupport::buildPlane(SMap,buildIndex+4, Origin+Y*(width/2.0), Y);
  ModelSupport::buildPlane(SMap,buildIndex+14, Origin+Y*(width/2.0+wall), Y);
  
  ModelSupport::buildPlane(SMap,buildIndex+5, Origin-Z*(height/2.0), Z); 
  ModelSupport::buildPlane(SMap,buildIndex+15, Origin-Z*(height/2.0+wall), Z);
  ModelSupport::buildPlane(SMap,buildIndex+6, Origin+Z*(height/2.0), Z);
  ModelSupport::buildPlane(SMap,buildIndex+16, Origin+Z*(height/2.0+wall), Z);
  
  return; 
}
  
void
F5Collimator::addToInsertChain(attachSystem::ContainedComp& CC) const
  /*!
    Adds this object to the containedComp to be inserted.
    \param CC :: ContainedComp object to add to this
  */
{
  for(int i=buildIndex+1;i<cellIndex;i++)
    CC.addInsertCell(i);
  
  return;
}


void
F5Collimator::createObjects(Simulation& System)
  /*!
    Create the F5 collimator
    \param System :: Simulation to add results
  */
{
  ELog::RegMethod RegA("F5Collimator","createObjects");
  
  std::string Out;
  
  const int voidMat = 0;
  
  Out=ModelSupport::getComposite(SMap,buildIndex, " 11 -2 13 -14 15 -16");
  addOuterSurf(Out);
  
  // Internal region
  Out=ModelSupport::getComposite(SMap,buildIndex," 1 -2 3 -4 5 -6");
  System.addCell(MonteCarlo::Object(cellIndex++,voidMat,0.0, Out));
  
  // Wall
  Out=ModelSupport::getComposite(SMap, buildIndex,
				 " (11 -2 13 -14 15 -16) (-1:2:-3:4:-5:6) ");
  MonteCarlo::Object c = MonteCarlo::Object(cellIndex++,voidMat,0.0,Out);
  c.setImp(0);
  System.addCell(c);
  
  return; 
}

void F5Collimator::createLinks()
/*!
  Creates a full attachment set
  Links/directions going outwards true.
*/
{
  ELog::RegMethod RegA("F5Collimator","createLinks");
  
  FixedComp::setConnect(0,   Origin-X*(1.0+wall), -X);
  FixedComp::setLinkSurf(0, -SMap.realSurf(buildIndex+1));

  FixedComp::setConnect(1,   Origin+X*length,  X);
  
  FixedComp::setConnect(2,   Origin-Y*(width/2+wall), -Y);
  FixedComp::setConnect(3,   Origin+Y*(width/2+wall),  Y);
  
  FixedComp::setConnect(4,   Origin-Z*(height/2+wall), -Z); 
  FixedComp::setConnect(5,   Origin+Z*(height/2+wall),  Z); 
  
  for (size_t i=0; i<6; i++)
    FixedComp::setLinkSurf(i,SMap.realSurf(buildIndex+static_cast<int>(i)));
  
  return;
}
  

void
F5Collimator::createAll(Simulation& System,
			const attachSystem::FixedComp& FC,
			const long int sideIndex)
  /*!
    Extrenal build everything
    \param System :: Simulation
    \param FC :: FixedComponent for origin
    \param sideIndex :: side point
  */
{
  ELog::RegMethod RegA("F5Collimator","createAll");
  populate(System.getDataBase());
  
  createUnitVector(FC,sideIndex);
  createSurfaces();
  createObjects(System);
  createLinks();
  insertObjects(System);       
  
  return;
}
  
}  // namespace essSystem
