/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   src/SimMonte.cxx
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#include <cmath>
#include <complex> 
#include <vector>
#include <map> 
#include <list> 
#include <set>
#include <stack>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <boost/functional.hpp>
#include <boost/bind.hpp>
#include <boost/multi_array.hpp>

#include "MersenneTwister.h"
#include "Exception.h"
#include "ManagedPtr.h"
#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "GTKreport.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "XMLwriteVisitor.h"
// #include "XMLimportVisitor.h"
#include "mathSupport.h"
#include "support.h"
#include "BaseVisit.h"
#include "Element.h"
#include "MapSupport.h"
#include "MXcards.h"
#include "Material.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Triple.h"
#include "Track.h"
#include "Surface.h"
#include "Quadratic.h"
#include "ArbPoly.h"
#include "Cylinder.h"
#include "Cone.h"
#include "CylCan.h"
#include "General.h"
#include "MBrect.h"
#include "NullSurface.h"
#include "Plane.h"
#include "Sphere.h"
#include "Torus.h"
#include "Rules.h"
#include "varList.h"
#include "Code.h"
#include "FItem.h"
#include "FuncDataBase.h"
#include "surfIndex.h"
#include "HeadRule.h"
#include "Object.h"
#include "Qhull.h"
#include "ObjSurfMap.h"
// #include "Component.h"
#include "neutMaterial.h"
#include "ObjComponent.h"
// #include "CompStore.h"
// #include "ObjStore.h"
// #include "InstrumentGeom.h"
#include "surfaceFactory.h"
#include "Beam.h"
// #include "OutZone.h"
#include "neutron.h"
#include "Detector.h"
#include "DetGroup.h"
#include "NRange.h"
#include "Simulation.h"
#include "SimMonte.h"

extern MTRand RNG;

SimMonte::SimMonte() : Simulation(),
  TCount(0),B(0),DUnit()
  /*!
    Start of simulation Object
    Initialise currentSample to Sample 
  */
{}

SimMonte::SimMonte(const SimMonte& A)  :
  Simulation(A),
  TCount(A.TCount),B((A.B) ? A.B->clone() : 0),
  DUnit(A.DUnit)
  /*!
    Copy constructor:: makes a deep copy of the SurMap 
    object including calling the virtual clone on the 
    Surface pointers
    \param A :: object to copy
  */
{}


SimMonte&
SimMonte::operator=(const SimMonte& A) 
  /*!
    Assignment operator :: makes a deep copy of the SurMap 
    object including calling the virtual clone on the 
    Surface pointers
    \param A :: object to copy
    \return *this
  */
{
  if (this!=&A)
    {
      delete B;
      B=(A.B) ? A.B->clone() : 0;
      DUnit=A.DUnit;
    }
  return *this;
}

SimMonte::~SimMonte()
  /*!
    Destructor :: deletes the memory in the SurMap 
    list
  */
{
  delete B;
}

void
SimMonte::clearAll()
 /*!
   Thermonuclear distruction of the whole system
 */
{
  TCount=0;
  DUnit.clear();
  return;
}


void 
SimMonte::setBeam(const Transport::Beam& BObj) 
  /*!
    Set the beam pointer
    \param BObj :: Point to a beam implementation
  */
{
  delete B;
  B=BObj.clone();
  return;
}


void
SimMonte::setDetector(const Transport::Detector& DObj)
  /*!
    Set the detector
    \parma DetObj :: Detector object to add
  */
{
  ELog::RegMethod RegA("SimMonte","setDetector");
  DUnit.addDetector(DObj);
  return;
}
 
void
SimMonte::runMonte(const size_t Npts)
  /*!
    Run a specific number 
    \param Npts :: number of points
  */
{
  ELog::RegMethod RegA("SimMonte","runMonte");

  static MonteCarlo::Object* defObj(0);

  //  const int aim((Npts>10) ? Npts/10 : 1);
  const Geometry::Surface* surfPtr;
  MonteCarlo::neutron Nout(0,Geometry::Vec3D(0,0,0),
			   Geometry::Vec3D(1,0,0));
  const ModelSupport::ObjSurfMap* OSMPtr =getOSM();

  //  double tDist;  // Track disnace 
  //  Geometry::Surface* SPtr;  // Exit surface

  for(size_t i=0;i<Npts;i++)
    {
      try
	{
	  // No material info at this point:
	  MonteCarlo::neutron n=B->generateNeutron();
	  //      if (!DUnit.calcCell(n,testA,testB))
	  //	ELog::EM<<"Failed on hit with "<<n<<ELog::endErr;
	  
	  // Note teh double loop : 
	  //    -- A to track to scatter point [outer]
	  //    -- B to track to track length point [inner]

	  const MonteCarlo::Object* OPtr=this->findCell(n.Pos,defObj);
	  
	  while (OPtr->getImp())
	    {
	      Transport::ObjComponent Cell(OPtr);
	      double R=RNG.randExc();
	      // Calculate forward Track:
	      int surfN;
	      surfN=Cell.trackWeight(n,R,surfPtr);   
	      if (surfN)  
		OPtr=OSMPtr->findNextObject(surfN,n.Pos,
					    OPtr->getName());
	      else         // Internal scatter : Get new R
		{
		  /*
		  Cell.selectEnergy(n,Nout);		  
		  // Internal scatter : process fraction to detector
		  
		  // To sample you need : 
		  // Direction / solid angle / dsigma/domega
		  DUnit.project(n,Nout);                       // get both
		  // Object
		  Nout.weight*=CellS.catTotalRatio(n,Nout);
		  // ATTENUATE:
		  Layout.attenPath(Nout,SP.first,SP.second);
		  DUnit.addEvent(Nout);
		  // Now scattering neutron
		  SP.second->scatterNeutron(n);
		  do
		    R=RNG.randExc();
		  while(R-1.0>Geometry::shiftTol);
		  */  
		}
	    }
	}
      catch (ColErr::NumericalAbort& A)
	{
	  ELog::EM<<"Failed at point :"<<i<<ELog::endCrit;
	  ELog::EM<<"From :"<<A.what()<<ELog::endCrit;
	}
    }
  
  return;
}

void
SimMonte::writeDetectors(const std::string& outName,
			 const double lambda) const
  /*!
    Write out a summary of the SimMonte
    \param outName :: Output head name
    \param lambda :: Output wavelength bin
  */
{
  const std::string DetectorFile=outName+".det";
  std::ofstream OX;
  OX.open(DetectorFile.c_str());
  DUnit.write(OX,lambda);
  OX.close();
  return;
}
  
