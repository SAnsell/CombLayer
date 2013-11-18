/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   tallyInc/textTally.h
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
#ifndef textTally_h
#define textTally_h

namespace tallySystem
{
  /*!
    \class textTally
    \version 1.0
    \date July 2006
    \author S. Ansell
    \brief Hold a surface tally

   */
class textTally : public Tally
  {
  private:
    
    /// List of lines
    std::vector<std::string> Lines;
    
  public:
    
    explicit textTally(const int);
    textTally(const textTally&);
    virtual textTally* clone() const; 
    textTally& operator=(const textTally&);
    virtual ~textTally();
    
    virtual int addLine(const std::string&); 
    /// Accessor to lines
    const std::vector<std::string>& getLines() const 
      { return Lines; }
    virtual void renumberCell(const int,const int);
    virtual void renumberSurf(const int,const int);
    
    virtual void write(std::ostream&) const;      
  };

};

#endif
