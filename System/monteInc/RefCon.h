/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/RefCon.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef RefCon_h
#define RefCon_h

/*!
  \namespace RefCon
  \brief Physics constants for Reflectivity
  \author S. Ansell
  \version 1.0
  \date September 2005
*/

namespace RefCon
{
  const double c2(8.987551787e16);            ///< Speed of light^2 [m^2sec^4]
  const double e2(2.567010233e-38);           ///< electron charge^2  
  const double me(9.109534e-31);              ///< Mass of electron (kg)
  const double hc_e(12.39852066);             ///< convertion in keV to angstrom (wavelength)
  const double reyb(13.60569172);             ///< Reyberg Constant
  const double pi(3.14159265359);             ///< Pi
  const double re(2.817940285e-15);           ///< electron Radius
  const double avogadro(0.60221413);          ///< Avogadro in unis of gram/anstrom
  const double kevExafs(16.200878062623);     ///< Conversion E(KeV) to k [angstrom]
  const double hbar_mn(6.2962236371273e-8);   ///< mass of hbar/neutronmass [m^2/sec] 
  const double h_mn(3.9560339847515e-7) ;     ///< mass of h/neutronmass [m^2/sec] 
  const double h2_mneV(1.63608402829e-21) ;   ///<  Energy / lambda (neut) conversion [eV]
  const double hplank(6.62606876e-34);        ///< Hplank [Js]
  
  const double k_bmev(0.086173422);           ///< K boltzman [meV]
  const double k_bev(8.6173422964141e-5);        ///< K boltzman [eV]
  const double k_boltz(1.3806503e-23);           ///< K boltzman [J]
  
}

#endif
