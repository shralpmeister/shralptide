//
//  RootViewController.h
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

#import <CoreLocation/CoreLocation.h>
#import "SDTide.h"
#import "WaitView.h"
#import "SDTideStation.h"
#import "ChartScrollView.h"

@class MainViewController;
@class FlipsideViewController;

@interface RootViewController : UIViewController <UITableViewDelegate, UITableViewDataSource, UISearchBarDelegate, UIScrollViewDelegate, CLLocationManagerDelegate>
{

	IBOutlet UIButton *infoButton;
	IBOutlet UIScrollView *scrollView;
    IBOutlet UIPageControl *pageControl;
	IBOutlet WaitView *waitView;
	IBOutlet UILabel *waitReason;
	IBOutlet UIActivityIndicatorView *waitIndicator;
    NSMutableArray *viewControllers;
	NSMutableArray *chartViewControllers;
	FlipsideViewController *flipsideViewController;
	IBOutlet ChartScrollView *chartScrollView;
	UISearchBar *searchBar;
	UIActivityIndicatorView *activityIndicator;
	UITableView *tableView;
	NSArray *locations;
	NSArray *allLocations;
	NSMutableArray *nearbyLocations;
	NSMutableArray *filteredLocations;
	NSMutableArray *savedLocations;
	SDTide *sdTide;
	NSString *location;
	NSCalendar *currentCalendar;
    CLLocationManager *locationManager;
	BOOL transitioning;
	BOOL pageControlUsed;
	BOOL acceptLocationUpdates;
	SDTideStation *tideStation;
}

@property (nonatomic, retain) UIButton *infoButton;
@property (nonatomic, retain) UIScrollView *scrollView;
@property (nonatomic, retain) FlipsideViewController *flipsideViewController;
@property (nonatomic, retain) ChartScrollView *chartScrollView;
@property (nonatomic, retain) UISearchBar *searchBar;
@property (nonatomic, retain) UIActivityIndicatorView *activityIndicator;
@property (nonatomic, retain) NSArray *locations;
@property (nonatomic, retain) NSMutableArray *filteredLocations;
@property (nonatomic, retain) NSString *location;
@property (nonatomic, retain) NSMutableArray *savedLocations;
@property (nonatomic, retain) SDTide* sdTide;
@property (nonatomic, retain) NSMutableArray *viewControllers;
@property (nonatomic, retain) NSMutableArray *chartViewControllers;
@property (nonatomic, retain) NSCalendar *currentCalendar;
@property (nonatomic, retain) CLLocationManager *locationManager;
@property (nonatomic, retain) UILabel *waitReason;
@property (nonatomic, retain) SDTideStation *tideStation;

@property (readonly, getter=isTransitioning) BOOL transitioning;

-(IBAction)chooseFromAllTideStations;
-(void)chooseFromNearbyTideStations;
-(void)toggleView;
- (IBAction)changePage:(id)sender;

- (SDTide *)computeTidesForDate:(NSDate*)date;
-(NSDate *)add:(int)number daysToDate: (NSDate*) date;
- (void)recalculateTides:(id)object;
-(void)updateWaitReason:(id)object;
- (void)refreshViews;

- (void)locationManager:(CLLocationManager *)manager
    didUpdateToLocation:(CLLocation *)newLocation
           fromLocation:(CLLocation *)oldLocation;

- (void)locationManager:(CLLocationManager *)manager
       didFailWithError:(NSError *)error;

@end
