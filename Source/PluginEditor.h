/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "./LookAndFeel/DialLAF.h"
#include "HLevelMeter.h"

//==============================================================================
/**
*/
class FwUtilityPluginAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    FwUtilityPluginAudioProcessorEditor (FwUtilityPluginAudioProcessor&);
    ~FwUtilityPluginAudioProcessorEditor() override;

    //==============================================================================
    void timerCallback() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FwUtilityPluginAudioProcessor& audioProcessor;
    
    Gui::HMeter hMeterLeft, hMeterRight;
    juce::Slider gainDial, panDial, lpDial, hpDial;
    juce::Label gainLabel,panLabel,lpLabel,hpLabel, title;
    juce::TextButton monoButton;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainDialAttach,panDialAttach,lpDialAttach, hpDialAttach;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>monoButtonAttach;
    
    CustomDial customDialLAF;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FwUtilityPluginAudioProcessorEditor)
};
