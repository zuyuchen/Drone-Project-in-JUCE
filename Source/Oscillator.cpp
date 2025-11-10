/*
  ==============================================================================

    Oscillator.cpp
    Abstract bass class (Oscillator) and subclasses for sawOSC, squareOSC and triangleOSC
    Created: 13 Feb 2025 10:44:00am
    Author:  chenzuyu

  ==============================================================================
*/

#include <iostream>
#include <vector>
#include <ctime>
#include "Oscillator.h"

/**
 Osillator class
 @class Osillator
 @brief osillator class
 */

Oscillator::Oscillator()
: sampleRate(48000), gain(1) {}  // Constructor, set to the default gain
    
Oscillator::~Oscillator(){
    
} // Destructor
    
void Oscillator::setSampleRate(float sr){
    sampleRate = sr;
}
    
void Oscillator::setFrequency(float f){
    frequency = f;
}

void Oscillator::setGain(float g){
    gain = g;
}

void Oscillator::setPhase(float p){
    phase = p;
}
float sineOSC::process(){
    
    float output = sin(2*M_PI*phase);
    phaseDelta = frequency/sampleRate;
    phase += phaseDelta;
    
    if (phase > 1)
    {
        phase -= 1;
    }
    
    return gain*output;
}

// default sawOSC
float sawOSC::process(){
    float scaling_factor = atanh(0.98);
    float output = tanh(scaling_factor*2*(phase - 0.5));
    
    // phase increment
    phaseDelta = frequency/sampleRate;
    phase += phaseDelta;
    
    //phase wrapping
    if (phase > 1)
    {
        phase -= 1;
    }
    
    return gain*output;
}

// exponential sawOSC
float sawOSC::process(bool exponential){
    
    float scaling_factor = atanh(0.98);
    float output;
    float exp_phase;
    if (exponential)
    {
        // Transform phase into an exponential curve
        exp_phase = pow(2.7, phase) - 1;  // Exponential shaping
        output = tanh(scaling_factor*2*(exp_phase - 0.5));
    }
    else
    {
        output = sawOSC::process();
    }
    
//    float output = 2*(phase-0.5);
    
    // phase increment
    phaseDelta = frequency/sampleRate;
    phase += phaseDelta;
    
    //phase wrapping
    if (phase > 1)
    {
        phase -= 1;
    }
    
    return gain*output;
}

float squareOSC::process(){
    
    float output;
    // harnessing soft clipping funciton tanh() to emulate square wave without causing aliasing
    float scaling_factor = 10;
    
    sine.setFrequency(frequency);
    sine.setSampleRate(sampleRate);
    output = tanh(scaling_factor*sine.process());
//    if (phase < 0.5)
//    {
//        output = -1;
//    }
//    else
//    {
//        output = 1;
//    }
    // update the phase
    phaseDelta = frequency/sampleRate;
    phase += phaseDelta;
    
    // phase wrapup
    if (phase > 1)
    {
        phase -= 1;
    }
    
    return gain*output;
}

float triangleOSC::process(){
    
    float output;
    
    saw.setFrequency(frequency);
    saw.setSampleRate(sampleRate);
    // Generate the triangle wave using the sawtooth wave
    if (phase < 0.5)
    {
        output = -saw.process();  // Invert the sawtooth to form one half of the triangle
    }
    else
    {
        output = saw.process();  // Direct sawtooth for the other half of the triangle
    }

    // Apply a smooth scaling to normalize the wave
    output = 2 * (output - 0.5);  // Scale to range [-1, 1] from [0, 1]

    // Update the phase increment
    phaseDelta = frequency / sampleRate;
    phase += phaseDelta;
    
    if (phase > 1)
    {
        phase -= 1;  // Keep the phase in the [0, 1) range
    }

    // You can apply a soft gain if you want to control the wave's shape more gently
    // output = tanh(input_gain * output);  // Uncomment if you want a soft non-linearity

    return gain*output;  // Direct triangle wave without excessive warping
}


