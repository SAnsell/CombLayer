/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   transport/ObjComponent.cxx
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <complex>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <stdexcept> 
#include <boost/test/floating_point_comparison.hpp>

#include "MersenneTwister.h"
#include "Exception.h"
#include "FileReport.h"
#include "GTKreport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "BaseVisit.h"
#include "BaseModVisit.h"
#include "support.h"
#include "mathSupport.h"
#include "MatrixBase.h"
#include "Matrix.h"
#include "Vec3D.h"
#include "Quaternion.h"
#include "Track.h"
#include "Line.h"
#include "LineIntersectVisit.h"
#include "Surface.h"
#include "surfIndex.h"
#include "Rules.h"
#include "neutron.h"
#include "HeadRule.h"
#include "Object.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "neutMaterial.h"
#include "DBNeutMaterial.h"
#include "ObjComponent.h"

extern MTRand RNG;

namespace Transport
{

const scatterSystem::neutMaterial*
ObjComponent::neutMat(const int matN) 
  /*!
    Static function to get the neutMaterial based on the MCNPX
    material number
    \param matN :: Material number
  */
{
  ELog::RegMethod RegA("ObjComponent","neutMat");
  
  return scatterSystem::DBNeutMaterial::Instance().getMat(matN);
}

std::ostream&
operator<<(std::ostream& OX,const ObjComponent& A)
/*!
  Standard Output stream
  \param OX :: Output stream
  \param A :: Component to write
  \return OX
 */

{
  A.write(OX);
  return OX;
}

ObjComponent::ObjComponent(const MonteCarlo::Object* OP) : 
  ObjPtr(OP),MatPtr(neutMat(OP->getMat()))
  /*!
   Constructor
 */
{}


ObjComponent::~ObjComponent()
/*!
  Destructor
*/
{}



int
ObjComponent::hasIntercept(const MonteCarlo::neutron& N) const
  /*!
    Given a line IP + lambda(UV) does it intercept
    this object: (used for virtual objects)

    \param N :: Initial neutron
    \return True(1)  / Fail(0)
  */
{
  return ObjPtr->hasIntercept(N.Pos,N.uVec);
}

void
ObjComponent::attenuate(const double D,MonteCarlo::neutron& N) const
  /*!
    Attenuate the Neutron in the material over the 
    distance					
    \param D :: Distance to travel
    \param N :: Neutron point
  */
{
  if (!MatPtr) return;
  N.weight*=MatPtr->calcAtten(N.wavelength,D);
  return;
}  


double 
ObjComponent::getRefractive(const MonteCarlo::neutron& N) const
  /*!
    Get the real refractive index
    \param N :: Neutron
    \return  n : the refractive index 
  */
{ 
  return (MatPtr) ? MatPtr->calcRefIndex(N.wavelength) : 1.0; 
}

double 
ObjComponent::getRefractive(const double W) const
  /*!
    Get the real refractive index
    \param W :: Wavelength [Angstrom]
    \return n : the refractive index 
  */
{ 
  return (MatPtr) ? MatPtr->calcRefIndex(W) : 1.0; 
}


void
ObjComponent::scatterNeutron(MonteCarlo::neutron& N) const
  /*!
    Scatter the neutron into a new direction + energy
    Random scatter.
    \param N :: Neutron to scatter
  */
{
  if (MatPtr)
    MatPtr->scatterNeutron(N);
  return;
}

double
ObjComponent::ScatTotalRatio(const MonteCarlo::neutron& NIn,
			     const MonteCarlo::neutron&) const
  /*!
    Get the scattering / Total ratio
    \param NIn :: Input neutron 
    \param :: Output neutron (After scatter)
    \return new neutron weight
  */
{
  return (MatPtr) ? MatPtr->ScatTotalRatio(NIn.wavelength) : 0.0;
}

double
ObjComponent::TotalCross(const MonteCarlo::neutron& N) const
  /*!
    Get the total scatterin cross section
    \param N :: neutron to obtain wavelength
    \return sigma_total * density
   */
{
  return (MatPtr) ? MatPtr->TotalCross(N.wavelength) : 0.0;
}
  
int
ObjComponent::trackOutCell(const MonteCarlo::neutron& N,
			   double& aDist,
			   const Geometry::Surface*& surfPtr) const
  /*!
    This tracks a neutron to an object and determines the 
    minimum distance that the object has to travel from the object
    \param N :: neutron to obtain track from
    \param aDist :: primary distance
    \param surfPtr ;: surface  on exit
    \return true/false if neutron intersects object    
  */
{
  return ObjPtr->trackOutCell(N,aDist,surfPtr);
}

int
ObjComponent::trackIntoCell(const MonteCarlo::neutron& N,
			    double& aDist,
			    const Geometry::Surface*& surfPtr) const
  /*!
    This tracks a neutron to an object and determines the 
    minimum distance that the object has to travel to the object
    \param N :: neutron to obtain track from
    \param aDist :: primary distance
    \param surfPtr ;: surface  on exit
    \return true/false if neutron intersects object    
  */
{
  return ObjPtr->trackIntoCell(N,aDist,surfPtr);
}

int
ObjComponent::trackWeight(MonteCarlo::neutron& N,
			  double& R,
			  const Geometry::Surface*& surfPtr) const
  /*!
    This tracks a neutron through object and determines the 
    the track modification to a scattering point.
    \param N :: neutron to move forward:change weight:new direction
    \param R :: Random number exponent step
    \param surfPtr :: surface that track ended on
    \return surface number that it exited at /  0
  */
{
  ELog::RegMethod RegA("ObjComponent","trackWeight");
  double aDist(0);
      
  const int SN=ObjPtr->trackOutCell(N,aDist,surfPtr);
  //  ELog::EM<<"Nutron Track"<<N.weight<<ELog::endDiag;
  if (MatPtr)    // not-void
    {
      // Material to attenuate beam:
      const double sXsec=MatPtr->ScatCross(N.wavelength);
      const double aXsec=MatPtr->TotalCross(N.wavelength)-sXsec;

      const double DV= -log(R)/sXsec;
      // Neutron did not reach other size
      if (DV<aDist-Geometry::shiftTol)
	{
	  N.weight*=exp(-DV*aXsec);
	  N.moveForward(DV);
	  surfPtr=0;
	  return 0;
	}
      R=exp(-sXsec*(Geometry::shiftTol+DV-aDist));
      N.weight*=exp(-aDist*aXsec);
    }
  N.moveForward(aDist);
  // Now step over 
  Geometry::Vec3D XX(N.Pos);
  N.Pos+=surfPtr->surfaceNormal(N.Pos)*(sign(SN)*Geometry::shiftTol);
  return SN;
}

int
ObjComponent::trackAttn(MonteCarlo::neutron& N,
			const Geometry::Surface*& SPtr) const
  /*!
    This tracks a neutron through object and determines the 
    the track modification to an exit
    \param N :: neutron to move forward:change weight:new direction
    \param SPtr :: Surface Pointer
    \return surface number that it exited [Signed].
  */
{
  ELog::RegMethod RegA("ObjComponent","trackAttn");
  double aDist(0);
      
  // Signed number
  const int SN=ObjPtr->trackOutCell(N,aDist,SPtr);
  if (MatPtr)    // not-void
    {
      // Material to attenuate beam:
      const double tXsec=MatPtr->TotalCross(N.wavelength);
      N.weight*=exp(-aDist*tXsec);
    }
  // Micro extra to avoid surface boundary
  N.moveForward(aDist);
  N.Pos-=SPtr->surfaceNormal(N.Pos)*(sign(SN)*Geometry::shiftTol);

  return SN;
}


void
ObjComponent::selectEnergy(const MonteCarlo::neutron& NIn,
			   MonteCarlo::neutron& NOut) const
  /*!
    Given a incomming neutron select an outgoing energy for
    the neutron based on the cross section.
  */
{
  ELog::RegMethod RegA("ObjComponent","selectEnergy");  
  NOut=NIn;
  if (MatPtr)
    {
      // Choise between elastic and inelastic scattering:
      const double R=RNG.rand();
      const double elasticRatio=MatPtr->ElasticTotalRatio(NIn.wavelength);
      if (R<elasticRatio)
	return;      
      //     NOut.wavelength=MatPtr->incEnergy(NIn.wavelength);
    }
  return;
}
			      
void
ObjComponent::write(std::ostream& OX) const
  /*!
    Writes out to a stream
    \param OX :: output stream
  */
{
  ObjPtr->write(OX);
  return;
}

void
ObjComponent::writeMCNPX(std::ostream& OX) const
  /*!
    Write out the MCNPX component
    \param OX :: Output stream
  */
{
  OX<<"c ObjComponent "<<std::endl;
  ObjPtr->write(OX);
  return;
}

} // Namespace MonteCarlo
