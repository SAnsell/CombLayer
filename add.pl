#!/usr/bin/perl -w
use strict;
my $text=
'*
 * Copyright (c) 2004-2014 by Stuart Ansell
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
 ****************************************************************************/';



while(@ARGV)
  {
    my $file=shift @ARGV;

    open(OX,'>',"tmp");
    print OX "\/********************************************************************* \n"; 
    print OX "  CombLayer : MNCPX Input builder\n"; 
    print OX " \n"; 
    print OX " * File:   ",$file,"\n";
    print OX $text,"\n";

    my $DX;
    my $active=0;
    if (open($DX,'<',$file))
      {
	  my $line;
	  while(!$active && defined($line=<$DX>))
	    {
	      $active=1 if ($line=~/Copyright \(c\)/);
	      print OX $line;
	    }
	  close (OX);
      }

    if (!$active)
      {
	system("cp tmp ".$file);
      }
  }
