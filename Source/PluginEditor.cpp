/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
FwUtilityPluginAudioProcessorEditor::FwUtilityPluginAudioProcessorEditor (FwUtilityPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    setResizable (true, true);
    const float ratio = 3.0/ 4.0;
    setResizeLimits (300,  juce::roundToInt (300.0 / ratio),
                         650, juce::roundToInt (650.0 / ratio));
    getConstrainer()->setFixedAspectRatio (ratio);
    setSize (300, 300/ratio);
    
    addAndMakeVisible(hMeterLeft);
    addAndMakeVisible(hMeterRight);
    startTimerHz(24);
    
    addAndMakeVisible(gainDial);
    gainLabel.setText("GAIN", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainDial, false);
    gainDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainDial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 36);
    gainDial.setLookAndFeel(&customDialLAF);
    gainDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "gain", gainDial);
    
    addAndMakeVisible(panDial);
    panLabel.setText("BALANCE", juce::dontSendNotification);
    panLabel.attachToComponent(&panDial, false);
    panDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    panDial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 36);
    panDial.setLookAndFeel(&customDialLAF);
    panDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "pan", panDial);
    
    addAndMakeVisible(lpDial);
    lpLabel.setText("LOW PASS", juce::dontSendNotification);
    lpLabel.attachToComponent(&lpDial, false);
    lpDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    lpDial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 36);
    lpDial.setSkewFactorFromMidPoint(2000.0);
    lpDial.setLookAndFeel(&customDialLAF);
    lpDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "lp", lpDial);
    
    addAndMakeVisible(hpDial);
    hpLabel.setText("HIGH PASS", juce::dontSendNotification);
    hpLabel.attachToComponent(&hpDial, false);
    hpDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    hpDial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 36);
    hpDial.setSkewFactorFromMidPoint(2000.0);
    hpDial.setLookAndFeel(&customDialLAF);
    hpDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "hp", hpDial);
    
    addAndMakeVisible(monoButton);
    monoButtonAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "mono", monoButton);
    
    monoButton.setButtonText("MONO");
    
    monoButton.setClickingTogglesState(true);
    monoButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colour::fromFloatRGBA(0.9f, 0.9f, 0.9f, 1).darker(0.5f));
    monoButton.setColour(juce::TextButton::ColourIds::buttonColourId, juce::Colours::dimgrey);
    
    monoButton.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colour::fromFloatRGBA(0.9f, 0.9f, 0.9f, 0.5f));

    
    
    
    for(auto* label : {&gainLabel, &panLabel, &lpLabel, &hpLabel,&title})
    {
        addAndMakeVisible(label);
        label->setJustificationType(juce::Justification::centred);
        label->setFont (juce::Font (12.0f, juce::Font::bold));
    }
    
    title.setText ("F.W UTILITY v1.0", juce::dontSendNotification);
    title.setColour(0x1000281, juce::Colours::whitesmoke);
    title.setInterceptsMouseClicks(false, false);
    title.setJustificationType(juce::Justification::topLeft);
    title.setFont (juce::Font (10.0f));
    title.setInterceptsMouseClicks(false, false);
    
}

FwUtilityPluginAudioProcessorEditor::~FwUtilityPluginAudioProcessorEditor()
{
    // avoid memory leak
    gainDial.setLookAndFeel(nullptr);
}

//==============================================================================
void FwUtilityPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    
    g.fillAll (juce::Colour::fromFloatRGBA (0.08f, 0.08f, 0.08f, 1.0f));
   
}

void FwUtilityPluginAudioProcessorEditor::resized()
{
    juce::Rectangle<int> bounds = getLocalBounds();

    title.setBounds(bounds);
    int margin = bounds.getWidth()/20;
    juce::Rectangle<int> metersBounds = bounds.removeFromRight(getWidth()/4);
    juce::Rectangle<int> meterBounds = metersBounds.removeFromRight(metersBounds.getWidth()/2);
    hMeterRight.setBounds(meterBounds.reduced(margin));
    hMeterLeft.setBounds(hMeterRight.getX() - hMeterRight.getWidth() * 1.2 , hMeterRight.getY(), hMeterRight.getWidth(), hMeterRight.getHeight());
    
    juce::Rectangle<int> leftBounds = bounds.removeFromLeft(getWidth()/4).removeFromTop(getHeight()/4);
    gainDial.setBounds(leftBounds.getX() + leftBounds.getWidth()/1.5f ,leftBounds.getY() + leftBounds.getHeight()/2.5f, leftBounds.getWidth()*1.5f,leftBounds.getHeight()*1.5f);
   
    panDial.setBounds(gainDial.getX() + gainDial.getWidth(), gainDial.getY() + gainDial.getHeight()/6,gainDial.getWidth()/1.5f,gainDial.getHeight()/1.5f);
    
    
    hpDial.setBounds(gainDial.getX() + gainDial.getWidth()/6, panDial.getY() + gainDial.getHeight()*1.2f ,panDial.getWidth(),panDial.getHeight());
    
    lpDial.setBounds(panDial.getX() , hpDial.getY() ,panDial.getWidth(),panDial.getHeight());
}

void FwUtilityPluginAudioProcessorEditor::timerCallback()
{
    hMeterLeft.setLevel(audioProcessor.getRmsValue(0));
    hMeterRight.setLevel(audioProcessor.getRmsValue(1));
    hMeterLeft.repaint();
    hMeterRight.repaint();
}
