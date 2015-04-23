/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   endfInc/ENDFmaterial.h
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
#ifndef ENDF_ENDFmaterial_h
#define ENDF_ENDFmaterial_h

namespace ENDF
{

  /*!
    \class ENDFmaterial
    \brief Neutronic information from ENDF chapt 7.4
    \author S. Ansell
    \version 1.0
    \date January 2010
    
    This can be extended so that more sophisticated material
    components can be used.
    \todo This class needs to have a base class.
  */
  
class ENDFmaterial 
{
 private:

  int mat;            ///< Mat number
  size_t tmpIndex;       ///< Temperature index
  double tempActual;  ///< Real temperature
  
  int ZA;             ///< Zaid number 
  double AWR;         ///< Atomic Weight Ratio  
  int LAT;            ///< [:0 actual / :1 Temp=300K]
  int LASYM;          ///< Symmetric 
  int LLN;            ///< Log Flag (0: normal, 1: log)
  int NS;             ///< Number of Non-principle scatterer
  int NI;             ///< Number of items in B(N) [NI=6*(NS+1)]
  int NT;             ///< Total temperatures
  
  SQWtable Sn;                   ///< Table of object
  SEtable SE;                    ///< Energy table
  std::vector<double> Teff;      ///< Effective temperatures for NP-atoms
  std::vector<double> B;         ///< B points
  
  void procZaid(std::string&);
  void procB(std::istream&);
  void procBeta(std::istream&);
  void procAlpha(std::istream&);
  void procTeff(std::istream&);
  void populateSETable();

 public:
  
  ENDFmaterial();
  ENDFmaterial(const std::string&);
  ENDFmaterial(const ENDFmaterial&);
  virtual ENDFmaterial* clone() const;
  ENDFmaterial& operator=(const ENDFmaterial&);
  virtual ~ENDFmaterial();
  
  /// Effective typeid
  virtual std::string className() const { return "ENDFmaterial"; }

  int inRange(const double&,const double&) const;

  int ENDF7file(const std::string&);
  double Sab(const size_t,const double,const double,const double) const;  
  double dSdOdE(const double,const double,const double) const;
  double sigma(const double) const;
  void write(std::ostream&) const;
};


} // NAMESPACE ENDF

#endif
