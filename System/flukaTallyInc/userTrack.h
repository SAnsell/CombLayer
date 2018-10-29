/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTallyInc/userTrack.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef flukaSystem_userTrack_h
#define flukaSystem_userTrack_h

namespace flukaSystem
{
/*!
  \class userTrack
  \version 1.0
  \date Ferbruary 2018
  \author S. Ansell
  \brief userbin for fluka
*/

class userTrack : public flukaTally
{
 private:

  std::string particle;             ///< particle/type
    
  bool eLogFlag;                    ///< energy log flag
  bool aLogFlag;                    ///< angle log flag
  bool fluenceFlag;                 ///< fluence score
  bool oneDirFlag;                  ///< one direction flag

  size_t nE;                        ///< number of energy
  double energyA;                   ///< Energy start
  double energyB;                   ///< Energy end
  
  int cellA;                        ///< start cell

  int getLogType() const;
  
 public:

  explicit userTrack(const int);
  userTrack(const std::string&,const int);
  userTrack(const userTrack&);
  virtual userTrack* clone() const; 
  userTrack& operator=(const userTrack&);
  virtual ~userTrack();
  
  void setParticle(const std::string&);
  void setCell(const int);
  virtual void setEnergy(const bool,const double,
			 const double,const size_t);
  
  virtual void write(std::ostream&) const;  
};

}

#endif
