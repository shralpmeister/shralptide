//
//  SDTideFactory.h
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
#import "FMDatabase.h"
#import "FMResultSet.h"

@interface SDTideFactory : NSObject {
}

+(SDTide*)tideWithStart:(NSDate*)startDate End:(NSDate*)endDate andInterval:(int)interval atLocation:(NSString *)aLocation;
+(NSArray*) populateTideEvents:(time_t) stoptime;
+(NSArray*)locations;
+(NSArray*)locationsNearLatitude:(double)latitude andLongitude:(double)longitude;
@end
