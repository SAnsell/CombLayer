/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   flukaTallyInc/userRadDecay.h
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
#ifndef flukaSystem_userRadDecay_h
#define flukaSystem_userRadDecay_h

namespace flukaSystem
{
/*!
  \class userRadDecay
  \version 1.0
  \date Ferbruary 2018
  \author S. Ansell
  \brief userbin for fluka
*/

class userRadDecay : public flukaTally
{
 private:

  size_t nReplica;                  ///< Radioactive decay repead

  std::map<std::string,int> biasCard; ///< Which biases to apply

  double gammaTransCut;             ///< Transport cut off value [1.0 default]

  /// Irradiation decay times / intensity
  std::vector<std::pair<double,double>> iradTime;
  std::vector<double> decayTime;    ///< Different decay times

  /// map of detectors to use: [previous named detectors]
  std::map<std::string,size_t> detectors;
  
 public:

  userRadDecay();
  userRadDecay(const std::string&,const int);
  userRadDecay(const userRadDecay&);
  virtual userRadDecay* clone() const; 
  userRadDecay& operator=(const userRadDecay&);
  virtual ~userRadDecay();

  void setNReplica(const size_t NR) { nReplica=NR; }
  void setIradTime(const double,const std::vector<double>&);
  void setDecayTime(const std::vector<double>&);
  void addDetectors(const std::map<size_t,std::string>&);
  
  virtual void write(std::ostream&) const;  
};

}

#endif
