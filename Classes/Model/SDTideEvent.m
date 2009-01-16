//
//  SDTideEvent.m
//  xtidelib
//
//  Created by Michael Parlee on 7/20/08.
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

#import "SDTideEvent.h"

@implementation SDTideEvent
-(id)initWithTime:(NSDate *)time Event:(SDTideState)state andHeight:(float)height;
{
    if (![self init]) {
        return nil;
    }
    
    eventTime = [time retain];
    eventType = state;
    eventHeight = height;
    
    return self;    
}

-(NSString *)eventTypeDescription
{
    NSString* result;
    switch ([self eventType]) {
        case SDTideStateLowTide:
            result = @"Low";
            break;
        case SDTideStateHightTide:
            result = @"High";
            break;
        case SDTideStateRising:
            result = @"Rising";
            break;
        case SDTideStateFalling:
            result = @"Falling";
            break;
        case SDTideStateMaxFlood:
            result = @"Max Flood";
            break;
        case SDTideStateMaxEbb:
            result = @"Max Ebb";
            break;
        case SDTideStateSlack:
            result = @"Slack";
            break;
    }
    return result;
}

-(NSString *)eventTimeString24HR {
	NSDateFormatter *formatter = [[[NSDateFormatter alloc] init] autorelease];
	[formatter setDateFormat:@"HH:mm"];
	return [formatter stringFromDate:eventTime];
}
-(NSString *)eventTimeString12HR {
	NSDateFormatter *formatter = [[[NSDateFormatter alloc] init] autorelease];
	[formatter setDateFormat:@"HH:mm pm"];
	return [formatter stringFromDate:eventTime];	
}

-(void)dealloc {
	[eventTime release];
	[super dealloc];
}

@synthesize eventTime;
@synthesize eventType;
@synthesize eventHeight;
@end
