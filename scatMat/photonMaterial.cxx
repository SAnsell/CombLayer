/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   scatMat/photonMaterial.cxx
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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cmath>
#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <random>

#include "FileReport.h"
#include "NameStack.h"
#include "RegMethod.h"
#include "OutputLog.h"
#include "RefCon.h"
#include "Vec3D.h"
#include "Random.h"
#include "particle.h"
#include "neutron.h"
#include "Zaid.h"
#include "MXcards.h"
#include "Material.h"
#include "photonMaterial.h"

namespace scatterSystem
{

photonMaterial::photonMaterial() :
  MonteCarlo::Material(), 
  E0(0.0),Amass(1.0),sigmaTotal(0.0),
  sigmaElastic(0.0),sigmaComp(0.0),sigmaAbs(0.0)
  /*!
    Constructor
  */
{}

  
photonMaterial::photonMaterial(const photonMaterial& A) :
  MonteCarlo::Material(), 
  E0(A.E0),Amass(A.Amass),sigmaTotal(A.sigmaTotal),
  sigmaElastic(A.sigmaElastic),sigmaComp(A.sigmaComp),
  sigmaAbs(A.sigmaAbs)
  /*!
    Constructor
  */
{}

photonMaterial&
photonMaterial::operator=(const photonMaterial& A) 
  /*!
    Assignment operator 
    \param A :: photonMaterial to copy
    \return *this
  */
{
  if (this!=&A)
    {
      Material::operator=(A);
      E0=A.E0;
      Amass=A.Amass;
      sigmaTotal=A.sigmaTotal;
      sigmaElastic=A.sigmaElastic;
      sigmaComp=A.sigmaComp;
      sigmaAbs=A.sigmaAbs;
    }
  return *this;
}

photonMaterial*
photonMaterial::clone() const
  /*!
    Clone method
    \return new (this)
   */
{
  return new photonMaterial(*this);
}

photonMaterial::~photonMaterial() 
  /*!
    Destructor
   */
{}

  

void
photonMaterial::write(std::ostream& OX) const 
  /*!
    Write out the material information for MCNPX
    \param OX :: Output stream
  */
{
  OX<<"c PhotonMat:"<<std::endl;
    
  Material::write(OX);
  return;
}

} // NAMESPACE MonteCarlo

