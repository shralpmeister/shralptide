//
//  SDTideEvent.h
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

enum {
    SDTideStateLowTide = 1,
    SDTideStateHightTide = 2,
    SDTideStateRising = 3,
    SDTideStateFalling = 4,
    SDTideStateMaxFlood = 5,
    SDTideStateMaxEbb = 6,
    SDTideStateSlack = 7
};
typedef NSUInteger SDTideState;

@interface SDTideEvent : NSObject {
    NSDate *eventTime;
    SDTideState eventType;
    float eventHeight;
}
-(id)initWithTime:(NSDate *)t Event:(SDTideState)e andHeight:(float)f;
-(NSString *)eventTypeDescription;
-(NSString *)eventTimeNativeFormat;
-(NSString *)eventTimeString12HR;
-(NSString *)eventTimeString24HR;
@property (retain,readwrite) NSDate *eventTime;
@property (readonly) SDTideState eventType;
@property (readwrite) float eventHeight;
@end
