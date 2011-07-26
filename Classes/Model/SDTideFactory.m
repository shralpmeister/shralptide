//
//  SDTideFactory.m
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

#import "SDTideFactory.h"
#include "everythi.h"
#import "SDTideStation.h"

@interface SDTideFactory (PrivateMethods)
+(void)mapTideStation:(SDTideStation*)station fromResultSet:(FMResultSet*)rs;
@end

@implementation SDTideFactory

+(SDTide*)tideWithStart:(NSDate*)startDate End:(NSDate*)endDate andInterval:(int)interval atStation:(SDTideStation *)station
{
	NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
	[dateFormatter setDateFormat:@"yyyy:MM:dd:HH:mm"];
    char cStart[30];
    char cEnd[30];
    time_t stoptime = 0;
    
	NSString *stationName = station.name;

    //strcpy(location,[stationName cStringUsingEncoding:NSISOLatin2StringEncoding]);
	strcpy(location, [stationName cStringUsingEncoding:NSISOLatin1StringEncoding]);
    
    strcpy(hfile_name,[[[NSBundle mainBundle] 
                   pathForResource:@"harmonics" ofType:@""] 
                  cStringUsingEncoding: NSASCIIStringEncoding]);
    
    load_data ();
    if (checkyear) {
        check_epoch ();
        exit (0);
    }
    
	NSLog(@"start date = %@", startDate);
	NSLog(@"start date string = %@", [dateFormatter stringFromDate:startDate]);
	NSLog(@"end date string = %@", [dateFormatter stringFromDate:endDate]);
	
    [[dateFormatter stringFromDate:startDate] getCString:cStart maxLength:30 encoding:NSASCIIStringEncoding];
    [[dateFormatter stringFromDate:endDate] getCString:cEnd maxLength:30 encoding:NSASCIIStringEncoding];
	[dateFormatter release];
	
    faketime = parse_time_string(cStart);
    stoptime = parse_time_string(cEnd);
    
    time_t start, end;
    assert (interval > 0);
    if (faketime < stoptime) {
        start = faketime;
        end = stoptime;
    } else {
        start = stoptime;
        end = faketime;
    }
    
    NSMutableArray *tideArray = [[NSMutableArray alloc] init];
    for (; start<=end; start+=(time_t)interval) {
        NSAutoreleasePool *loopPool = [[NSAutoreleasePool alloc] init];
        
        float height = time2asecondary(start);
        NSDate *date = [NSDate dateWithTimeIntervalSince1970:(unsigned long)start];
        SDTideInterval *tide = [[[SDTideInterval alloc] initWithDate:date andHeight:height] autorelease];
        
        [tideArray addObject: tide];
        
        [loopPool release];
    }
    
	NSArray *tideEvents = [self populateTideEvents:stoptime];
	NSLog(@"Found %d tide events.",[tideEvents count]);
    
	NSMutableArray *filteredEvents = [[NSMutableArray alloc] initWithArray:tideEvents];
	
	NSMutableArray *pair = [[NSMutableArray alloc] init];
	for (SDTideEvent *event in tideEvents) {
		[pair addObject:event];
		if ([pair count] == 2) {
			int height1 = [[pair objectAtIndex:0] eventHeight] * 100;
			int height2 = [[pair objectAtIndex:1] eventHeight] * 100;
			
			if (round(height1) == round(height2)) {
				for (id object in pair) {
					[filteredEvents removeObject: object];
					NSLog(@"Removed tide: %@",object);
				}
			}
			[pair removeObjectAtIndex:0];
		}
	}
	
	[pair release];
    
    SDTide *tideObj = [[[SDTide alloc] initWithTideStation: station StartDate:startDate EndDate:endDate Events:filteredEvents andIntervals:tideArray] autorelease];
	
	[filteredEvents release];
	
	[tideArray release];
	
	[tideObj setUnitLong:[NSString stringWithCString:units encoding:NSISOLatin2StringEncoding]];
	[tideObj setUnitShort:[NSString stringWithCString:units_abbrv encoding:NSISOLatin2StringEncoding]];
	
	free_nodes();
	free_epochs();
	free_cst();    
    return tideObj;
}


+(NSArray*) populateTideEvents:(time_t) stoptime
{
    NSMutableArray *result = [[[NSMutableArray alloc] init] autorelease];
    int event_type;
    SDTideState high, low, rise, fall;
    next_ht = faketime;
    if (iscurrent) {
        high = SDTideStateMaxFlood;
        low = SDTideStateMaxEbb;
        rise = fall = SDTideStateSlack;
    } else {
        high = SDTideStateHightTide;
        low =  SDTideStateLowTide;
        rise = SDTideStateRising;
        fall = SDTideStateFalling;
    }

     while ((long)next_ht <= (long)stoptime) {
        event_type = update_high_tide ();
        if ((long)next_ht > (long)stoptime) {
            break;
        }
        NSDate *time = [NSDate dateWithTimeIntervalSince1970:(long)next_ht];
        
        if ((event_type & 4) && (event_type & 1)) {
            SDTideEvent *event = [[SDTideEvent alloc] initWithTime:time Event:fall andHeight: time2atide (next_ht)];
            [result addObject:event];
            [event release];
        }
        if ((event_type & 8) && (event_type & 2)) {
            SDTideEvent *event = [[SDTideEvent alloc] initWithTime:time Event:rise andHeight: time2atide (next_ht)];
            [result addObject:event];
            [event release];
        }
        if (event_type & 1) {
            if (llevelmult) {
                SDTideEvent *event = [[SDTideEvent alloc] initWithTime:time Event:low andHeight: ltleveloff * time2atide (next_ht)];
                [result addObject:event];
                [event release];
            }
            else {
                SDTideEvent *event = [[SDTideEvent alloc] initWithTime:time Event:low andHeight: ltleveloff + time2atide (next_ht)];
                [result addObject:event];
                [event release];
            }
                
        }
        if (event_type & 2) {
            if (hlevelmult) {
                SDTideEvent *event = [[SDTideEvent alloc] initWithTime:time Event:high andHeight: ltleveloff * time2atide (next_ht)];
                [result addObject:event];
                [event release];
            }
            else {
                SDTideEvent *event = [[SDTideEvent alloc] initWithTime:time Event:high andHeight: ltleveloff + time2atide (next_ht)];
                [result addObject:event];
                [event release];
            }
        }
        if ((event_type & 4) && !(event_type & 1)) {
            SDTideEvent *event = [[SDTideEvent alloc] initWithTime:time Event:fall andHeight: time2atide (next_ht)];
            [result addObject:event];
            [event release];
        }
        if ((event_type & 8) && !(event_type & 2)) {
            SDTideEvent *event = [[SDTideEvent alloc] initWithTime:time Event:rise andHeight: time2atide (next_ht)];
            [result addObject:event];
            [event release];
        }
    }
    
    return result;
}

+(NSArray*)locations {
	NSMutableArray *locations = [[[NSMutableArray alloc] init] autorelease];
	
	FMDatabase* db = [FMDatabase databaseWithPath:[[NSBundle mainBundle] pathForResource:@"tidestations" ofType:@"sqlite"]];
    if (![db open]) {
        NSLog(@"Could not open db.");
    }
	
	FMResultSet *rs = [db executeQuery:@"select s.name, s.unit, s.lat, s.long, s.disp_name, s.disp_state from station s"];

	if ([db hadError]) {
		NSLog(@"Err %d: %@", [db lastErrorCode], [db lastErrorMessage]);
	}
	
	while ([rs next]) {
		SDTideStation *station = [[SDTideStation alloc] init];
		[SDTideFactory mapTideStation:station fromResultSet:rs];
		[locations addObject:station];
		[station release];
    }
    // close the result set.
    // it'll also close when it's dealloc'd, but we're closing the database before
    // the autorelease pool closes, so sqlite will complain about it.
    [rs close];
	[db close];
	return locations;
}

+(NSArray*)locationsNearLatitude:(double)latitude andLongitude:(double)longitude {
	NSMutableArray *locations = [[[NSMutableArray alloc] init] autorelease];
	
	NSNumber *minLongitude = [NSNumber numberWithInt:(int)longitude - 2];
	NSNumber *maxLongitude = [NSNumber numberWithInt:(int)longitude + 2];
	NSNumber *minLatitude = [NSNumber numberWithInt:(int)latitude - 2];
	NSNumber *maxLatitude = [NSNumber numberWithInt:(int)latitude + 2];
	
    NSFileManager *fm = [NSFileManager defaultManager];
    
    NSString *cachesDir = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    
    NSString *datastorePath = [[NSBundle mainBundle] pathForResource:@"tidestations" ofType:@"sqlite"];
    
    NSString *cachedDatastorePath = [cachesDir stringByAppendingPathComponent:@"tidestations.sqlite"];
    
    if (![fm fileExistsAtPath:cachedDatastorePath]) {
        NSError *error;
        if (![fm copyItemAtPath:datastorePath toPath:cachedDatastorePath error:&error]) {
            NSLog(@"Unresolved error %@, %@", error, [error userInfo]);
            exit(-1);
        };
    }
    
	FMDatabase* db = [FMDatabase databaseWithPath:cachedDatastorePath];
    if (![db open]) {
        NSLog(@"Could not open db.");
    }
	
	FMResultSet *rs = [db executeQuery:@"select s.name, s.unit, s.lat, s.long, s.disp_name, s.disp_state from station s where s.lat between ? and ? and s.long between ? and ?",
		 minLatitude, 
		 maxLatitude, 
		 minLongitude,
		 maxLongitude];

	if ([db hadError]) {
        NSLog(@"Err %d: %@", [db lastErrorCode], [db lastErrorMessage]);
    }
    while ([rs next]) {
		SDTideStation *station = [[SDTideStation alloc] init];
		[SDTideFactory mapTideStation:station fromResultSet:rs];
		[locations addObject:station];
		[station release];
    }
    // close the result set.
    // it'll also close when it's dealloc'd, but we're closing the database before
    // the autorelease pool closes, so sqlite will complain about it.
    [rs close];  
	[db close];
	return locations;
}

+(SDTideStation*)tideStationWithName:(NSString*)name {
	FMDatabase* db = [FMDatabase databaseWithPath:[[NSBundle mainBundle] pathForResource:@"tidestations" ofType:@"sqlite"]];
    if (![db open]) {
        NSLog(@"Could not open db.");
    }
	
	FMResultSet *rs = [db executeQuery:@"select s.name, s.unit, s.lat, s.long, s.disp_name, s.disp_state from station s where s.name =  ?", name];
	
	[rs next];
	
	SDTideStation *station = [[SDTideStation alloc] init];
	[SDTideFactory mapTideStation:station fromResultSet:rs];
	[station autorelease];
	
	[rs close];
	[db close];
	return station;
}

+(void)mapTideStation:(SDTideStation*)station fromResultSet:(FMResultSet*)rs
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	station.name = [rs stringForColumn:@"name"];
	station.latitude = [NSNumber numberWithDouble:[rs doubleForColumn:@"lat"]];
	station.longitude = [NSNumber numberWithDouble:[rs doubleForColumn:@"long"]];
	station.displayName = [rs stringForColumn:@"disp_name"];
	station.displayState = [rs stringForColumn:@"disp_state"];
	station.units = [rs stringForColumn:@"unit"];
    [pool release];
}

@end
