/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/sswTally.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef tallySystem_sswTally_h
#define tallySystem_sswTally_h

namespace tallySystem
{
/*!
  \class sswTally
  \version 1.0
  \date October 2016
  \author S. Ansell
  \brief Hold a ssw tally [single item]
*/
class sswTally : public Tally
{
 private:

  std::vector<int> surfList;               ///< List of surfaces
  
 public:
  
  explicit sswTally(const int);
  sswTally(const sswTally&);
  sswTally& operator=(const sswTally&);
  virtual ~sswTally();
  virtual sswTally* clone() const; 
    
  /// ClassName
  virtual std::string className() const 
      { return "sswTally"; }

  void addSurfaces(const std::vector<int>&);
  virtual void renumberSurf(const int,const int);

  virtual void write(std::ostream&) const;
};


}  // NAMESPACE tallySystem

#endif
