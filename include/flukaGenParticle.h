/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/flukaGenParticle.h
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
#ifndef flukaGenParticle_h
#define flukaGenParticle_h

/*!
  \class flukaGenParticle
  \version 1.0
  \author S. Ansell
  \date October 2017
  \brief Singleton for particle name change
*/

class flukaGenParticle
{
 private:

  std::map<std::string,int> flukaGen;            ///< General fluka particle

  flukaGenParticle();  
  flukaGenParticle(const flukaGenParticle&);
  flukaGenParticle& operator=(const flukaGenParticle&);
  
 public:

  static const flukaGenParticle& Instance();

  bool hasName(const std::string&) const;
  int flukaITYP(const std::string&) const;

  const std::string& nameToFLUKA(const std::string&) const;
  double momentumFromKE(const std::string&,const double) const; 

};
 


#endif 
