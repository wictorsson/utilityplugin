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
    const float ratio = 4.0/ 3.0;
    setResizeLimits (470,  juce::roundToInt (470.0 / ratio),
                         650, juce::roundToInt (650.0 / ratio));
    getConstrainer()->setFixedAspectRatio (ratio);
    setSize (550, 550/ratio);
    
    addAndMakeVisible(hMeterLeft);
    addAndMakeVisible(hMeterRight);
    startTimerHz(24);
    
    addAndMakeVisible(gainDial);
    gainLabel.setText("GAIN", juce::dontSendNotification);
    gainLabel.attachToComponent(&gainDial, false);
    gainLabel.setJustificationType(juce::Justification::centred);
    gainDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainDial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 36);
    gainDial.setLookAndFeel(&customDialLAF);
    gainDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "gain", gainDial);
    
    addAndMakeVisible(panDial);
    panLabel.setText("BALANCE", juce::dontSendNotification);
    panLabel.attachToComponent(&panDial, false);
    panLabel.setJustificationType(juce::Justification::centred);
    panDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    panDial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 36);
    panDial.setLookAndFeel(&customDialLAF);
    panDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "pan", panDial);
    
    addAndMakeVisible(lpDial);
    lpLabel.setText("LOW PASS", juce::dontSendNotification);
    lpLabel.attachToComponent(&lpDial, false);
    lpLabel.setJustificationType(juce::Justification::centred);
    lpDial.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    lpDial.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 36);
    lpDial.setSkewFactorFromMidPoint(2000.0);
    lpDial.setLookAndFeel(&customDialLAF);
    lpDialAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "lp", lpDial);
    
    addAndMakeVisible(hpDial);
    hpLabel.setText("HIGH PASS", juce::dontSendNotification);
    hpLabel.attachToComponent(&hpDial, false);
    hpLabel.setJustificationType(juce::Justification::centred);
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

    title.setText ("F.W UTILITY v1.0", juce::dontSendNotification);
    title.setJustificationType(juce::Justification::left);
    title.setColour(0x1000281, juce::Colours::whitesmoke);
    title.setFont (juce::Font (10.0f));
    title.setInterceptsMouseClicks(false, false);
    addAndMakeVisible (title);
    
}

FwUtilityPluginAudioProcessorEditor::~FwUtilityPluginAudioProcessorEditor()
{
    // avoid memory leak
    gainDial.setLookAndFeel(nullptr);
}

//==============================================================================
void FwUtilityPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
   
//    juce::Rectangle<int> background = getLocalBounds();
//    g.setGradientFill(juce::ColourGradient::vertical(juce::Colour::fromFloatRGBA(0.6f, 0.6f, 0.6f, 1.0), 0, juce::Colour::fromFloatRGBA(0.15f, 0.15f, 0.15f, 1.0), background.getBottom()));
//        g.fillRect(background);
    
    g.fillAll (juce::Colour::fromFloatRGBA (0.08f, 0.08f, 0.08f, 1.0f));
   
}

void FwUtilityPluginAudioProcessorEditor::resized()
{
    auto widthMargin = getWidth() * 0.12;
    auto heightMargin = getHeight() * 0.1;
    //auto bottomMargin = getHeight() * 0.25;
    auto dialSize = getWidth() * 0.2;
    
    auto buttonWidth = getWidth() * 0.1;
    auto buttonHeight = getHeight() * 0.05;
    auto meterHeight = getHeight() * 0.9 - heightMargin;
    auto meterWidth = getWidth() * 0.045;
    
    title.setBounds(widthMargin * 0.1, heightMargin * 0.05, 80, 30);
    
    gainDial.setBounds(getWidth() * 0.2, heightMargin * 1.4, dialSize, dialSize);
    hMeterLeft.setBounds(getWidth() * 0.5 - meterWidth, heightMargin, meterWidth, meterHeight);
    hMeterRight.setBounds(hMeterLeft.getX() + meterWidth * 1.2 , heightMargin, meterWidth, meterHeight);
    
    panDial.setBounds(getWidth() * 0.2, heightMargin + getHeight()*0.4  , dialSize, dialSize);
    
    monoButton.setBounds(getWidth() * 0.2 + buttonWidth * 0.5, heightMargin + getHeight()*0.7 , buttonWidth , buttonHeight);
    
    lpDial.setBounds(getWidth() * 0.6,  heightMargin + getHeight()*0.4, dialSize, dialSize);
    hpDial.setBounds(getWidth() * 0.6 ,     heightMargin * 1.4, dialSize, dialSize);

    //saveWindowSize();
   
}

void FwUtilityPluginAudioProcessorEditor::timerCallback()
{
    hMeterLeft.setLevel(audioProcessor.getRmsValue(0));
    hMeterRight.setLevel(audioProcessor.getRmsValue(1));
    hMeterLeft.repaint();
    hMeterRight.repaint();
}
