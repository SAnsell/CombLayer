/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   weightsInc/WForm.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef WeightSystem_WForm_h
#define WeightSystem_WForm_h

namespace MonteCarlo
{
  class Qhull;
}

/*!
  \namespace WeightSystem
  \brief Weigh modifications
  \version 1.0
  \author S. Ansell
  \date August 2008
*/

namespace WeightSystem
{

/*!
  \class WForm
  \brief Top level class for all weight controllers
  \version 1.0
  \date April 2010
  \author S. Ansell
*/
class WForm
{
 protected:

  int activeWWP;               ///< Controls wwp being active 
  std::string pType;           ///< Particle type
  std::vector<double> Energy;  ///< Energy windows.
  double wupn;         ///< Max weight before upsplitting * minWeight
  double wsurv;        ///< survival possiblitiy
  int maxsp;           ///< max split
  int mwhere;          ///< Check weight -1:col 0:all 1:surf
  int mtime;           ///< Flag to inditace energy(0)/time(1)

 public:
  
  WForm();
  explicit WForm(const std::string&);
  WForm(const WForm&);
  WForm& operator=(const WForm&);
  virtual ~WForm() {}  ///< Destructor
  
  bool operator==(const WForm&) const; 

  ///< Set type e.g. n,p
  void setParticle(const std::string& P) { pType=P; }  

  void setEnergy(const std::vector<double>&);
  void setParam(const double,const double,const int,
		const int,const int);

  /// Energy
  const std::vector<double>& getEnergy() const 
    { return Energy; }
  /// Get particle
  const std::string& getParticle() const { return pType; }
  /// accessor to flag
  void setActiveWWP(const int F) { activeWWP=F; }

  ///\cond ABSTRACT
  virtual const std::vector<double>& getWeights(const int) const =0;

  virtual void setWeights(const int,const size_t,const double) =0;
  virtual void setWeights(const int,const std::vector<double>&) =0;

  virtual void scaleWeights(const int,const std::vector<double>&) =0;
  virtual void scaleWeights(const int,const double&) =0;
  virtual void maskCell(const int) =0;
  virtual void populateCells(const std::map<int,MonteCarlo::Qhull*>&) =0;
  virtual void renumberCell(const int,const int) =0;
  virtual void balanceScale(const std::vector<double>&) =0;
  virtual void writePHITS(std::ostream&) const =0;
  virtual void write(std::ostream&) const =0;

  ///\endcond ABSTRACT
  
  // Nullop for non-cell
  virtual void writePHITSHead(std::ostream&) const {}
  

};

}  

#endif
