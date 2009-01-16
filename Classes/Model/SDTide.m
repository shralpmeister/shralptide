//
//  SDTide.m
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

#import "SDTide.h"
#import "SDTideInterval.h"
#import "SDTideEvent.h"

@interface SDTide(PrivateMethods)
-(int)findPreviousInterval:(int) minutesFromMidnight;
-(int)findNearestInterval:(int) minutesFromMidnight;
@end

@implementation SDTide

-(id)initWithStartDate:(NSDate*)start EndDate:(NSDate*)end Events:(NSArray*)tideEvents andIntervals:(NSArray*)tideIntervals
{
    if (![super init]) {
        return nil;
    }
    
    NSAssert(start != nil, @"Start date must not be nil");
    NSAssert(end != nil, @"End date must not be nil");
    
    startTime = [start retain];
    stopTime = [end retain];
    intervals = [tideIntervals retain];
    events = [tideEvents retain];
    
    return self;
}

-(NSString*)shortLocationName {
	NSArray *parts = [location componentsSeparatedByString:@","];
	return [parts objectAtIndex:0];
}

- (CGPoint)nearestDataPointForTime:(int) minutesFromMidnight {
	int nearestX = [self findNearestInterval:minutesFromMidnight];
	float nearestY = [self findTideForTime:nearestX];
	return CGPointMake((float)nearestX, nearestY);
}

- (int)tideDirectionForTime:(int) time {
	if ([self findTideForTime:[self findNearestInterval:time]] > [self findTideForTime:[self findPreviousInterval: time]]) {
		return SDTideStateRising;
	} else if ([self findTideForTime:[self findNearestInterval:time]] < [self findTideForTime:[self findPreviousInterval: time]]) {
		return SDTideStateFalling;
	} else {
		return SDTideStateSlack;
	}
}

-(float)findTideForTime:(int) time {
	float height = 0.0;
	int basetime = 0;
	for (SDTideInterval *tidePoint in [self intervals]) {
		int minutesSinceMidnight = 0;
		if (basetime == 0) {
			basetime = (int)[[tidePoint time] timeIntervalSince1970];
		}
		minutesSinceMidnight = (int)([[tidePoint time] timeIntervalSince1970] - basetime) / 60;
		if (minutesSinceMidnight == time) {
			height = tidePoint.height;
			return height;
		}
	}
	return height;
}

#pragma mark PrivateMethods

-(int)findPreviousInterval:(int) minutesFromMidnight {
	return [self findNearestInterval:minutesFromMidnight] - 15;
}

-(int)findNearestInterval:(int) minutesFromMidnight {
	int numIntervals = floor(minutesFromMidnight / 15);
	int remainder = minutesFromMidnight % 15;
	if (remainder >= 8) {
		++numIntervals;
	}
	return numIntervals * 15;
}

-(void)dealloc
{
    [startTime release];
    [stopTime release];
    [intervals release];
    [events release];
	[location release];
	[unitLong release];
	[unitShort release];
    [super dealloc];
}

@synthesize startTime;
@synthesize stopTime;
@synthesize intervals;
@synthesize events;
@synthesize location;
@synthesize unitLong;
@synthesize unitShort;
@end
