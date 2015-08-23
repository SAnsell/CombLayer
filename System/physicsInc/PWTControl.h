/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   physicsInc/PWTControl.h
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
#ifndef physicsSystem_PWTControl_h
#define physicsSystem_PWTControl_h

namespace physicsSystem
{
 
/*!
  \class PWTControl
  \version 2.0
  \date April 2015
  \author S.Ansell
  \brief Process Photon Weight
  
*/

class PWTControl 
{
 private:

  typedef MapSupport::Range<int> RTYPE;             ///< Range type
  typedef std::map<RTYPE,double> MTYPE;             ///< Master type
  
  /// cells : Exp card values
  std::map<MapSupport::Range<int>,double> MapItem; 

  /// maps NEW -> OLD
  std::map<int,int> renumberMap;
  
 public:
   
  PWTControl();
  PWTControl(const PWTControl&);
  PWTControl& operator=(const PWTControl&);
  virtual ~PWTControl();

  void clear();

  void setUnit(const MapSupport::Range<int>&,
	      const double);
  void setUnit(const int,const double);

  void renumberCell(const int,const int);
  
  void write(std::ostream&,const std::vector<int>&,
	     const std::set<int>&) const;
  
};

}

#endif
