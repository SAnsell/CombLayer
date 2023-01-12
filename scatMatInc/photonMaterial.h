/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   scatMatInc/photonMaterial.h
 *
 * Copyright (c) 2004-2023 by Stuart Ansell
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
#ifndef scatterSystem_photonMaterial_h
#define scatterSystem_photonMaterial_h

namespace MonteCarlo
{
  class Material;
  class MXcards;
  class Zaid;
  class neutron;
}

namespace scatterSystem
{
  /*!
    \class photonMaterial
    \brief Neutronic information on the material
    \author S. Ansell
    \version 1.0
    \date July 2007
    
    This can be extended so that more sophisticated material
    components can be used.
    \todo This class needs to have a base class.
  */
  
class photonMaterial :
    public MonteCarlo::Material
{
 private:

  double E0;             ///< Main elastic energy [for speed]
  
  double Amass;          ///< Mean Atomic Mass

  double sigmaTotal;       ///< total at E0 (barns)
  double sigmaElastic;     ///< scat. total at E0 (barns)
  double sigmaComp;        ///< compton total at E0
  double sigmaAbs;         ///< abs at E0a
  
 public:
  
  photonMaterial();
  photonMaterial(const double,MonteCarlo::Material*);
  photonMaterial(const photonMaterial&);
  photonMaterial& operator=(const photonMaterial&);
  virtual ~photonMaterial();
  
  virtual void write(std::ostream&) const;

};


} // NAMESPACE MonteCarlo

#endif
