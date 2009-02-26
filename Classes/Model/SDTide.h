//
//  SDTide.h
//  xtidelib
//
//  Created by Michael Parlee on 7/16/08.
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
#import <Foundation/Foundation.h>

@interface SDTide : NSObject {
	SDTideStation *tideStation;
	NSString *unitLong;
	NSString *unitShort;
	
    NSDate *startTime;
    NSDate *stopTime;
    
    NSArray *events;
    NSArray *intervals;
}

-(id)initWithTideStation:(SDTideStation *)station StartDate: (NSDate*)start EndDate:(NSDate*)end Events:(NSArray*)tideEvents andIntervals:(NSArray*)tideIntervals;
-(NSString*)shortLocationName;
- (float)findTideForTime:(int) time;
- (int)tideDirectionForTime:(int) time;
- (CGPoint)nearestDataPointForTime:(int) minutesFromMidnight;
-(NSNumber*)nextEventIndex;

@property (readonly) NSDate *startTime;
@property (readonly) NSDate *stopTime;
@property (readonly) NSArray *events;
@property (readonly) NSArray *intervals;
@property (nonatomic,retain) SDTideStation *tideStation;
@property (nonatomic,retain) NSString *unitLong;
@property (nonatomic,retain) NSString *unitShort;
@end
