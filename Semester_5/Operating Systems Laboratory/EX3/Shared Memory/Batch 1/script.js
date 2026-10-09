// ==========================================
// SIH 2026 - RISHIGANGA FLOOD ANALYSIS
// MEMBER 3 - GOOGLE EARTH ENGINE
// ==========================================

// Our study region in Uttarakhand
var studyArea = ee.Geometry.Rectangle([
  79.58, 30.44,
  79.73, 30.54
]);

// Move map to our study region
Map.centerObject(studyArea, 12);

// Display study-area boundary
Map.addLayer(studyArea, {}, 'Rishiganga Study Area');

print('Study Area:', studyArea);

// ==========================================
// STEP 2 - LOAD SENTINEL-1 RADAR DATA
// ==========================================

// Load Sentinel-1 image collection
var sentinel1 = ee.ImageCollection('COPERNICUS/S1_GRD')
  .filterBounds(studyArea)
  .filter(ee.Filter.eq('instrumentMode', 'IW'))
  .filter(ee.Filter.listContains(
    'transmitterReceiverPolarisation',
    'VV'
  ))
  .select('VV');

// BEFORE the 7 Feb 2021 flood
var beforeCollection = sentinel1
  .filterDate('2021-01-20', '2021-02-07');

// AFTER the flood
var afterCollection = sentinel1
  .filterDate('2021-02-08', '2021-02-25');

// Print how many satellite images we found
print('Before image count:', beforeCollection.size());
print('After image count:', afterCollection.size());

// Combine images from each period
var before = beforeCollection
  .median()
  .clip(studyArea);

var after = afterCollection
  .median()
  .clip(studyArea);

// Show satellite images
Map.addLayer(
  before,
  {min: -25, max: 5},
  'Before Flood - Sentinel 1'
);

Map.addLayer(
  after,
  {min: -25, max: 5},
  'After Flood - Sentinel 1'
);

// ==========================================
// STEP 3 - CHANGE DETECTION
// ==========================================

// Reduce radar noise slightly
var beforeSmooth = before.focal_mean(50, 'circle', 'meters');
var afterSmooth  = after.focal_mean(50, 'circle', 'meters');

// Calculate the difference between before and after
var change = afterSmooth.subtract(beforeSmooth);

// Display the change layer
Map.addLayer(
  change,
  {min: -5, max: 5},
  'Radar Change'
);

// Strong decrease in radar return
// This can indicate newly appearing water,
// but we will filter it further in the next step.
var possibleFlood = change.lt(-1.5);

// Hide non-detected pixels
possibleFlood = possibleFlood.selfMask();

// Display possible flood/change regions
Map.addLayer(
  possibleFlood,
  {palette: ['blue']},
  'Possible Flood Areas'
);

// Print result
print('Change Detection:', change);

// ==========================================
// STEP 4 - TERRAIN / SLOPE FILTER
// ==========================================

// Load elevation data
var dem = ee.Image('USGS/SRTMGL1_003')
  .clip(studyArea);

// Calculate slope from elevation
var slope = ee.Terrain.slope(dem);

// Show elevation and slope for checking
Map.addLayer(
  dem,
  {min: 1500, max: 6000},
  'Elevation',
  false
);

Map.addLayer(
  slope,
  {min: 0, max: 60},
  'Slope',
  false
);

// Keep only areas with relatively lower slope
// We start with < 20 degrees.
// We may tune this later.
var lowSlopeMask = slope.lt(40);

// Apply slope filter to our possible flood layer
var terrainFilteredFlood = possibleFlood
  .updateMask(lowSlopeMask)
  .selfMask();

// Show the filtered result
Map.addLayer(
  terrainFilteredFlood,
  {palette: ['cyan']},
  'Terrain Filtered Flood'
);

print('Slope image:', slope);

// ==========================================
// STEP 5 - REMOVE SMALL / ISOLATED NOISE
// ==========================================

// Count how many neighbouring detected pixels
// are connected to each detected pixel.
var connectedPixels = terrainFilteredFlood
  .connectedPixelCount(100, true);

// Keep only groups containing at least 8
// connected pixels.
var cleanedFlood = terrainFilteredFlood
  .updateMask(connectedPixels.gte(8))
  .selfMask();

// Display cleaned result
Map.addLayer(
  cleanedFlood,
  {palette: ['red']},
  'Cleaned Candidate Flood'
);

print('Connected pixel count:', connectedPixels);

// ==========================================
// STEP 6 - CHECK SENTINEL-1 ACQUISITION DATES
// ==========================================

// Get all Sentinel-1 images around the event
var eventImages = sentinel1
  .filterDate('2021-01-25', '2021-02-20')
  .sort('system:time_start');

// Convert image dates to a readable list
var dateList = eventImages.aggregate_array('system:time_start')
  .map(function(date) {
    return ee.Date(date).format('YYYY-MM-dd HH:mm');
  });

print('Sentinel-1 dates around event:', dateList);

// ==========================================
// STEP 7 - CHECK CLOSEST IMAGE ORBITS
// ==========================================

var closeImages = sentinel1
  .filterDate('2021-02-05', '2021-02-12')
  .sort('system:time_start');

var orbitInfo = closeImages.map(function(image) {
  return ee.Feature(null, {
    date: image.date().format('YYYY-MM-dd HH:mm'),
    orbitPass: image.get('orbitProperties_pass'),
    relativeOrbit: image.get('relativeOrbitNumber_start')
  });
});

print('CLOSEST IMAGE ORBIT DETAILS:', orbitInfo);

// ==========================================
// STEP 8 - FIND MATCHING BEFORE/AFTER PAIR
// ==========================================

// Same geometry = much safer comparison
var orbit165 = sentinel1
  .filter(ee.Filter.eq('orbitProperties_pass', 'DESCENDING'))
  .filter(ee.Filter.eq('relativeOrbitNumber_start', 165));

// Closest suitable BEFORE event
var beforePair = orbit165
  .filterDate('2021-01-20', '2021-02-07')
  .sort('system:time_start', false)
  .first();

// Closest suitable AFTER event
var afterPair = orbit165
  .filterDate('2021-02-08', '2021-02-25')
  .sort('system:time_start')
  .first();

print(
  'SELECTED BEFORE:',
  ee.Date(beforePair.get('system:time_start')).format('YYYY-MM-dd HH:mm')
);

print(
  'SELECTED AFTER:',
  ee.Date(afterPair.get('system:time_start')).format('YYYY-MM-dd HH:mm')
);

print(
  'Before orbit:',
  beforePair.get('relativeOrbitNumber_start'),
  beforePair.get('orbitProperties_pass')
);

print(
  'After orbit:',
  afterPair.get('relativeOrbitNumber_start'),
  afterPair.get('orbitProperties_pass')
);

// ==========================================
// STEP 9 - MATCHED-ORBIT CHANGE DETECTION
// ==========================================

// Extract VV band from our selected images
var beforeMatched = ee.Image(beforePair)
  .select('VV')
  .clip(studyArea);

var afterMatched = ee.Image(afterPair)
  .select('VV')
  .clip(studyArea);

// Reduce radar speckle/noise
var beforeSmooth = beforeMatched.focal_mean(30, 'circle', 'meters');
var afterSmooth  = afterMatched.focal_mean(30, 'circle', 'meters');

// Because Sentinel-1 GRD values here are in dB,
// subtract BEFORE from AFTER.
var matchedChange = afterSmooth.subtract(beforeSmooth);

// Display the two matched observations
Map.addLayer(
  beforeSmooth,
  {min: -25, max: 5},
  'MATCHED Before - Orbit 165',
  false
);

Map.addLayer(
  afterSmooth,
  {min: -25, max: 5},
  'MATCHED After - Orbit 165',
  false
);

// Display change itself
Map.addLayer(
  matchedChange,
  {
    min: -6,
    max: 6,
    palette: ['blue', 'white', 'red']
  },
  'MATCHED Radar Change',
  false
);

// Initial candidate:
// strong decrease in VV backscatter
var matchedCandidate = matchedChange.lt(-1.5);

// Apply our existing terrain filter
matchedCandidate = matchedCandidate
  .updateMask(lowSlopeMask)
  .selfMask();
// DEBUG: show candidates BEFORE connected-pixel cleanup
Map.addLayer(
  matchedCandidate.selfMask(),
  {palette: ['magenta']},
  'DEBUG Raw Matched Candidate'
);

// Remove small isolated detections
var matchedConnected = matchedCandidate
  .connectedPixelCount(100, true);

var matchedCleaned = matchedCandidate
  .updateMask(matchedConnected.gte(3))
  .selfMask();

// Final display
Map.addLayer(
  matchedCleaned,
  {palette: ['yellow']},
  'MATCHED Event Candidates'
);

print('Matched change image:', matchedChange);


// ==========================================
// STEP 10 - FIND WHICH FILTER IS TOO STRICT
// ==========================================

// A. Radar-change threshold ONLY
// No slope filter
var thresholdOnly = matchedChange
  .lt(-1.5)
  .selfMask();

Map.addLayer(
  thresholdOnly,
  {palette: ['orange']},
  'DEBUG Threshold Only',
  false
);

// B. Slightly relaxed radar threshold
// Still no slope filter
var relaxedThreshold = matchedChange
  .lt(-1.0)
  .selfMask();

Map.addLayer(
  relaxedThreshold,
  {palette: ['purple']},
  'DEBUG Relaxed Threshold',
  false
);

// C. Relaxed threshold + current slope filter
var relaxedWithSlope = matchedChange
  .lt(-1.0)
  .updateMask(lowSlopeMask)
  .selfMask();

Map.addLayer(
  relaxedWithSlope,
  {palette: ['lime']},
  'DEBUG Relaxed + Slope',
  false
);

// ==========================================
// STEP 11 - INSPECT RADAR CHANGE STATISTICS
// ==========================================

var changeStats = matchedChange.reduceRegion({
  reducer: ee.Reducer.minMax()
    .combine({
      reducer2: ee.Reducer.mean(),
      sharedInputs: true
    })
    .combine({
      reducer2: ee.Reducer.percentile([1, 5, 10, 25, 50, 75, 90, 95, 99]),
      sharedInputs: true
    }),
  geometry: studyArea,
  scale: 10,
  bestEffort: true,
  maxPixels: 1e8
});

print('MATCHED CHANGE STATISTICS:', changeStats);


// ==========================================
// STEP 12 - FILTER SURVIVAL DIAGNOSTIC
// ==========================================

// 1. Pixels passing radar threshold
var radarCandidates = matchedChange.lt(-1.5);

// 2. Pixels passing radar + slope
var slopeCandidates = radarCandidates.updateMask(lowSlopeMask);

// Count radar-threshold pixels
var radarCount = radarCandidates.selfMask().reduceRegion({
  reducer: ee.Reducer.count(),
  geometry: studyArea,
  scale: 10,
  maxPixels: 1e8
});

// Count pixels remaining after slope filtering
var slopeCount = slopeCandidates.selfMask().reduceRegion({
  reducer: ee.Reducer.count(),
  geometry: studyArea,
  scale: 10,
  maxPixels: 1e8
});

// Also inspect slope distribution
var slopeStats = slope.reduceRegion({
  reducer: ee.Reducer.percentile([10, 25, 50, 75, 90])
    .combine({
      reducer2: ee.Reducer.mean(),
      sharedInputs: true
    }),
  geometry: studyArea,
  scale: 30,
  bestEffort: true,
  maxPixels: 1e8
});

print('RADAR CANDIDATE PIXELS:', radarCount);
print('AFTER SLOPE FILTER:', slopeCount);
print('SLOPE STATISTICS:', slopeStats);