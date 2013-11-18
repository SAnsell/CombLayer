/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monteInc/RuleBinary.h
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
#ifndef RuleBinary_h
#define RuleBinary_h 

/*!
  \class RuleBinary
  \brief Represents a rule in a binary list
  \author S. Ansell
  \version 1.0
  \date May 2006

  Holds a binary tree of the geometry object
  rule. Has simple information about the surface
  map so that extra knowledge can be obtained.
  (Decisions like px 5 > px 6 etc. )
  
  The tree can be expressed as a Essentual Prime Implicate form
*/  

class RuleBinary
{
 private:
  
  Rule* TopRule;                   ///< Rule tree that this applies to
  std::map<int,int> SurfMap;       ///< map of surfNumber -> index
  std::vector<int> SurfKeys;       ///< list of surface numbers 
  std::vector<BnId> DNFobj;        ///< DNF items (1==true,0=ether,-1=false)
  std::vector<BnId> EPI;           ///< List of Essentual PI

  void populateBase(std::map<int,int>&) const;
  void setBase(std::map<int,int>&,const BnId&) const;
  int makeEPI(std::vector<BnId>&);       ///< Calculate EPI from PI list.
  std::vector<int> commonFactor(const size_t) const;

 public:
  
  static int addBinCounter(std::vector<size_t>&,const size_t,size_t&);

  RuleBinary();
  RuleBinary(Rule*);
  RuleBinary(const RuleBinary&);
  RuleBinary& operator=(const RuleBinary&);
  ~RuleBinary();

  size_t makeSurfIndex();         ///< Creates surface index

  /// Creates list of DNF's from input implicants
  size_t fillDNFitems(const size_t,const std::vector<size_t>&);
  int createDNFitems();        ///< Creats list of DNFs.
  
  int group();                 ///< Creates groups
  Rule* createTree() const;     ///< create tree from a minimised DNF
  /// Access EPI
  const std::vector<BnId>& getEPI() const { return EPI; }

  void write(std::ostream&) const;  ///< write to a big string

};

std::ostream&
operator<<(std::ostream&,const RuleBinary&);

#endif
