/*
  ==============================================================================

    Oscillator.h
    Created: 12 Feb 2025 9:10:23pm
    Author:  chenzuyu

  ==============================================================================
*/

#pragma once  // Ensures the file is included only once

#include <vector>
#include <ctime>

// Base class
class Oscillator{
    
public:
    Oscillator(); // Constructor
    
    virtual ~Oscillator(); // Destructor
    
    void setSampleRate(float sr);
    void setFrequency(float f);
    void setGain(float g);
    void setPhase(float p);
    
    virtual float process() = 0; // Virtual process() method to override
    
protected:
    float frequency;
    float sampleRate;
    float phase = 0.0f;
    float phaseDelta;
    float gain;
    
};

// Sub classes
class sineOSC : public Oscillator {
public:
    float process() override;
};


class sawOSC : public Oscillator {
public:
    // defaut sawOSC class
    float process() override;
    // with exponential mapping
    float process(bool exponential);      // New overload for exponential
};

class squareOSC : public Oscillator {
    
public:
    float process() override;
private:
    sineOSC sine;
    
};

class triangleOSC : public Oscillator {
public:
    float process() override;
private:
    sawOSC saw;
};

    



