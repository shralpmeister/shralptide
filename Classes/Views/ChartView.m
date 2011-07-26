//
//  ChartView.m
//  ShralpTide
//
//  Created by Michael Parlee on 9/22/08.
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

#import "ChartView.h"
#import <QuartzCore/QuartzCore.h>

@interface ChartView ()
- (float)findLowestTide:(SDTide *)tide;
- (float)findHighestTide:(SDTide *)tide;
- (void)showTideForPoint:(CGPoint) point;
- (void)hideTideDetails;
- (NSDate*)midnight;
- (NSString*)timeInNativeFormatFromMinutes:(int)minutesSinceMidnight;
- (NSString*)timeIn24HourFormatFromMinutes:(int)minutesSinceMidnight;
- (void)animateFirstTouchAtPoint:(CGPoint)touchPoint;
@end

@implementation ChartView

@synthesize datasource;
@synthesize cursorView;
@synthesize navBarView;
@synthesize activityIndicator;
@synthesize times;

- (id)initWithCoder:(NSCoder *)coder {
	if (self = [super initWithCoder:coder]) {
		times = [[NSMutableDictionary alloc] init];
    }
    return self;
}

-(void)drawRect:(CGRect)rect {
#define HEIGHT 256
#define WIDTH 480
#define MINUTES_FROM_MIDNIGHT 1440
#define SECONDS_PER_MINUTE 60

	CGContextRef context = UIGraphicsGetCurrentContext();
					  
	SDTide *tide = [datasource tideDataToChart];
	
	// Drawing with a white stroke color
	CGContextSetRGBStrokeColor(context, 1.0, 1.0, 1.0, 1.0);
	
	// 480 x 320 = 24hrs x amplitude + some margin
	float min = [self findLowestTide:tide];
	float max = [self findHighestTide:tide];
	
	float ymin = min - 1;
	float ymax = max + 1;
	float yratio =  HEIGHT / (ymax - ymin);
	float yoffset = (HEIGHT + ymin * yratio) + 64;
	NSLog(@"yoffset = %0.4f", yoffset);
	
	float xmin = 0;
	float xmax = MINUTES_FROM_MIDNIGHT;
	float xratio = 0.3333;
	NSLog(@"xratio = %0.4f",xratio);

	CGContextSetLineWidth(context,2.0);
	CGContextMoveToPoint(context, xmin, yoffset);
	CGContextAddLineToPoint(context, xmax * xratio, yoffset);
	CGContextStrokePath(context);
					  
	int basetime = 0;
	for (SDTideInterval *tidePoint in [tide intervals]) {
		int minutesSinceMidnight = 0;
		if (basetime == 0) {
			basetime = (int)[[tidePoint time] timeIntervalSince1970];
		}
		minutesSinceMidnight = (int)([[tidePoint time] timeIntervalSince1970] - basetime) / SECONDS_PER_MINUTE;
		if (minutesSinceMidnight == 0) {
			CGContextMoveToPoint(context, minutesSinceMidnight * xratio, yoffset - [tidePoint height] * yratio);
		} else {
			CGContextAddLineToPoint(context, (minutesSinceMidnight * xratio), yoffset - ([tidePoint height] * yratio));
		}
		
	}
	
	CGContextAddLineToPoint(context, WIDTH, HEIGHT + 64);
	CGContextAddLineToPoint(context, 0, HEIGHT + 64);
	
	CGContextSetRGBFillColor(context, 0.0, 1.0, 1.0, 0.7);
	CGContextFillPath(context);
			
	cursorView.center = CGPointMake([self currentTimeInMinutes:tide] * xratio, (HEIGHT + 64) / 2);
	if (cursorView.center.x > 0) {
		NSLog(@"min: %0.1f, max: %0.1f",min,max);
		[self addSubview:cursorView];
		[self insertSubview:cursorView belowSubview: navBarView];
		[self showTideForPoint:[tide nearestDataPointForTime:floor(cursorView.center.x / xratio)]];	
	} else {
		[self hideTideDetails];
	}


	NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
	[formatter setDateStyle:NSDateFormatterFullStyle];
	self.navBarView.topItem.prompt = [formatter stringFromDate:[tide startTime]];
	[formatter release];
}

-(void)showTideForPoint:(CGPoint) point {
	[[[self.navBarView items] objectAtIndex:0] setTitle:[NSString stringWithFormat:@"%0.2f%@ @ %@",point.y, [[datasource tideDataToChart] unitShort], [self timeInNativeFormatFromMinutes: (int)point.x]]];
}

-(void)hideTideDetails
{
	[[[self.navBarView items] objectAtIndex:0] setTitle:@""];	
}

- (NSDate*)midnight {
	NSCalendar *gregorian = [NSCalendar currentCalendar];
	unsigned unitflags = NSYearCalendarUnit | NSMonthCalendarUnit | NSDayCalendarUnit;
	NSDateComponents *components = [gregorian components: unitflags fromDate: [NSDate date]];
	
	return [gregorian dateFromComponents:components];
}
	

- (int)currentTimeInMinutes:(SDTide *)tide {
	// The following shows the current time on the tide chart.  Need to make sure that it only shows on 
	// the current day!
	NSDate *datestamp = [NSDate date];
	NSDate *midnight = [self midnight];
	
	if ([midnight compare:[tide startTime]] == NSOrderedSame) {
		return ([datestamp timeIntervalSince1970] - [midnight timeIntervalSince1970]) / SECONDS_PER_MINUTE;
	} else {
		return -1;
	}
}

- (NSString*)timeInNativeFormatFromMinutes:(int)minutesSinceMidnight {
	NSString* key = [NSString stringWithFormat:@"%d",minutesSinceMidnight];
	if ([self.times objectForKey:key] != nil) {
		return [self.times objectForKey:key];
	} else {
		int hours = minutesSinceMidnight / 60;
		int minutes = minutesSinceMidnight % 60;
		
		NSCalendar *gregorian = [NSCalendar currentCalendar];
		NSDateComponents *components = [[NSDateComponents alloc] init];
		[components setHour:hours];
		[components setMinute:minutes];
		
		NSDate *time = [gregorian dateByAddingComponents:components toDate:[self midnight] options:0];
		[components release];
		
		NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
		[formatter setTimeStyle:NSDateFormatterShortStyle];
		NSString *timeString = [formatter stringFromDate:time];
		[formatter release];
		
		[self.times setObject:timeString forKey:key];
		return timeString;
	}
}

- (NSString*)timeIn24HourFormatFromMinutes:(int)minutesSinceMidnight {
	int hours = minutesSinceMidnight / 60;
	int minutes = minutesSinceMidnight % 60;
	
	return [NSString stringWithFormat:@"%02d%02d",hours,minutes];
}

- (float)findLowestTide:(SDTide *)tide {	
	NSSortDescriptor *ascDescriptor = [[NSSortDescriptor alloc] initWithKey:@"height" ascending:YES];
	NSArray *descriptors = [NSArray arrayWithObject:ascDescriptor];
	NSArray *ascResult = [[tide intervals] sortedArrayUsingDescriptors:descriptors];
	[ascDescriptor release];
	return [(SDTideInterval*)[ascResult objectAtIndex:0] height];
}

- (float)findHighestTide:(SDTide *)tide {
	NSSortDescriptor *descDescriptor = [[NSSortDescriptor alloc] initWithKey:@"height" ascending:NO];
	NSArray *descriptors = [NSArray arrayWithObject:descDescriptor];
	NSArray *descResult = [[tide intervals] sortedArrayUsingDescriptors:descriptors];
	[descDescriptor release];
	return [(SDTideInterval*)[descResult objectAtIndex:0] height];
}


- (void)dealloc {
	[cursorView release];
	[times release];
    [super dealloc];
}

#pragma mark HandleTouch
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    
    // We only support single touches, so anyObject retrieves just that touch from touches
    UITouch *touch = [touches anyObject];
    
    // Animate the first touch
    CGPoint touchPoint = [touch locationInView:self];
	CGPoint movePoint = CGPointMake(touchPoint.x, 150);
	
	if (cursorView.superview == nil) {
		[self addSubview:cursorView];
		[self insertSubview:cursorView belowSubview: navBarView];
	}
	
    [self animateFirstTouchAtPoint:movePoint];
	[self showTideForPoint: [[datasource tideDataToChart] nearestDataPointForTime: touchPoint.x / 0.3333]];
}


- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    
    UITouch *touch = [touches anyObject];
	
    CGPoint location = [touch locationInView:self];
    cursorView.center = CGPointMake(location.x, 150);
	[self showTideForPoint: [[datasource tideDataToChart] nearestDataPointForTime:location.x / 0.3333]];
}


- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    self.userInteractionEnabled = NO;
    [self animateCursorViewToCurrentTime];
	
	// if not the current day hide the cursor and tide details
	if (cursorView.center.x <= 0.0) {
		[cursorView removeFromSuperview];
		[self hideTideDetails];
	}
}


- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    
    /*
     To impose as little impact on the device as possible, simply set the cursor view's center and transformation to the original values.
     */
    cursorView.center = self.center;
    cursorView.transform = CGAffineTransformIdentity;
}

- (void)animateFirstTouchAtPoint:(CGPoint)touchPoint {
    
#define MOVE_ANIMATION_DURATION_SECONDS 0.15
    
    NSValue *touchPointValue = [NSValue valueWithCGPoint:touchPoint];
    [UIView beginAnimations:nil context:touchPointValue];
    [UIView setAnimationDuration:MOVE_ANIMATION_DURATION_SECONDS];
    cursorView.transform = CGAffineTransformMakeScale(1.5, 1.5);
    cursorView.center = [touchPointValue CGPointValue];
    [UIView commitAnimations];
}

- (void)animateCursorViewToCurrentTime {
    
    // Bounces the placard back to the center
	
    CALayer *welcomeLayer = cursorView.layer;
    
    // Create a keyframe animation to follow a path back to the center
    CAKeyframeAnimation *bounceAnimation = [CAKeyframeAnimation animationWithKeyPath:@"position"];
    bounceAnimation.removedOnCompletion = NO;
    
    CGFloat animationDuration = 0.5;
	
    
    // Create the path for the bounces
    CGMutablePathRef thePath = CGPathCreateMutable();
    
    CGFloat midX = [self currentTimeInMinutes:[self.datasource tideDataToChart]] * 0.3333;
    CGFloat midY = 150.0;
    CGFloat originalOffsetX = cursorView.center.x - midX;
    CGFloat originalOffsetY = cursorView.center.y - midY;
    CGFloat offsetDivider = 10.0;
    
    BOOL stopBouncing = NO;
    
    // Start the path at the cursors's current location
    CGPathMoveToPoint(thePath, NULL, cursorView.center.x, cursorView.center.y);
    CGPathAddLineToPoint(thePath, NULL, midX, midY);
    
    // Add to the bounce path in decreasing excursions from the center
    while (stopBouncing != YES) {
        CGPathAddLineToPoint(thePath, NULL, midX + originalOffsetX/offsetDivider, midY + originalOffsetY/offsetDivider);
        CGPathAddLineToPoint(thePath, NULL, midX, midY);
		
        offsetDivider += 10;
        animationDuration += 1/offsetDivider;
        if ((abs(originalOffsetX/offsetDivider) < 6)) {
            stopBouncing = YES;
        }
    }
    
    bounceAnimation.path = thePath;
    bounceAnimation.duration = animationDuration;
	
    
    // Create a basic animation to restore the size of the placard
    CABasicAnimation *transformAnimation = [CABasicAnimation animationWithKeyPath:@"transform"];
    transformAnimation.removedOnCompletion = YES;
    transformAnimation.duration = animationDuration;
    transformAnimation.toValue = [NSValue valueWithCATransform3D:CATransform3DIdentity];
    
    
    // Create an animation group to combine the keyframe and basic animations
    CAAnimationGroup *theGroup = [CAAnimationGroup animation];
    
    // Set self as the delegate to allow for a callback to reenable user interaction
    theGroup.delegate = self;
    theGroup.duration = animationDuration;
    theGroup.timingFunction = [CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseIn];
    
    theGroup.animations = [NSArray arrayWithObjects:bounceAnimation, transformAnimation, nil];
    
    
    // Add the animation group to the layer
    [welcomeLayer addAnimation:theGroup forKey:@"animatePlacardViewToCenter"];
    
    // Set the placard view's center and transformation to the original values in preparation for the end of the animation
    cursorView.center = CGPointMake(midX, midY);
    cursorView.transform = CGAffineTransformIdentity;
    
    CGPathRelease(thePath);
	
	[self showTideForPoint: [[datasource tideDataToChart] nearestDataPointForTime:midX / 0.3333]];
}


- (void)animationDidStop:(CAAnimation *)theAnimation finished:(BOOL)flag {
    //Animation delegate method called when the animation's finished:
    // restore the transform and reenable user interaction
    cursorView.transform = CGAffineTransformIdentity;
    self.userInteractionEnabled = YES;
}

@end
