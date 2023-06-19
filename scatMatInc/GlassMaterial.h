/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   scatMatInc/GlassMaterial.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef scatterSystem_GlassMaterial_h
#define scatterSystem_GlassMaterial_h

namespace scatterSystem
{
  
  class neutron;

  /*!
    \class GlassMaterial 
    \brief Test material for refractive index
    \author S. Ansell
    \version 1.0
    \date June 2010
  */
  
class GlassMaterial : public neutMaterial
{
 private:

  double refIndex;         ///< Refractive Index

 public:
  
  GlassMaterial();
  GlassMaterial(const std::string&,const double,const double,
		const double,const double,const double,const double);
  GlassMaterial(const double,const double,const double,
		const double,const double,const double);
  GlassMaterial(const GlassMaterial&);
  GlassMaterial* clone() const override;
  GlassMaterial& operator=(const GlassMaterial&);
  ~GlassMaterial() override;  
  

  void setRefIndex(const double R) { refIndex=R; }

  /// Returns the refractive index
  double calcRefIndex(const double) const override { return refIndex; }


};


} // NAMESPACE scatterSystem

#endif
