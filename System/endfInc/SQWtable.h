/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   endfInc/SQWtable.h
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
#ifndef ENDF_SQWtable_h
#define ENDF_SQWtable_h

namespace ENDF
{
  /*!						
    \struct SQWtable
    \version 1.0
    \author S. Ansell
    \date May 2010
    \brief Hold Data for a principle atoms in a scattering law
  */
struct SQWtable
{
 
  size_t nAlpha;                    ///< Number of alpha
  size_t nBeta;                     ///< Number of beta
  std::vector<double> Alpha;     ///< Alpha values
  std::vector<double> Beta;      ///< Beta values
  boost::multi_array<double,2> SAB;               ///< S(Alpha:Beta)

  std::vector<int> alphaInterp;          ///< Alpha intep type
  std::vector<int> alphaIBoundary;       ///< Alpha inter Cut point 

  std::vector<int> betaInterp;           ///< Beta intep type
  std::vector<int> betaIBoundary;        ///< Beta inter Cut point 


  int alphaType(const long int) const;
  int betaType(const long int) const;
  int isValidRangePt(const double&,const double&,long int&,long int&) const;
  
 public:
  
  SQWtable();
  SQWtable(const SQWtable&);
  SQWtable& operator=(const SQWtable&);
  ~SQWtable() {}                       ///< Destructor

  void setNAlpha(const size_t);
  void setNBeta(const size_t);
  int isValidRange(const double&,const double&) const;
  int checkAlpha(const std::vector<int>&,const std::vector<int>&,
		 const std::vector<double>&) const;
  void setData(const size_t,const std::vector<double>&,
	       const std::vector<double>&);

  double Sab(const double,const double) const;
  
};


}
#endif
