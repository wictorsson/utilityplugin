/*
  ==============================================================================

    DialLAF.h
    Created: 23 May 2022 3:42:53pm
    Author:  Fredrik Wictorsson

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class CustomDial : public juce::LookAndFeel_V4
{
public:
    CustomDial();
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;
    
    juce::Label* createSliderTextBox (juce::Slider& slider) override;
private:
    
    
    juce::DropShadow shadowProperties;
    juce::DropShadowEffect dialShadow;
};
