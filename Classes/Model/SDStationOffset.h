//
//  SDStationOffset.h
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
#import <Foundation/Foundation.h>

@interface SDStationOffset : NSObject {
	NSString *referenceStationName;
	int highTideMinutesOffset;
	int lowTideMinutesOffset;
	float highTideHeightCorrection;
	float lowTideHeightCorrection;
}

-(id)initWithStation:(NSString *)refStationName deltaHighMinutes: (int)deltaHigh deltaLow: (int)deltaLow highCorrection: (float) hiCorrection lowCorrection: (float) loCorrection;
-(NSString*)shortRefStationName;

@property (retain,readwrite) NSString *referenceStationName;
@property (readwrite) int highTideMinutesOffset, lowTideMinutesOffset;
@property (readwrite) float highTideHeightCorrection, lowTideHeightCorrection;

@end
