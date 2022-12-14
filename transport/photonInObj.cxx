/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   transport/photonInObj.cxx
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "mathSupport.h"
#include "Vec3D.h"
#include "particle.h"
#include "photon.h"
#include "HeadRule.h"
#include "Importance.h"
#include "Object.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "photonMaterial.h"
#include "photonInObj.h"


namespace Transport
{


photonInObj::photonInObj(const MonteCarlo::Object* OP) : 
  ObjPtr(OP)
  /*!
   Constructor
 */
{}


photonInObj::~photonInObj()
/*!
  Destructor
*/
{}



int
photonInObj::hasIntercept(const MonteCarlo::particle& N) const
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
photonInObj::attenuate(const double D,
		       MonteCarlo::particle& N) const
  /*!
    Attenuate the Neutron in the material over the 
    distance					
    \param D :: Distance to travel
    \param N :: Neutron/Photon point
  */
{
  const MonteCarlo::Material* matPtr=ObjPtr->getMatPtr();
  N.weight *= matPtr->calcAtten(N,D);
  return;
}  


void
photonInObj::scatterParticle(MonteCarlo::particle& N) const
  /*!
    Scatter the neutron into a new direction + energy
    Random scatter.
    \param N :: Neutron to scatter
  */
{
  return;
}


double
photonInObj::scatTotalRatio(const MonteCarlo::particle& n0,
			    const MonteCarlo::particle& n1) const
  /*!
    Get the scattering / Total ratio
    \param n0 :: Input particle
    \param n1 :: Ouput particle
    \param :: Output photon (After scatter)
    \return new neutron weight
  */
{
  return 1.0;
}


double
photonInObj::totalXSection(const MonteCarlo::particle& n0) const
  /*!
    Get the total scattering cross section
    \param n0 :: particle to obtain wavelength
    \return sigma_total * density
  */
{
  return 0.0;
}

} // Namespace Transport
