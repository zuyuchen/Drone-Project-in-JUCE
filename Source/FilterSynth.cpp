/*
  ==============================================================================

    FilterSynth.cpp
    Class for a subtractive synthesizer with an LFO-modulated filter cutoff.
    Created: 26 Feb 2025 9:35:17pm
    Author:  chenzuyu

  ==============================================================================
*/

#include "FilterSynth.h"

FilterSynth::FilterSynth()
: filterType(FilterType::LowPass), cutoff(10000.0f), resonance(0.7f) {} // Set the default parameters

FilterSynth::~FilterSynth() {};

void FilterSynth::setSampleRate(float sr) {
    sampleRate = sr;
    setFilterCoeff(cutoff); // Update filter coefficients when sampleRate is changed
    }
    

void FilterSynth::setOSC(OscType oscType, float freq, float phase) {
    switch (oscType)
    {
        case OscType::Saw:
            activeOsc = &Saw; // assign the pointer to the declared Saw object
            break;
        case OscType::Square:
            activeOsc = &Square; // assign the pointer to the declared Square object
            break;
        case OscType::Triangle:
            activeOsc = &Triangle;
            break;
    }
    activeOsc -> setFrequency(freq);
    activeOsc -> setPhase(phase);
    activeOsc -> setSampleRate(sampleRate);
}

void FilterSynth::setLFO(LFOType _lfoType, float rate, float depth, float phase) {
    lfoType = _lfoType;
    switch (_lfoType)
    {
        case LFOType::Sine:
            activeLFO = &LFOSine;
            break;
        case LFOType::Saw:
            activeLFO = &LFOSaw;
            break;
        case LFOType::Square:
            activeLFO = &LFOSquare;
            break;
        case LFOType::Triangle:
            activeLFO = &LFOTriangle;
            break;
    }
    
    activeLFO -> setFrequency(rate);
    activeLFO -> setPhase(phase);
    activeLFO -> setGain(depth);
    activeLFO -> setSampleRate(sampleRate);
};

void FilterSynth::setFilter(FilterType _filterType, float fc, float _resonance) {
    filterType = _filterType;
    cutoff = fc;
    resonance = _resonance;
    setFilterCoeff(cutoff); // Update the filter coefficients when the filter setup is changed
};

void FilterSynth::setFilterCoeff(float modCutoff) {
    switch (filterType) {
        case FilterType::LowPass:
            filter.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, modCutoff, resonance));
            break;
        case FilterType::HighPass:
            filter.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, modCutoff, resonance));
            break;
        case FilterType::BandPass:
            filter.setCoefficients(juce::IIRCoefficients::makeBandPass(sampleRate, modCutoff, resonance));
            break;
        case FilterType::AllPass:
            filter.setCoefficients(juce::IIRCoefficients::makeAllPass(sampleRate, modCutoff, resonance));
            break;
            
    }
};
    
float FilterSynth::process(bool expLFO) {
    
    if (activeOsc == nullptr || activeLFO == nullptr)  // Ensure both are selected
         return 0.0f;
    
    // Generate oscillator and LFO samples (single)
    float oscSample = activeOsc -> process(); // dereferences the pointer and accesses the method
    
    float lfoSample;
    // if LFO is a Saw waveform, then do exponential rampping
    if (lfoType == LFOType::Saw) {
        sawOSC* sawLFO = dynamic_cast<sawOSC*>(activeLFO);
        if (sawLFO) {
            lfoSample = sawLFO -> process(expLFO);
        }
    }
    lfoSample = activeLFO -> process();
    
    // Modulate filter cutoff with LFO
    float modCutoff = juce::jlimit(20.0f, sampleRate/2.0f, cutoff + lfoSample);
   
    setFilterCoeff(modCutoff);
    
    // Filter the current audio sample
    return filter.processSingleSampleRaw(oscSample);
    
}
    
