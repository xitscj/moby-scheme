#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "scheme.h"

#define FLOATING float
#define SAMPLE unsigned char /* NOTE THAT THE ALGORITHM WILL NOT WORK IF THE DATA PROVIDED EXCEEDS THE MAXIMUM VALUE FOR SAMPLE (AS BASED ON ITS TYPE) */

#define PI M_PI
#define THRESHOLD_COEFF_LOW 1.0 // The highest low freq power must be THRESHOLD_COEFF * the second highest low freq power
#define THRESHOLD_COEFF_HIGH 1.0 // The highest high freq power must be THRESHOLD_COEFF * the second highest high freq power

#define MAX_BINS 8
#define SAMPLING_RATE	9615.3846153846152
#define MAX_N 350 // A number > all ints in NS[]

static const FLOATING FREQUENCIES[MAX_BINS] = {697, 770, 852, 941, 1209, 1336, 1477, 1633};
static const FLOATING NS[MAX_BINS] = {331, 337, 316, 327, 342, 331, 319, 318};

static FLOATING ks[MAX_BINS];
static FLOATING est_freqs[MAX_BINS];
static FLOATING powers[MAX_BINS];

static FLOATING coeff;
static FLOATING Q1;
static FLOATING Q2;
static FLOATING sine;
static FLOATING cosine;

/* Call this routine before every "block" (size=N) of samples. */
static void ResetGoertzel()
{
  Q2 = 0;
  Q1 = 0;
}

static void setUpConstants()
{
  int i;
  for( i = 0; i < MAX_BINS; i++) {
    ks[i] = (int)(0.5 + (NS[i]*FREQUENCIES[i]/SAMPLING_RATE));
    est_freqs[i] = (float)((int)(0.5 + (((float)ks[i])*SAMPLING_RATE/NS[i])));
  }
  ResetGoertzel();
}

/* Call this each time a frequency is tested, to precompute the constants. */
static void InitGoertzel(const int i)
{
  FLOATING	floatN;
  FLOATING	omega;
  int k;

  floatN = (FLOATING) NS[i];
  k = (int) (ks[i]);
  omega = (2.0 * PI * k) / floatN;
  sine = sin(omega);
  cosine = cos(omega);
  coeff = 2.0 * cosine;

  ResetGoertzel();
}

/* Call this routine for every sample. */
static void ProcessSample(const SAMPLE sample)
{
  FLOATING Q0;
  Q0 = coeff * Q1 - Q2 + (FLOATING) sample;
  Q2 = Q1;
  Q1 = Q0;
}


/* Basic Goertzel */

/* Optimized Goertzel */
/* Call this after every block to get the RELATIVE magnitude squared. */
static FLOATING GetMagnitudeSquared(void)
{
  return (FLOATING)(Q1 * Q1 + Q2 * Q2 - Q1 * Q2 * coeff);
}

/* Test the given data */
static FLOATING Test(int i, const SAMPLE *input)
{
  int	index;

  FLOATING	magnitudeSquared;
  FLOATING	magnitude;
  FLOATING	real;
  FLOATING	imag;

  /* Process the samples. */
  for (index = 0; index < NS[i]; index++)
  {
    ProcessSample(input[index]);  // instead of processing the testData, process the actual sound file so long as the constants agree (no need to generate)
  }

  /* Do the "standard Goertzel" processing. */
  real = (Q1 - Q2 * cosine);
  imag = (Q2 * sine);
  magnitudeSquared = real*real + imag*imag;
  magnitude = sqrt(magnitudeSquared);
  return magnitude;
  ResetGoertzel();
}

// returns -1 if no conclusive data could be produced, returns 0 - 15 depending on which tone is heard (row 1 of DTMF tones contains 0 - 3, etc.)
static int determine(const SAMPLE *input) // the input[] should be of length MAX_N
{
  int i;
  int max_Low_Power_Index;
  int max_High_Power_Index;
  int second_To_mLPI;
  int second_To_mHPI;

  setUpConstants();
  for( i = 0; i < MAX_BINS; i++) {
    InitGoertzel(i);
    powers[i] = Test( i, input);
  }

  // Test to conclude winners for each half of frequencies
  max_Low_Power_Index= 0;
  max_High_Power_Index = MAX_BINS/2;
  second_To_mLPI = -1;
  second_To_mHPI = -1;
  for( i = 1; i < (MAX_BINS/2); i++) {
    if( powers[max_Low_Power_Index] <= powers[i]) {
      second_To_mLPI = max_Low_Power_Index;
      max_Low_Power_Index = i;
    }
  }
  for( i = (MAX_BINS/2 + 1); i < MAX_BINS; i++) {
    if( powers[max_High_Power_Index] <= powers[i]) {
      second_To_mHPI = max_High_Power_Index;
      max_High_Power_Index = i;
    }
  }
  if(powers[max_Low_Power_Index] < powers[second_To_mLPI] * THRESHOLD_COEFF_LOW || powers[max_High_Power_Index] < powers[second_To_mHPI] * THRESHOLD_COEFF_HIGH) {
    return -1;
  } else {
    return 4*max_Low_Power_Index + (max_High_Power_Index - 4);
  }
}

/* THIS SHOULD BE THE ONLY NON-STATIC METHOD IN THE FINAL VERSION */
FUN(goertzel) {
  START();
  int ret;
  SAMPLE *input = sb_val(ARG(0)) -> data; // is this the proper way to get the pointer to the front of the array

/* THIS IS ONLY FOR TESTING */
/*  int input = integer_val(ARG(0));  // this is a filler for testing -- remove it later
  int i;
  float randnums[MAX_N];  
  
  unsigned char testData[MAX_N];
  float arand;
  for( i = 0; i < MAX_N; i++) {
    arand = (float)(1.0*((((double)random())/((double)(RAND_MAX)+(double)(1)))));
    randnums[i] = arand;
  }

  float step = 1633.0 * ((2.0 * PI) / SAMPLING_RATE);
  float step2 = 941.0 * ((2.0 * PI) / SAMPLING_RATE);

  for (i = 0; i < MAX_N; i++)
  {
    testData[i] = (SAMPLE) (40.0*(1.0 * (0.5*sin(i * step) + 0.5*sin(i * step2)) + randnums[i] + 1.0));
  }*/
  // END TESTING 



  
 ret = determine(input); // testData should be changed to input when everything is running properly
    
  RETURN(alloc_integer(ret)); // should be ret 
}

