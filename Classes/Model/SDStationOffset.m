//
//  SDStationOffset.m
//  ShralpTide
//
//  Created by Michael Parlee on 2/22/09.
//  Copyright 2009 Michael Parlee. All rights reserved.
//
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

#import "SDStationOffset.h"

@implementation SDStationOffset

@synthesize referenceStationName;
@synthesize highTideMinutesOffset;
@synthesize lowTideMinutesOffset;
@synthesize highTideHeightCorrection;
@synthesize lowTideHeightCorrection;

-(id)initWithStation:(NSString *)refStationName deltaHighMinutes: (int)deltaHigh deltaLow: (int)deltaLow highCorrection: (float) hiCorrection lowCorrection: (float) loCorrection
{
	if (![super init]) {
        return nil;
    }
    
	self.referenceStationName = refStationName;
	self.highTideMinutesOffset = deltaHigh;
	self.lowTideMinutesOffset = deltaLow;
	self.highTideHeightCorrection = hiCorrection;
	self.lowTideHeightCorrection = loCorrection;
	
    return self;
}

-(NSString*)shortRefStationName {
	NSString *name = referenceStationName;
	NSArray *parts = [name componentsSeparatedByString:@","];
	return [parts objectAtIndex:0];
}

-(void)dealloc {
	[referenceStationName release];
	[super dealloc];
}

@end
