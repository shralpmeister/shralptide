//
//  SDTideStation.m
//  ShralpTide
//
//  Created by Michael Parlee on 11/30/08.
//  Copyright 2009 Michael Parlee. All rights reserved.
/*
   This file is part of ShralpTide.

   ShralpTide is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   ShralpTide is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with ShralpTide.  If not, see <http://www.gnu.org/licenses/>.
*/

#import "SDTideStation.h"

@implementation SDTideStation

@synthesize name;
@synthesize units;
@synthesize distance;
@synthesize latitude;
@synthesize longitude;

-(void)dealloc {
	[name release];
	[units release];
	[distance release];
	[latitude release];
	[longitude release];
	[super dealloc];
}

@end
