/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   tallyInc/Tally.h
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
#ifndef tallySystem_Tally_h
#define tallySystem_Tally_h

namespace tallySystem
{

/*!
  \class Tally
  \version 1.0
  \author S. Ansell
  \date June 2006
  \brief Holds a tally object as a base class
  
*/

class Tally
{
 private:
  
  int active;                            ///< Is the tally active
  int rotRNumFlag;                       ///< Rotation flag to be applied

  std::vector<char> printField;          ///< Print field (fq card)
  NRange Etab;                           ///< Energy grid (e card)
  NRange cosTab;                         ///< Cos table (c card)
  NRange timeTab;                        ///< time table (t card)
  NRange fModify;                        ///< fm: Modification card
  std::string Comment;                   ///< comment field (fc card)
  std::string Treatment;                 ///< Special treatment 
  
 protected:
  
  int IDnum;                             ///< ID number 
  std::vector<std::string> Particles;    ///< Particle list ( : list) 
  NList<double> SDfield;                 ///< sd card [scalar division]
  
  /// Access energy
  const NRange& getEnergy() const { return Etab; } 
  void writeParticles(std::ostream&) const;
  void writeFields(std::ostream&) const;
  int processParticles(std::string&);
  
 public:
  
  explicit Tally(const int);
  Tally(const Tally&);
  Tally& operator=(const Tally&);
  virtual Tally* clone() const; 
  virtual ~Tally();

  /// ClassName
  virtual std::string className() const 
      { return "BaseTally"; }

  static int firstLine(const std::string&);
  static int getID(const std::string&);
  static std::pair<std::string,int> getElm(const std::string&);
  
  /// Set active value
  void setActive(const int Flag) { active=(Flag) ? 1 : 0; }
  /// Check is active
  int isActive() const { return active; }
  
  int setTime(const std::string&);
  void cutEnergy(const double);
  int setAngle(const std::string&);
  int setEnergy(const std::string&);
  int setModify(const std::string&);
  int setFormat(const std::string&);
  int setSpecial(const std::string&);
  /// Set the coment
  int setComment(const std::string&);
  int setPrintField(std::string);
  void setCinderEnergy(const std::string&); 
  virtual int setSDField(const double);
  virtual int setSDField(const std::string&);
  /// Combine two tallies if possible [no-op default]
  virtual int mergeTally(const Tally&)
  { return 0; }
  
  /// set tally ID
  void setKey(const int K) { IDnum=K; }
  /// Return ID number      
  int getKey() const { return IDnum; }   
  /// Get comment
  const std::string& getComment() const {return Comment; }

  virtual void rotateMaster() { }          ///< Rotation to Master
  virtual int addLine(const std::string&);     
  /// Renumber [not normally required]
  virtual void renumberCell(const int,const int) {}
  /// Renumber [not normally required]
  virtual void renumberSurf(const int,const int) {}
  /// make a group sum into single units
  virtual int makeSingle() { return 0; }

  virtual void write(std::ostream&) const;
  
  void setParticles(const std::string&);
  
  std::string getParticles() const; 
};

std::ostream&
operator<<(std::ostream&,const Tally&);
  
}  // NAMESPACE tallySystem

#endif
