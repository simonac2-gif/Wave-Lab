//==============================================================================

#include "MainComponent.h"
#include "MainApplication.h"

MainComponent::MainComponent()
: deviceManager (MainApplication::getApp().audioDeviceManager), audioVisualizer(2) {
    
    
    setVisible(true);
    
    addAndMakeVisible(settingsButton);
    addAndMakeVisible(playButton);
    addAndMakeVisible(levelLabel);
    addAndMakeVisible(levelSlider);
    addAndMakeVisible(freqLabel);
    addAndMakeVisible(freqSlider);
    addAndMakeVisible(waveformLabel);
    addAndMakeVisible(waveformMenu);
    addAndMakeVisible(cpuLabel);
    addAndMakeVisible(cpuUsage);
   
    settingsButton.addListener(this);
    playButton.addListener(this);
    levelSlider.addListener(this);
    freqSlider.addListener(this);
    waveformMenu.setTextWhenNothingSelected("Waveforms");
    StringArray white_noise = {"White", "Brown", "Dust"};
    StringArray sin_wave = {"Sine"};
    StringArray lf_impulse_wave = {"LF Impulse", "LF Square", "LF Saw", "LF Triangle"};
    StringArray bl_impulse_wave = {"BL Impulse", "BL Square", "BL Saw", "BL Triangle"};
    StringArray wt_sine_wave = {"WT Sine", "WT Impulse", "WT Square", "WT Saw", "WT Triangle" };
    
    waveformMenu.addItemList(white_noise, WhiteNoise);
    waveformMenu.addSeparator();
    drawPlayButton(playButton, true);
    waveformMenu.addItemList(sin_wave, SineWave);
    waveformMenu.addSeparator();
    waveformMenu.addItemList(lf_impulse_wave, LF_ImpulseWave);
    waveformMenu.addSeparator();
    waveformMenu.addItemList(bl_impulse_wave, BL_ImpulseWave);
    waveformMenu.addSeparator();
    waveformMenu.addItemList(wt_sine_wave, WT_SineWave);
    waveformMenu.addSeparator();
    waveformMenu.addListener(this);
    addAndMakeVisible(audioVisualizer);
   
    
    
    
    settingsButton.setButtonText("Audio Settings...");
    
    waveformId = Empty;
    levelSlider.setRange(0.0, 1.0);
    levelSlider.setSliderStyle(Slider::LinearHorizontal);
    levelSlider.setTextBoxStyle(Slider::TextBoxLeft, true, 90, 22);
    freqSlider.setRange(0.0, 5000.0);
    freqSlider.setSliderStyle(Slider::LinearHorizontal);
    freqSlider.setTextBoxStyle(Slider::TextBoxLeft, true, 90, 22);
    
//    drawPlayButton(playButton, false);
    cpuLabel.setJustificationType(Justification::centredRight);
    
    deviceManager.addAudioCallback(&audioSourcePlayer);
    audioSourcePlayer.setSource(this);
    
    
    
    
    
}

MainComponent::~MainComponent() {
    audioSourcePlayer.setSource(nullptr);
    deviceManager.removeAudioCallback(&audioSourcePlayer);
    deviceManager.closeAudioDevice();
   
}

//==============================================================================
// Component overrides
//==============================================================================

void MainComponent::paint (Graphics& g) {
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

void MainComponent::resized() {
    Rectangle<int> boundaries = getLocalBounds();
       boundaries.reduce(8,8);
       Rectangle<int> first_line = boundaries.removeFromTop(24);
       Rectangle<int> second_line = boundaries.removeFromTop(32);

       settingsButton.setBounds(first_line.removeFromLeft(118));
    first_line.removeFromLeft(8);
       playButton.setBounds(first_line.removeFromLeft(56));
       playButton.setSize(56, 56);

    second_line.removeFromTop(8);
       waveformMenu.setBounds(second_line.removeFromLeft(118));

       levelLabel.setBounds(first_line.removeFromLeft(72));
       levelSlider.setBounds(first_line);

       //to account for playbutton width
    second_line.removeFromLeft(8 + 56);
    second_line.removeFromLeft(8);
       freqLabel.setBounds(second_line.removeFromLeft(72));
       freqSlider.setBounds(second_line);

       Rectangle<int> wave_screen = boundaries.withTrimmedBottom(24).withTrimmedTop(8);
       audioVisualizer.setBounds(wave_screen);
       Rectangle<int> ofset_below = boundaries.removeFromBottom(24);
       ofset_below.removeFromRight(8);

}

void MainComponent::drawPlayButton(juce::DrawableButton& button, bool showPlay) {
  juce::Path path;
    
  if (showPlay) {

      path.addTriangle(0, 0, 0, 100, 86.6, 50);
    // draw the triangle
  }
  else {
      path.clear();
      path.addRectangle(0, 0, 42, 100);
      path.addRectangle(100, 0, 42, 100);
    // draw the two bars
  }
  juce::DrawablePath drawable;
  drawable.setPath(path);
  juce::FillType fill (Colours::white);
  drawable.setFill(fill);
  button.setImages(&drawable);
}

//==============================================================================
// Listener overrides
//==============================================================================

void MainComponent::buttonClicked (Button *button) {
    std::cout <<"In MainComponent, Button Clicked" << std::endl;
    if (button == &playButton) {
        if (isPlaying()) {
            audioSourcePlayer.setSource(nullptr);
            drawPlayButton(playButton, isPlaying());
        } else {
            audioSourcePlayer.setSource(this);
            drawPlayButton(playButton, isPlaying());
        }
       
           
    } else if (button == &settingsButton) {
        openAudioSettings();
    }
        
    }
    


void MainComponent::sliderValueChanged (Slider *slider) {
    
    if (slider == &freqSlider) {
        freq = freqSlider.getValue();
        phaseDelta = freq /srate;
    
       
    }
    if (slider == &levelSlider) {
        level = levelSlider.getValue();
        
    }
}

    void MainComponent::comboBoxChanged (ComboBox *menu) {
        waveformId = static_cast<MainComponent::WaveformId>(menu->getSelectedId());
        if (menu == &waveformMenu) {
            if (waveformMenu.getSelectedIdAsValue() == WhiteNoise || waveformMenu.getSelectedIdAsValue() == BrownNoise) {
                freqSlider.setEnabled(false);
                
            } else {
                freqSlider.setEnabled(true);
            }
        }
    }
//==============================================================================
// Timer overrides
//==============================================================================

void MainComponent::timerCallback() {
    
    cpuUsage.setText(String(deviceManager.getCpuUsage() * 100, 2, false) + " %", dontSendNotification);
}

//==============================================================================
// AudioSource overrides
//==============================================================================

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate) {
    srate = sampleRate;
    phase = 0;
    phaseDelta = TwoPi * (freq / srate);
    audioVisualizer.setBufferSize(samplesPerBlockExpected);
    audioVisualizer.setSamplesPerBlock(samplesPerBlockExpected);
//    for (auto & index:oscillators) {
//        index->setFrequency(freq, srate);
//     }
}

void MainComponent::releaseResources() {
}

void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) {
  bufferToFill.clearActiveBufferRegion();
  switch (waveformId) {
    case WhiteNoise:      whiteNoise(bufferToFill);   break;
    case DustNoise:       dust(bufferToFill);         break;
    case BrownNoise:      brownNoise(bufferToFill);   break;
    case SineWave:        sineWave(bufferToFill);     break;
    case LF_ImpulseWave:  LF_impulseWave(bufferToFill);  break;
    case LF_SquareWave:   LF_squareWave(bufferToFill);   break;
    case LF_SawtoothWave: LF_sawtoothWave(bufferToFill); break;
    case LF_TriangeWave:  LF_triangleWave(bufferToFill); break;
    case BL_ImpulseWave:  BL_impulseWave(bufferToFill);  break;
    case BL_SquareWave:   BL_squareWave(bufferToFill);   break;
    case BL_SawtoothWave: BL_sawtoothWave(bufferToFill); break;
    case BL_TriangeWave:  BL_triangleWave(bufferToFill); break;
    case WT_SineWave:
    case WT_ImpulseWave:
    case WT_SquareWave:
    case WT_SawtoothWave:
    case WT_TriangleWave:
      WT_wave(bufferToFill);
      break;
    case Empty:
      break;
  }
  audioVisualizer.pushBuffer(bufferToFill);
}

//==============================================================================
// Audio Utilities
//==============================================================================

double MainComponent::phasor() {
    return fmod(phase + phaseDelta, 1);
}

float MainComponent::ranSamp() {
    return (random.Random::nextFloat()) *2 - 1;
    
}

float MainComponent::ranSamp(const float mul) {
    
  return (ranSamp() * mul);
}

    float MainComponent::lowPass(const float value, const float prevout, const float alpha) {
        return  (prevout + alpha * (value - prevout));
    }

bool MainComponent::isPlaying() {
    return audioSourcePlayer.getCurrentSource() != nullptr;
}

void MainComponent::openAudioSettings() {
    auto object = std::make_unique<juce::AudioDeviceSelectorComponent>(deviceManager,0, 2, 0, 2, true, false, true, false);
    object->setSize(500,270);
    DialogWindow::LaunchOptions dw;
    dw.useNativeTitleBar = true;
    dw.resizable = false;
    dw.dialogTitle = "Audio Settings";
    dw.dialogBackgroundColour= Colours::black;
    dw.content.setOwned(object.release());
    dw.launchAsync();
}

void MainComponent::createWaveTables() {
  createSineTable(sineTable);
  oscillators.push_back(std::make_unique<WavetableOscillator>(sineTable));
  createImpulseTable(impulseTable);
  oscillators.push_back(std::make_unique<WavetableOscillator>(impulseTable));
  createSquareTable(squareTable);
  oscillators.push_back(std::make_unique<WavetableOscillator>(squareTable));
  createSawtoothTable(sawtoothTable);
  oscillators.push_back(std::make_unique<WavetableOscillator>(sawtoothTable));
  createTriangleTable(triangleTable);
  oscillators.push_back(std::make_unique<WavetableOscillator>(triangleTable));
}

//==============================================================================
// Noise
//==============================================================================

// White Noise

void MainComponent::whiteNoise (const AudioSourceChannelInfo& bufferToFill) {
    for (auto chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan) {
        float * const waves =bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);
        for (auto i= 0; i < bufferToFill.numSamples; ++i) {
            waves[i] = ranSamp(level);
        }
    }
}

// Dust

void MainComponent::dust (const AudioSourceChannelInfo& bufferToFill) {
    double odds = freq/(srate * bufferToFill.buffer->getNumChannels());
         for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan) {
            float* const channelData = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);
          
            for (int i = 0; i < bufferToFill.numSamples ; ++i) {
               if (random.nextDouble() < odds) {
                  channelData[i] = ranSamp(level);
               }
            }
         }
}

// Brown Noise

void MainComponent::brownNoise (const AudioSourceChannelInfo& bufferToFill) {
    
     for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
     {
      
       float* const channelData = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);
      
       for (int i = 0; i < bufferToFill.numSamples ; ++i)
       {
           Random r;
           auto a = r.nextFloat();
           channelData[i] = ranSamp(level);
           channelData[i] = lowPass(channelData[i], channelData[i - 1], a);
       }
     }
}

//==============================================================================
// Sine Wave
//==============================================================================

void MainComponent::sineWave (const AudioSourceChannelInfo& bufferToFill) {
    double initialPhase = phase;
     for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
     {
       phase = initialPhase;
         float* const channelData = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);
       for (int i = 0; i < bufferToFill.numSamples ; ++i)
       {
           channelData[i] =  std::sin(TwoPi * phase) * level;
         phase += phaseDelta;
       }
     }
}

//==============================================================================
// Low Frequency Waveforms
//==============================================================================

/// Impulse wave

void MainComponent::LF_impulseWave (const AudioSourceChannelInfo& bufferToFill) {
    double initialPhase = phase;
      for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
      {
         phase = initialPhase;
         float* const channelData = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);
         for (int i = 0; i < bufferToFill.numSamples ; ++i) {
            double pre = phasor();
            phase += phaseDelta;
            double curr = phasor();
            if (curr - pre < 0) {
          
               channelData[i] = 1 * level;
            }
         }
      }
}

/// Square wave

void MainComponent::LF_squareWave (const AudioSourceChannelInfo& bufferToFill) {
    double initialPhase = phase;
      for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
      {
         phase = initialPhase;
         float* const channelData = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);
         for (int i = 0; i < bufferToFill.numSamples ; ++i) {

            if (phasor() < .5) {
               channelData[i] =  -1 * level;
            } else {
               channelData[i] =  1 * level;
            }
            phase += phaseDelta;
         }
      }
}

/// Sawtooth wave

void MainComponent::LF_sawtoothWave (const AudioSourceChannelInfo& bufferToFill) {
    double initialPhase = phase;
      for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
      {
         phase = initialPhase;
         float* const channelData = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);
         for (int i = 0; i < bufferToFill.numSamples ; ++i) {
         
            channelData[i] =  (phasor() * 2 - 1) * level;
            phase += phaseDelta;
         }
      }
}

/// Triangle wave

void MainComponent::LF_triangleWave (const AudioSourceChannelInfo& bufferToFill) {
    double initPhase = phase;
       for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
       {
          phase = initPhase;
          float* const channelData = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);
          for (int i = 0; i < bufferToFill.numSamples ; ++i) {
                // phasor in first half
             if (phasor() < .5) {
                channelData[i] =  (phasor() * 4 - 1) * level;
             } else {
                channelData[i] =  ((phasor()  * -4 + 3) *level);
             }
             phase += phaseDelta;
          }
       }
    
}

//==============================================================================
// Band Limited Waveforms
//==============================================================================

/// Impulse (pulse) wave

/// Synthesized by summing sin() over frequency and all its harmonics at equal
/// amplitude. To make it band limited only include harmonics that are at or
/// below the nyquist limit.
void MainComponent::BL_impulseWave (const AudioSourceChannelInfo& bufferToFill) {
    
  
    
    double initPhase = phase;
       for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
       {
          phase = initPhase;
           float* const channel0Data = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
          float* const channelData = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);
           for (int i = 0; i < bufferToFill.numSamples ; ++i) {
               
               
               if (freq != 0) {
                   auto numHarmonic = srate/2/freq;
                   for (auto u = 1; u <= numHarmonic; ++u) {
                       channelData[i] += 1.0 *  std::sin(TwoPi * phasor() * u) * (level/numHarmonic);
                   }
                   phase += phaseDelta;
                   
               }
               std::memcpy(channelData, channel0Data, bufferToFill.numSamples * sizeof(float));
           }
               
          
       }
    
}

/// Square wave

/// Synthesized by summing sin() over all ODD harmonics at 1/harmonic amplitude.
/// To make it band limited only include harmonics that are at or below the
/// nyquist limit.
void MainComponent::BL_squareWave (const AudioSourceChannelInfo& bufferToFill) {
    
    double initPhase = phase;
       for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
       {
          phase = initPhase;
           float* const channel0Data = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
          float* const channelData = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);
           for (int i = 0; i < bufferToFill.numSamples ; ++i) {


               if (freq != 0) {
                   auto numHarmonic = srate/2/freq;
                   for (auto u = 1; u <= numHarmonic; ++u) {
                       if (u % 2 != 0) {
                                         channelData[i] += 1.0 * std::sin(TwoPi * phasor() * u) * (level/u);
                                      }
                      // channelData[i] += 1.0 *  std::sin(TwoPi * phasor() * u) * (level/numHarmonic);
                   }
                   phase += phaseDelta;
               }
               std::memcpy(channelData, channel0Data, bufferToFill.numSamples * sizeof(float));
               
           }


       }


}

/// Sawtooth wave
///
/// Synthesized by summing sin() over all harmonics at 1/harmonic amplitude. To make
/// it band limited only include harmonics that are at or below the nyquist limit.
void MainComponent::BL_sawtoothWave (const AudioSourceChannelInfo& bufferToFill) {
    double initPhase = phase;
       for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
       {
          phase = initPhase;
           float* const channel0Data = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);
          float* const channelData = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);
          for (int i = 0; i < bufferToFill.numSamples ; ++i) {
              
              if (freq != 0) {
                         auto numHarmonic = srate/2/freq;
                         for (auto u = 1; u <= numHarmonic; ++u) {
             
                               channelData[i] += (float) std::sin(TwoPi * phasor() * u) * (level/u);
             
                         }
                      }
              phase += phaseDelta;
              
           
          }
           std::memcpy(channelData, channel0Data, bufferToFill.numSamples * sizeof(float));
       }
    

}

/// Triangle wave
///
/// Synthesized by summing sin() over all ODD harmonics at 1/harmonic**2 amplitude.
/// To make it band limited only include harmonics that are at or below the
/// Nyquist limit.
void MainComponent::BL_triangleWave (const AudioSourceChannelInfo& bufferToFill) {
    
    double initPhase = phase;
       for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
       {
          phase = initPhase;
           float* const channel0Data = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
          float* const channelData = bufferToFill.buffer->getWritePointer(chan, bufferToFill.startSample);
           for (int i = 0; i < bufferToFill.numSamples ; ++i) {


               if (freq != 0) {
                   auto numHarmonic = srate/2/freq;
                   for (auto u = 1; u <= numHarmonic; ++u) {
                       if (u % 2 != 0) {
                           channelData[i] += std::sin(TwoPi * phasor() * u) * ((2*level)/u);
                                      }
                      // channelData[i] += 1.0 *  std::sin(TwoPi * phasor() * u) * (level/numHarmonic);
                   }
                   phase += phaseDelta;
               }
               std::memcpy(channelData, channel0Data, bufferToFill.numSamples * sizeof(float));
           }


       }
    
    

    
}

//==============================================================================
// WaveTable Synthesis
//==============================================================================

// The audio block loop
void inline MainComponent::WT_wave(const AudioSourceChannelInfo& bufferToFill) {
    
   
    
}

// Create a sine wave table
void MainComponent::createSineTable(AudioSampleBuffer& waveTable) {

}

// Create an inpulse wave table
void MainComponent::createImpulseTable(AudioSampleBuffer& waveTable) {
  
}

// Create a square wave table
void MainComponent::createSquareTable(AudioSampleBuffer& waveTable) {
   
}

// Create a sawtooth wave table
void MainComponent::createSawtoothTable(AudioSampleBuffer& waveTable) {
    
}

// Create a triagle wave table
void MainComponent::createTriangleTable(AudioSampleBuffer& waveTable) {
    
}
