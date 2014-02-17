/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   endfInc/ENDFreaction.h
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
#ifndef ENDF_ENDFreaction_h
#define ENDF_ENDFreaction_h

namespace ENDF
{

  /*!
    \class ENDFreaction
    \brief Neutronic information from ENDF chapt 7.4
    \author S. Ansell
    \version 1.0
    \date January 2010
    
    This can be extended so that more sophisticated material
    components can be used.
    \todo This class needs to have a base class.
  */
  
class ENDFreaction 
{
 private:

  int mat;            ///< Mat number
  int mf;             ///< MF card number
  int mt;             ///< MT card number
  int tmpIndex;       ///< Temperature index
  double tempActual;  ///< Real temperature
  
  int ZA;             ///< Zaid number 
  double AWR;         ///< Atomic Weight Ratio  
  int LAT;            ///< [:0 actual / :1 Temp=300K]
  int LLN;            ///< Log Flag (0: normal, 1: log)

  std::vector<double> XData;   ///< Y Data 
  std::vector<double> YData;   ///< X Data
  
  void procZaid(std::string&);

 public:
  
  ENDFreaction();
  ENDFreaction(const std::string&,const int,const int,const int);
  ENDFreaction(const ENDFreaction&);
  virtual ENDFreaction* clone() const;
  ENDFreaction& operator=(const ENDFreaction&);
  virtual ~ENDFreaction();
  
  /// Effective typeid
  virtual std::string className() const { return "ENDFreaction"; }

  int ENDFfile(const std::string&,const int,const int,const int);
  void exportWork(const std::vector<double>&,
		  std::vector<DError::doubleErr>&) const;
  void write(std::ostream&) const;
};


} // NAMESPACE ENDF

#endif
