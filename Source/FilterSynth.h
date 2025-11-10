/*
  ==============================================================================

    FilterSynth.h
    Class for a subtractive synthesizer with an LFO-modulated filter cutoff.
    Created: 26 Feb 2025 9:02:14pm
    Author:  chenzuyu

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Oscillator.h"

enum class FilterType {
    LowPass,
    HighPass,
    BandPass,
    AllPass
};

enum class OscType {
    Saw,
    Square,
    Triangle
};

enum class LFOType {
    Sine,
    Saw,
    Square,
    Triangle
};

class FilterSynth
{
public:
    FilterSynth();
    ~FilterSynth();
    
    void setSampleRate(float sr); // set the synth's sample rate
    void setOSC(OscType oscType, float frequency, float phase); // set the oscillator type
    void setLFO(LFOType lfoType, float rate, float depth, float phase); // set LFO to modulate the cutoff frequency
    void setFilter(FilterType _filterType, float _fc, float _resonance); // set filter arguments
    void setFilterCoeff(float modCutoff); // set the filter coefficients corresponding to the arguments
    
    float process(bool expLFO); //Generate a sample
    
private:
    // Instantiate the Ocillator objects
    
    sawOSC Saw;
    squareOSC Square;
    triangleOSC Triangle;
    
    sineOSC LFOSine;
    sawOSC LFOSaw;
    squareOSC LFOSquare;
    triangleOSC LFOTriangle;
    LFOType lfoType;
//    bool expLFO;
    
    Oscillator* activeOsc = nullptr; // Pointer to the selected oscillator
    Oscillator* activeLFO = nullptr; // Pointer to the selected LFO
    
    // Set the filter and its parameters
    
    juce::IIRFilter filter;
    FilterType filterType;
    float sampleRate;
    float cutoff;
    float resonance;

    // Other objects and parameters
    
//    juce::Reverb reverb;
//    juce::Random random;

};
