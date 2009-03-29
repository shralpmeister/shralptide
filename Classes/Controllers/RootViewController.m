//
//  RootViewController.m
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

#import <QuartzCore/QuartzCore.h>

#import "RootViewController.h"
#import "MainViewController.h"
#import "FlipsideViewController.h"
#import "SDTideFactory.h"
#import "ChartViewController.h"
#import "ChartView.h"

// Shorthand for getting localized strings, used in formats below for readability
#define LocStr(key) [[NSBundle mainBundle] localizedStringForKey:(key) value:@"" table:nil]

//crazy core animation stuff
#define kAnimationKey @"transitionViewAnimation"

@interface RootViewController (PrivateMethods)
- (void)loadScrollViewWithPage:(int)page;
- (void)scrollViewDidScroll:(UIScrollView *)sender;
- (void)createMainViews;
- (void)startWaitIndicator;
- (void)stopWaitIndicator;
- (void)saveState;
- (NSString*)lastLocation;
- (BOOL)writeApplicationPlist:(id)plist toFile:(NSString *)fileName;
- (id)applicationPlistFromFile:(NSString *)fileName;
- (BOOL)writeApplicationData:(NSData *)data toFile:(NSString *)fileName;
- (NSData *)applicationDataFromFile:(NSString *)fileName;
- (void)replaceSubview:(UIView *)oldView withSubview:(UIView *)newView transition:(NSString *)transition direction:(NSString *)direction duration:(NSTimeInterval)duration;
-(void)refreshLocationTable;
-(void)setDefaultLocation;
@end

static NSUInteger kNumberOfPages = 5;

@implementation RootViewController

@synthesize infoButton;
@synthesize flipsideViewController;
@synthesize chartViewController;
@synthesize searchBar;
@synthesize activityIndicator;
@synthesize location;
@synthesize locations;
@synthesize filteredLocations;
@synthesize savedLocations;
@synthesize sdTide;
@synthesize currentCalendar;
@synthesize viewControllers;
@synthesize scrollView;
@synthesize locationManager;
@synthesize waitReason;
@synthesize transitioning;
@synthesize tideStation;

- (void)viewDidLoad {
	NSString *lastLocation = [self lastLocation];
	
	if (lastLocation) {
		[self setLocation:lastLocation];
		self.tideStation = [SDTideFactory tideStationWithName:lastLocation];
		if (self.tideStation.name == nil) {
			NSString *message = [NSString stringWithFormat:@"%@ is no longer a supported location. Please choose another location.",lastLocation];
			UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Sorry" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
			[alert show];
			[alert release];
			[self setDefaultLocation];
		}
	} else {
		[self setDefaultLocation];
	}
	self.currentCalendar = [NSCalendar currentCalendar];
	
	[self createMainViews];
}

- (void)setDefaultLocation {
	NSString *defaultLocation = @"La Jolla, Scripps Pier, California";
	[self setLocation:defaultLocation];
	self.tideStation = [SDTideFactory tideStationWithName:defaultLocation];
}

- (void)createMainViews {
	NSMutableArray *controllers = [[NSMutableArray alloc] init];
    for (unsigned i = 0; i < kNumberOfPages; i++) {
        [controllers addObject:[NSNull null]];
    }
    self.viewControllers = controllers;
    [controllers release];
	
    // a page is the width of the scroll view
    scrollView.pagingEnabled = YES;
    scrollView.contentSize = CGSizeMake(self.view.frame.size.width * kNumberOfPages, self.view.frame.size.height);
    scrollView.showsHorizontalScrollIndicator = NO;
    scrollView.showsVerticalScrollIndicator = NO;
    scrollView.scrollsToTop = NO;
	scrollView.directionalLockEnabled = YES;
    scrollView.delegate = self;
	scrollView.autoresizingMask = UIViewAutoresizingNone;
	
    pageControl.numberOfPages = kNumberOfPages;
    pageControl.currentPage = 0;
	pageControl.hidden = NO;
	pageControl.defersCurrentPageDisplay = YES;
	
	[pageControl removeFromSuperview];
	[self.view addSubview:pageControl];
	
	[self loadScrollViewWithPage:0];
}

- (void)refreshViews {
	NSLog(@"Refresh views called at %@", [NSDate date]);
	if (chartViewController != nil && [chartViewController.view superview] != nil) {
		[chartViewController showCurrentTime];
	}
	
	MainViewController *pageOneController = [viewControllers objectAtIndex:0];
	if ([[NSDate date] timeIntervalSinceDate: [[pageOneController sdTide] startTime]] > 86400) {
		[self viewDidAppear:YES];
	} else {
		[pageOneController updatePresentTideInfo];
	}
}

- (void)recalculateTides:(id)object {
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	for (unsigned i = 0; i < kNumberOfPages; i++) {
		
		[self performSelectorOnMainThread:@selector(updateWaitReason:) 
							   withObject:[NSString stringWithFormat:@"Calculating day %d", i + 1] 
							waitUntilDone:NO];
		
		[self loadScrollViewWithPage:i];
	}
	[self stopWaitIndicator];
	[pool release];
}

-(void)startWaitIndicator {
	UIViewController* currentPageController = [viewControllers objectAtIndex:pageControl.currentPage];
	if ((NSNull*)currentPageController == [NSNull null]) {
		return;
	}
	[self.view insertSubview:waitView aboveSubview:scrollView];
	[waitIndicator startAnimating];
}

-(void)stopWaitIndicator {
	[waitIndicator stopAnimating];
	[waitReason setText:@""];
	if ([waitView superview] != nil) {
		[waitView removeFromSuperview];
	}
}

-(void)updateWaitReason:(id)object
{
	[waitReason setText:(NSString*)object];
}

#pragma mark UIScrollViewDelegate
- (void)loadScrollViewWithPage:(int)page {
    if (page < 0) return;
    if (page >= kNumberOfPages) return;
	
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
    // replace the placeholder if necessary
    MainViewController *controller = [viewControllers objectAtIndex:page];
    if ((NSNull *)controller == [NSNull null]) {
        controller = [[MainViewController alloc] initWithPageNumber:page];
		
		[controller setSdTide:[self computeTidesForDate: [self add: page daysToDate: [NSDate date]]]];
		[controller setParentViewController:self];
        [viewControllers replaceObjectAtIndex:page withObject:controller];
        [controller release];
    } else {
		[controller setSdTide:[self computeTidesForDate: [self add: page daysToDate: [NSDate date]]]];
	}
	
	[pool release];
	
    // add the controller's view to the scroll view
    if (nil == controller.view.superview) {
        CGRect frame = scrollView.frame;
        frame.origin.x = frame.size.width * page;
        frame.origin.y = 0;
        controller.view.frame = frame;
        [scrollView addSubview:controller.view];
    }

}

-(NSDate *)add:(int)number daysToDate: (NSDate*) date {
	unsigned int unitFlags = NSDayCalendarUnit;
	NSDateComponents *comps = [[NSDateComponents alloc] init];
	[comps setDay: number];
	NSDate *result = [currentCalendar dateByAddingComponents:comps toDate:date options:unitFlags];
	[comps release];
	return result;
}

- (void)scrollViewDidScroll:(UIScrollView *)sender {
    // We don't want a "feedback loop" between the UIPageControl and the scroll delegate in
    // which a scroll event generated from the user hitting the page control triggers updates from
    // the delegate method. We use a boolean to disable the delegate logic when the page control is used.
    if (pageControlUsed) {
        // do nothing - the scroll was initiated from the page control, not the user dragging
        return;
    }
    // Switch the indicator when more than 50% of the previous/next page is visible
    CGFloat pageWidth = scrollView.frame.size.width;
    int page = floor((scrollView.contentOffset.x - pageWidth / 2) / pageWidth) + 1;
    pageControl.currentPage = page;
	
    // load the visible page and the page on either side of it (to avoid flashes when the user starts scrolling)
    //[self loadScrollViewWithPage:page - 1];
    //[self loadScrollViewWithPage:page];
    //[self loadScrollViewWithPage:page + 1];
	
    // A possible optimization would be to unload the views+controllers which are no longer visible
}

// At the end of scroll animation, reset the boolean used when scrolls originate from the UIPageControl
- (void)scrollViewDidEndScrollingAnimation:(UIScrollView *)aScrollView {
	NSLog(@"scrollview ended animating. page: %d",pageControl.currentPage);
	[pageControl updateCurrentPageDisplay];
    pageControlUsed = NO;
}

- (IBAction)changePage:(id)sender {
    int page = pageControl.currentPage;
    // load the visible page and the page on either side of it (to avoid flashes when the user starts scrolling)
    //[self loadScrollViewWithPage:page - 1];
    //[self loadScrollViewWithPage:page];
    //[self loadScrollViewWithPage:page + 1];
    // update the scroll view to the appropriate page
    CGRect frame = scrollView.frame;
    frame.origin.x = frame.size.width * page;
    frame.origin.y = 0;
    [scrollView scrollRectToVisible:frame animated:YES];
    // Set the boolean used when scrolls originate from the UIPageControl. See scrollViewDidScroll: above.
    pageControlUsed = YES;
}

#pragma mark ViewToggleControls

- (void)loadFlipsideViewController {
	FlipsideViewController *viewController = [[FlipsideViewController alloc] initWithNibName:@"FlipsideView" bundle:nil];
	self.flipsideViewController = viewController;
	[viewController release];
	
	[[flipsideViewController view] setAutoresizingMask:(UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight)];

	filteredLocations = [[NSMutableArray alloc] initWithCapacity:[locations count]];
	[filteredLocations addObjectsFromArray:locations];
	
	savedLocations = [[NSMutableArray alloc] initWithCapacity:[locations count]];
	
	UISearchBar *mySearchBar = [[UISearchBar alloc] initWithFrame:CGRectMake(0.0, 0.0, 320.0, 44.0)];
	mySearchBar.barStyle = UIBarStyleBlackOpaque;
	// don't get in the way of use typing in any way!
    mySearchBar.autocorrectionType = UITextAutocorrectionTypeNo;
    mySearchBar.autocapitalizationType = UITextAutocapitalizationTypeNone;
    mySearchBar.showsCancelButton = NO;
	mySearchBar.autoresizingMask = UIViewAutoresizingFlexibleWidth;
	
	[mySearchBar setDelegate:self];
	self.searchBar = mySearchBar;
	[mySearchBar release];
	
	tableView = [[UITableView alloc] initWithFrame: CGRectMake(0.0, 45.0, 320.0, 420.0) style:UITableViewStylePlain];

	[tableView setRowHeight:60.0];
	[tableView setDelegate:self];
	[tableView setDataSource:self];
	
	tableView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
}

-(void)refreshLocationTable {
	[filteredLocations release];
	[savedLocations release];
	
	filteredLocations = [[NSMutableArray alloc] initWithCapacity:[locations count]];
	[filteredLocations addObjectsFromArray:locations];
	
	savedLocations = [[NSMutableArray alloc] initWithCapacity:[locations count]];
	
	[searchBar setText:@""];
	
	[tableView reloadData];
	[tableView scrollToRowAtIndexPath:0 atScrollPosition:UITableViewScrollPositionTop animated:NO];
}

- (void)loadChartViewController
{
	ChartViewController *viewController = [[ChartViewController alloc] initWithNibName:@"ChartView" bundle:nil];
	self.chartViewController = viewController;
	[viewController release];
}

-(void)chooseFromNearbyTideStations {
	[self startWaitIndicator];
	[waitReason setText:@"Determining current location."];

	self.locationManager = [[CLLocationManager alloc] init];
	self.locationManager.delegate = self; // Tells the location manager to send updates to this object
	
	acceptLocationUpdates = YES;
	
	[locationManager startUpdatingLocation];
}

-(void)showNearbyLocations: (CLLocation*) newLocation {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	nearbyLocations = [[SDTideFactory locationsNearLatitude:newLocation.coordinate.latitude 
											  andLongitude:newLocation.coordinate.longitude] retain];
	[pool release];

	for (SDTideStation *station in nearbyLocations) {
		CLLocation *stationLoc = [[CLLocation alloc] initWithLatitude:[station.latitude doubleValue]
															longitude:[station.longitude doubleValue]];
		station.distance = [NSNumber numberWithDouble:([newLocation getDistanceFrom:stationLoc] / 1000)];
		[stationLoc release];
	}
	
	NSSortDescriptor *sortDescriptor = [[NSSortDescriptor alloc] initWithKey:@"distance" ascending: YES];
	NSSortDescriptor *nameDescriptor = [[NSSortDescriptor alloc] initWithKey:@"name" ascending: YES];
	NSArray *descriptors = [NSArray arrayWithObjects:sortDescriptor,nameDescriptor,nil];
	
	[nearbyLocations sortUsingDescriptors:descriptors];
	
	locations = nearbyLocations;
	
	[sortDescriptor release];
	[nameDescriptor release];
	
	[self stopWaitIndicator];
	
	if (flipsideViewController) {
		[self refreshLocationTable];
	} else {
		[self loadFlipsideViewController];
	}
	
	if ([locations count] > 0) {
		[self toggleView];
	} else {
		UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Sorry" message:@"There are no tide stations in your area. You may select a location manually by pressing the globe icon." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alert show];
		[alert release];
	}
}

-(IBAction)chooseFromAllTideStations {
	if (!allLocations) {
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		allLocations = [[SDTideFactory locations] retain];
		[pool release];
	}
	if (locations != allLocations) {
		if (locations != nil) {
			[locations release];
		}
		locations = allLocations;
		NSLog(@"Locations retain count=%d",[locations retainCount]);
		if (flipsideViewController) {
			[self refreshLocationTable];
		} else {
			[self loadFlipsideViewController];
		}
	}
	[self toggleView];
}

- (void) toggleView {	
	/*
	 This method is called when the info or Done button is pressed.
	 It flips the displayed view from the main view to the flipside view and vice-versa.
	 */
	UIView *mainView = scrollView;
	UIView *flipsideView = flipsideViewController.view;
	
	[mainView setNeedsLayout];
	[flipsideView setNeedsLayout];
	[UIView beginAnimations:nil context:NULL];
	[UIView setAnimationDuration:1];
	[UIView setAnimationTransition:([mainView superview] ? UIViewAnimationTransitionFlipFromRight : UIViewAnimationTransitionFlipFromLeft) forView:self.view cache:YES];
	
	if (flipsideView.superview == nil) {
		[flipsideViewController viewWillAppear:YES];
		[self viewWillDisappear:YES];
		[pageControl removeFromSuperview];
		[mainView removeFromSuperview];
		[self.view addSubview:flipsideView];
		[self.view insertSubview:searchBar aboveSubview:flipsideView];
		[self.view insertSubview:tableView aboveSubview:flipsideView];
		[self viewDidDisappear:YES];
		[flipsideViewController viewDidAppear:YES];
	} else {
		[self viewWillAppear:YES];
		[flipsideViewController viewWillDisappear:YES];
		[flipsideView removeFromSuperview];
		[searchBar removeFromSuperview];
		[tableView removeFromSuperview];
		[self.view addSubview:mainView];
		[self.view addSubview:pageControl];
		[flipsideViewController viewDidDisappear:YES];
		[self viewDidAppear:YES];
	}
	[UIView commitAnimations];
}

-(void)viewDidAppear:(BOOL)animated {
	[super viewDidAppear: animated];
	[self startWaitIndicator];
	[NSThread detachNewThreadSelector:@selector(recalculateTides:) toTarget:self withObject:nil];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	UIView *mainView = scrollView;
	UIView *chartView = [chartViewController view];
	
	NSArray *subviews = [self.view subviews];
	if ([[subviews objectAtIndex:([subviews count] - 1)] isKindOfClass:[WaitView class]]) {
		return NO;
	} else if ([mainView superview] != nil || [chartView superview] != nil) {
		if ([mainView superview] != nil && interfaceOrientation == UIInterfaceOrientationPortraitUpsideDown) {
			return NO;
		}
		return YES;
	} else {
		return NO;
	}
}


- (void)didReceiveMemoryWarning {
	[super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
	// Release anything that's not essential, such as cached data
	NSLog(@"Low memory warning!");
	
	if (flipsideViewController && ![flipsideViewController.view superview]) {
		if (locations) {
			[locations release];
			locations = nil;
			allLocations = nil;
		}
		
		if (nearbyLocations) {
			[nearbyLocations release];
			nearbyLocations = nil;
		}
		
		if (filteredLocations) {
			[filteredLocations release];
			filteredLocations = nil;
		}
		
		if (savedLocations) {
			[savedLocations release];
			savedLocations = nil;
		}
	}
}


- (void)dealloc {
	[infoButton release];
	[flipsideViewController release];
	[chartViewController release];
	[searchBar release];
	[locations release];
	[filteredLocations release];
	[savedLocations release];
	[currentCalendar release];
	[viewControllers release];
	[scrollView release];
	[waitIndicator release];
	[waitReason release];
	[waitView release];
	[allLocations release];
	[nearbyLocations release];
	[locationManager release];
	[tideStation release];
	[super dealloc];
}

#pragma mark UIViewController

- (void)viewWillAppear:(BOOL)animated
{
    NSIndexPath *tableSelection = [tableView indexPathForSelectedRow];
    [tableView deselectRowAtIndexPath:tableSelection animated:NO];
}

-(SDTide *)computeTidesForDate:(NSDate *)date {
	unsigned int unitFlags = NSYearCalendarUnit | NSMonthCalendarUnit | NSDayCalendarUnit;
	NSDateComponents *nowComps = [currentCalendar components:unitFlags fromDate:date]; 
	NSDateComponents *comps = [[NSDateComponents alloc] init];
	[comps setYear: [nowComps year]];
	[comps setMonth: [nowComps month]];
	[comps setDay: [nowComps day]];
	
	[comps setHour: 0];
	[comps setMinute: 0];
	[comps setSecond:0];
	NSDate *start = [currentCalendar dateFromComponents:comps];
	
	[comps setHour:24];
	[comps setMinute:00];
	[comps setSecond:00];
	NSDate *end = [currentCalendar dateFromComponents:comps];
	
	[comps release];
	
	SDTide *result = [SDTideFactory tideWithStart:start 
									   End:end 
							   andInterval:900 
								atStation:tideStation];
	
	return result;
}
-(void)showMainView {
	if (chartViewController == nil) {
		return;
	}
	
	UIView *mainView = scrollView;
	UIView *chartView = [chartViewController view];
	
	if ([chartView superview] == nil) {
		return;
	}
	
	[mainView setNeedsLayout];
	[(MainViewController*)[self.viewControllers objectAtIndex:0] updatePresentTideInfo];
	[self replaceSubview:chartView withSubview:mainView transition:kCATransitionFade direction:@"" duration:0.75];
	[self.view addSubview:pageControl];
	
}

-(void)showChartView
{
	if (chartViewController == nil) {
		[self loadChartViewController];
	}
	
	NSLog(@"Setting chart view to use tide from page %d",pageControl.currentPage);
	SDTide *tide = [[[self viewControllers] objectAtIndex:pageControl.currentPage] sdTide];
	[chartViewController setTide:tide];
	
	UIView *mainView = scrollView;
	UIView *chartView = [chartViewController view];
	
	if ([mainView superview] == nil) {
		return;
	}
	
	[chartView setNeedsDisplay];
	[pageControl removeFromSuperview];
	[self replaceSubview:mainView withSubview:chartView transition:kCATransitionFade direction:@"" duration:0.75];
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
{
	UIView *mainView = scrollView; 
	UIView *chartView = [chartViewController view];
	
	if ([mainView superview] != nil || [chartView superview] != nil) {
		switch (toInterfaceOrientation) {
			case UIDeviceOrientationLandscapeLeft:
				NSLog(@"Device rotated to Landscape Left");
				[self showChartView];
				break;
			case UIDeviceOrientationLandscapeRight:
				NSLog(@"Device rotated to Landscape Right");
				[self showChartView];
				break;
			case UIDeviceOrientationPortrait:
				NSLog(@"Device rotated to Portrait");
				[self showMainView];
				break;
			case UIDeviceOrientationPortraitUpsideDown:
				NSLog(@"Device rotated to Portrait upsidedown");
				[self showMainView];
				break;
		}
	}
		

}

#pragma mark UITableViewDataSource

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [filteredLocations count];
}

- (UITableViewCell *)tableView:(UITableView *)aTableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = [aTableView dequeueReusableCellWithIdentifier:@"TideStationCell"];

	UILabel *name, *distance;
	
    if (cell == nil)
    {
		CGRect frame = CGRectMake(0, 0, 300, 60);
        cell = [[[UITableViewCell alloc] initWithFrame:frame reuseIdentifier:@"TideStationCell"] autorelease];

		name = [[[UILabel alloc] initWithFrame:CGRectMake(10.0, 0.0, 300.0, 23.0)] autorelease];
		name.tag = 1;
		name.font = [UIFont boldSystemFontOfSize:20.0];
		name.adjustsFontSizeToFitWidth = YES;
		name.textAlignment = UITextAlignmentLeft;
		name.textColor = [UIColor blackColor];
		name.autoresizingMask = UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleHeight;
		
		distance = [[[UILabel alloc] initWithFrame:CGRectMake(10.0, 28.0, 200.0, 16.0)] autorelease];
		distance.tag = 2;
		distance.font = [UIFont systemFontOfSize:14.0];
		distance.textColor = [UIColor blackColor];
		distance.lineBreakMode = UILineBreakModeWordWrap;
		distance.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
		[cell.contentView addSubview:distance];
		
		[cell.contentView addSubview:name];
		
    } else {
		name = (UILabel *)[cell.contentView viewWithTag:1];
		distance = (UILabel *)[cell.contentView viewWithTag:2];
	}
    SDTideStation *station = [filteredLocations objectAtIndex:indexPath.row];
    name.text = station.displayName;
	
	NSNumberFormatter *formatter = [[NSNumberFormatter alloc] init];
	[formatter setNumberStyle:NSNumberFormatterDecimalStyle];
	[formatter setMaximumFractionDigits:1];
	
	if (station.distance != nil) {
		if (![station.units isEqualToString:@"meters"]) {
			float distanceMiles = [station.distance floatValue] * 0.62137119;
			NSNumber *milesNumber = [NSNumber numberWithFloat:distanceMiles];
			distance.text = [NSString stringWithFormat: @"%@ (%@ mi)", station.displayState, [formatter stringFromNumber:milesNumber]];	
		} else {
			distance.text = [NSString stringWithFormat: @"%@ (%@ km)", station.displayState, [formatter stringFromNumber:station.distance]];		
		}
	} else {
		distance.text = [NSString stringWithFormat: @"%@", station.displayState];
	}
	
	[formatter release];
    
    return cell;
}


#pragma mark UITableViewDelegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    // in case the searchbar's keyboard is up, dismiss it on table selection
    if ([searchBar isFirstResponder])
        [searchBar resignFirstResponder];
	
	self.tideStation = [filteredLocations objectAtIndex:indexPath.row];
	
	[self setLocation: [tideStation name]];
	[self saveState];
	[self toggleView];
}


#pragma mark UISearchBarDelegate

- (void)searchBarTextDidBeginEditing:(UISearchBar *)aSearchBar
{
    // only show the status bar's cancel button while in edit mode
    aSearchBar.showsCancelButton = YES;
    
    // flush and save the current list content in case the user cancels the search later
    [savedLocations removeAllObjects];
    [savedLocations addObjectsFromArray: filteredLocations];
}

- (void)searchBarTextDidEndEditing:(UISearchBar *)aSearchBar
{
    aSearchBar.showsCancelButton = NO;
}

- (void)searchBar:(UISearchBar *)aSearchBar textDidChange:(NSString *)searchText
{
    [filteredLocations removeAllObjects];    // clear the filtered array first
    
    // search the table content for cell titles that match "searchText"
    // if found add to the mutable array and force the table to reload
    //
    SDTideStation *station;
    for (station in locations)
    {
        NSComparisonResult result = [station.name compare:searchText options:NSCaseInsensitiveSearch
												 range:NSMakeRange(0, [searchText length])];
        if (result == NSOrderedSame)
        {
            [filteredLocations addObject:station];
        }
    }
    
    [tableView reloadData];
}

// called when cancel button pressed
- (void)searchBarCancelButtonClicked:(UISearchBar *)aSearchBar
{
    // if a valid search was entered but the user wanted to cancel, bring back the saved list content
    if (aSearchBar.text.length > 0)
    {
        [filteredLocations removeAllObjects];
        [filteredLocations addObjectsFromArray: savedLocations];
    }
    
    [tableView reloadData];
    
    [aSearchBar resignFirstResponder];
    aSearchBar.text = @"";
}

// called when Search (in our case "Done") button pressed
- (void)searchBarSearchButtonClicked:(UISearchBar *)aSearchBar
{
    [aSearchBar resignFirstResponder];
}

#pragma mark CLLocationManagerDelegate

- (void)locationManager:(CLLocationManager *)manager
    didUpdateToLocation:(CLLocation *)newLocation
           fromLocation:(CLLocation *)oldLocation
{
	if (!acceptLocationUpdates) {
		return;
	}
	NSLog(@"Location determined: %0.4f,%0.4f", newLocation.coordinate.latitude, newLocation.coordinate.longitude);
	NSLog(@"at time: %@, current time is: %@", newLocation.timestamp, [NSDate date]);
	
	if ([[newLocation.timestamp addTimeInterval:60] compare:[NSDate date]] == NSOrderedAscending) {
		// if the location timestamp is more than a minute old, do nothing and wait for another update
		return;
	} else {
		// now that we have a fresh location we can stop updating and get on with the show
		acceptLocationUpdates = NO;
		[self stopWaitIndicator];
		[manager stopUpdatingLocation];
	}
	
#if TARGET_IPHONE_SIMULATOR
	[newLocation release];
	//newLocation = [[CLLocation alloc] initWithLatitude:48.4500 longitude:-123.3000]; // victoria, bc
	//newLocation = [[CLLocation alloc] initWithLatitude:-33.867707 longitude: 151.225777]; sydney, aus
	//newLocation = [[CLLocation alloc] initWithLatitude:-36.846581 longitude: 174.77809]; // aukland, nz
	//newLocation = [[CLLocation alloc] initWithLatitude:35.570922 longitude:140.331673]; // chiba, jp
	newLocation = [[CLLocation alloc] initWithLatitude:41.855242 longitude:-87.618713]; // chicago, il
#endif
	
    if (signbit(newLocation.horizontalAccuracy)) {
        // Negative accuracy means an invalid or unavailable measurement
		UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Sorry" message:@"Unable to determine your location at this time." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		[alert show];
		[alert release];
    } else {
		[self showNearbyLocations: newLocation];
    }
	
}


// Called when there is an error getting the location
- (void)locationManager:(CLLocationManager *)manager
       didFailWithError:(NSError *)error
{
	[self stopWaitIndicator];
	[locationManager stopUpdatingLocation];
    NSMutableString *errorString = [[[NSMutableString alloc] init] autorelease];
	
    if ([error domain] == kCLErrorDomain) {
		
        // We handle CoreLocation-related errors here
		
        switch ([error code]) {
				// This error code is usually returned whenever user taps "Don't Allow" in response to
				// being told your app wants to access the current location. Once this happens, you cannot
				// attempt to get the location again until the app has quit and relaunched.
				//
				// "Don't Allow" on two successive app launches is the same as saying "never allow". The user
				// can reset this for all apps by going to Settings > General > Reset > Reset Location Warnings.
				//
            case kCLErrorDenied:
                [errorString appendFormat:@"%@\n", NSLocalizedString(@"LocationDenied", nil)];
                break;
				
				// This error code is usually returned whenever the device has no data or WiFi connectivity,
				// or when the location cannot be determined for some other reason.
				//
				// CoreLocation will keep trying, so you can keep waiting, or prompt the user.
				//
            case kCLErrorLocationUnknown:
                [errorString appendFormat:@"%@\n", NSLocalizedString(@"LocationUnknown", nil)];
                break;
				
				// We shouldn't ever get an unknown error code, but just in case...
				//
            default:
                [errorString appendFormat:@"%@ %d\n", NSLocalizedString(@"GenericLocationError", nil), [error code]];
                break;
        }
    } else {
        // We handle all non-CoreLocation errors here
        // (we depend on localizedDescription for localization)
        [errorString appendFormat:@"Error domain: \"%@\"  Error code: %d\n", [error domain], [error code]];
        [errorString appendFormat:@"Description: \"%@\"\n", [error localizedDescription]];
    }
	
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Sorry" message:@"Unable to determine your location at this time." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
	[alert show];
	[alert release];
}

#pragma mark CrazyCoreAnimationStuff

	// Method to replace a given subview with another using a specified transition type, direction, and duration
- (void)replaceSubview:(UIView *)oldView withSubview:(UIView *)newView transition:(NSString *)transition direction:(NSString *)direction duration:(NSTimeInterval)duration {
		
		// If a transition is in progress, do nothing
		if(transitioning) {
			return;
		}
		
		[oldView removeFromSuperview];
	
		[self.view addSubview:newView];
		
		
		// Set up the animation
		CATransition *animation = [CATransition animation];
		[animation setDelegate:self];
		
		// Set the type and if appropriate direction of the transition, 
		if (transition == kCATransitionFade) {
			[animation setType:kCATransitionFade];
		} else {
			[animation setType:transition];
			[animation setSubtype:direction];
		}
		
		// Set the duration and timing function of the transtion -- duration is passed in as a parameter, use ease in/ease out as the timing function
		[animation setDuration:duration];
		[animation setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
		
		[[self.view layer] addAnimation:animation forKey:kAnimationKey];
	}

#pragma mark savestate
- (void)saveState {
	NSMutableDictionary *plist = [[NSMutableDictionary alloc] initWithCapacity:1];
	[plist setObject:location forKey:@"location"];
	[self writeApplicationPlist:plist toFile:@"tidestate.plist"];
	[plist release];
}

-(NSString*)lastLocation {
	NSDictionary *plist = [self applicationPlistFromFile:@"tidestate.plist"];
	return [(NSString *)[plist objectForKey:@"location"] retain];
}

- (BOOL)writeApplicationPlist:(id)plist toFile:(NSString *)fileName {
    NSString *error;
    NSData *pData = [NSPropertyListSerialization dataFromPropertyList:plist format:NSPropertyListBinaryFormat_v1_0 errorDescription:&error];
    if (!pData) {
        NSLog(@"%@", error);
        return NO;
    }
    return ([self writeApplicationData:pData toFile:(NSString *)fileName]);
}

- (id)applicationPlistFromFile:(NSString *)fileName {
    NSData *retData;
    NSString *error;
    id retPlist;
    NSPropertyListFormat format;
	
    retData = [self applicationDataFromFile:fileName];
    if (!retData) {
        NSLog(@"Data file not returned.");
        return nil;
    }
    retPlist = [NSPropertyListSerialization propertyListFromData:retData  mutabilityOption:NSPropertyListImmutable format:&format errorDescription:&error];
    if (!retPlist){
        NSLog(@"Plist not returned, error: %@", error);
    }
    return retPlist;
}

- (BOOL)writeApplicationData:(NSData *)data toFile:(NSString *)fileName {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    if (!documentsDirectory) {
        NSLog(@"Documents directory not found!");
        return NO;
    }
    NSString *appFile = [documentsDirectory stringByAppendingPathComponent:fileName];
    return ([data writeToFile:appFile atomically:YES]);
}

- (NSData *)applicationDataFromFile:(NSString *)fileName {
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *appFile = [documentsDirectory stringByAppendingPathComponent:fileName];
    NSData *myData = [[[NSData alloc] initWithContentsOfFile:appFile] autorelease];
    return myData;
}
	
@end
