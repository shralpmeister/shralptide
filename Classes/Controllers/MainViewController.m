//
//  MainViewController.m
//  ShralpTide
//
//  Created by Michael Parlee on 7/23/08.
//  Copyright Michael Parlee 2009. All rights reserved.
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

#import "MainViewController.h"
#import "MainView.h"
#import "SDTideFactory.h"
#import "SDTide.h"
#import "SDTideEvent.h"
#import "RootViewController.h"

@interface MainViewController (PrivateMethods)
- (int)currentTimeInMinutes:(SDTide *)tide;
@end

@implementation MainViewController

@synthesize sdTide;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
	if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
		// Custom initialization
	}
	return self;
}

// Load the view nib and initialize the pageNumber ivar.
- (id)initWithPageNumber:(int)page {
    if (self = [super initWithNibName:@"MainView" bundle:nil]) {
        pageNumber = page;
    }
    return self;
}

/*
 If you need to do additional setup after loading the view, override viewDidLoad.
 */
- (void)viewDidLoad {
	NSMutableArray *tempTable = [[NSMutableArray alloc] init];
	[tempTable addObject: [NSArray arrayWithObjects: time1, height1, state1, bullet1, nil]];
	[tempTable addObject: [NSArray arrayWithObjects: time2, height2, state2, bullet2, nil]];
	[tempTable addObject: [NSArray arrayWithObjects: time3, height3, state3, bullet3, nil]];
	[tempTable addObject: [NSArray arrayWithObjects: time4, height4, state4, bullet4, nil]];
	
	table = [tempTable retain];
	[tempTable release];
	
	[self refresh];
 }

- (void)setSdTide: (SDTide*)newTide {
	[newTide retain];
	if (sdTide != nil) {
		[sdTide release];
	}
	sdTide = newTide;
	[self refresh];
}

-(void)refresh {
	[self clearTable];
	
	NSDateFormatter *formatter = [[NSDateFormatter alloc] init];
	[formatter setDateStyle:NSDateFormatterFullStyle];
	[date setText: [formatter stringFromDate:[sdTide startTime]]];
	[formatter release];
	
	[locationLabel setText:[sdTide shortLocationName]];
	
	int minutesSinceMidnight = [self currentTimeInMinutes:sdTide];
	if (minutesSinceMidnight > 0) {
		[self updatePresentTideInfo];
	} else {
		[presentHeight setText:@""];
	}
	
	if ([[sdTide events] count] > 4) {
		// there shouldn't be more than 4 tide events in a day -- 2 high, 2 low
		[correctionLabel setText:@"Too many events predicted"];
		return nil;
	}
	 
	int index = 0;
	for (SDTideEvent *event in [sdTide events]) {
		[[[table objectAtIndex:index] objectAtIndex:0] setText: [event eventTimeString24HR]];
		[[[table objectAtIndex:index] objectAtIndex:1] setText: [NSString stringWithFormat:@"%0.2f %@",[event eventHeight], [sdTide unitShort]]];
		[[[table objectAtIndex:index] objectAtIndex:2] setText: [event eventTypeDescription]];
		++index;
	}
}

-(void)updatePresentTideInfo {
	int minutesSinceMidnight = [self currentTimeInMinutes:sdTide];
	
	[presentHeight setText:[NSString stringWithFormat:@"%0.2f %@",
							[sdTide nearestDataPointForTime: minutesSinceMidnight].y,
							[sdTide unitShort]]];
	int direction = [sdTide tideDirectionForTime:minutesSinceMidnight];
	NSString *imageName = nil;
	switch (direction) {
		case SDTideStateRising:
			imageName = @"Increasing";
			break;
		case SDTideStateFalling:
			imageName = @"Decreasing";
			break;
	}
	if (imageName != nil) {
		NSString *imagePath = [[NSBundle mainBundle] pathForResource:imageName 
															  ofType:@"png"];
		[tideStateImage setImage:[UIImage imageWithContentsOfFile:imagePath]];
	} else {
		[tideStateImage setImage:nil];
	}
	
	NSNumber *nextEventIndex = [sdTide nextEventIndex];
	int index = 0;
	for (SDTideEvent *event in [sdTide events]) {
		if (nextEventIndex != nil && index == [nextEventIndex intValue]) {
			[[[table objectAtIndex:index] objectAtIndex:3] setHidden:NO];
		} else {
			[[[table objectAtIndex:index] objectAtIndex:3] setHidden:YES];
		}
		++index;
	}
}

-(void)clearTable {
	[correctionLabel setText:@""];
	for (NSArray *row in table) {
		[[row objectAtIndex:0] setText: @""];
		[[row objectAtIndex:1] setText: @""];
		[[row objectAtIndex:2] setText: @""];
		[[row objectAtIndex:3] setHidden:YES];
	}
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	// Return YES for supported orientations
	return NO;
}

- (void)didReceiveMemoryWarning {
	[super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
	// Release anything that's not essential, such as cached data
}


- (IBAction)chooseTideStation:(id)sender {
	[(RootViewController*)[self parentViewController] chooseFromAllTideStations];
}

-(IBAction)chooseNearbyTideStation:(id)sender {
	[(RootViewController*)[self parentViewController] chooseFromNearbyTideStations];
}

-(IBAction)followHyperlink:(id)sender {
	[[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"http://devocean.com.au"]];
}

#pragma mark UtilMethods
// This should be moved for better re-use... my obj-c/cocoa is lacking though... now in ChartView as well.

- (int)currentTimeInMinutes:(SDTide *)tide {
	// The following shows the current time on the tide chart.  Need to make sure that it only shows on 
	// the current day!
	NSDate *datestamp = [NSDate date];
	
	NSCalendar *gregorian = [NSCalendar currentCalendar];
	unsigned unitflags = NSYearCalendarUnit | NSMonthCalendarUnit | NSDayCalendarUnit;
	NSDateComponents *components = [gregorian components: unitflags fromDate: datestamp];
	
	NSDate *midnight = [gregorian dateFromComponents:components];
	
	if ([midnight compare:[tide startTime]] == NSOrderedSame) {
		return ([datestamp timeIntervalSince1970] - [midnight timeIntervalSince1970]) / 60;
	} else {
		return -1;
	}
}

- (void)dealloc {
	[table release];
	[sdTide release];
	[super dealloc];
}

@end
