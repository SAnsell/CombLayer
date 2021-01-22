/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   phitsTallyInc/phitsTally.h
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
#ifndef phitsSystem_phitsTally_h
#define phitsSystem_phitsTally_h

namespace phitsSystem
{

  class eType;
  class aType;
  
/*!
  \class phitsTally
  \version 1.0
  \author S. Ansell
  \date September 2018
  \brief Holds a phits tally object as a base class
  
*/

class phitsTally
{
 protected:

  const std::string keyName;        ///< Master name
  const int idNumber;               ///< iD number
  std::string comments;             ///< comment line

  std::string particle;             ///< particle(s)

  std::string title;              ///< title
  std::string xTxt;               ///< x-text
  std::string yTxt;               ///< y-Text

  bool epsFlag;                     ///< Write an eps file
  bool vtkout;                     ///< Write a vtk file
  bool vtkBinary;                   ///< Write VTK in binary

  static std::string convertParticleType(const std::string&);
  
 public:
  
  explicit phitsTally(const int);
  phitsTally(const std::string&,const int);
  phitsTally(const phitsTally&);
  phitsTally& operator=(const phitsTally&);
  virtual phitsTally* clone() const; 
  virtual ~phitsTally();
  
  void setComment(const std::string&);

  /// no op renumber 
  virtual void renumberCell(const int,const int) {}
  
  virtual void setParticle(const std::string&);
  virtual void setEnergy(const eType&);
  virtual void setAngle(const aType&);
  virtual void setVTKout();
  virtual void setBinary();

  /// accessor to keyname
  const std::string& getKeyName() const { return keyName; }
  /// accessor to keyname
  int getID() const { return idNumber; }
  
  virtual void write(std::ostream&,const std::string&) const;

};
  
}  // NAMESPACE phitsSystem

#endif
