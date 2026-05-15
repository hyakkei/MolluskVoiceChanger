// Mollusk Voice Changer
// Copyright (C) 2026 Ito Hyakkei
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.

#pragma once
#include <JuceHeader.h>
#include <atomic>
#include <complex>
#include <vector>

// Phase vocoder pitch shifter with proper phase propagation.
// For each hop:
//   1. FFT → compute instantaneous frequency per bin (from phase difference with prev frame)
//   2. Shift bins by pitchRatio, interpolating instantaneous frequencies
//   3. Accumulate synthesis phases continuously → eliminates metallic phasiness
//   4. IFFT + overlap-add
class PitchShifter
{
public:
    PitchShifter();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    void setEnabled(bool e)       { enabled.store(e); }
    void setSemitones(float s)    { semitones.store(s); }

    bool  getEnabled()  const     { return enabled.load(); }
    float getSemitones() const    { return semitones.load(); }

    static constexpr int getLatencySamples() { return fftSize; }

private:
    void processFrame();

    static constexpr int fftOrder = 10;
    static constexpr int fftSize  = 1 << fftOrder;  // 1024
    static constexpr int hopSize  = fftSize / 4;     // 256
    static constexpr int fifoSize = fftSize * 4;     // 4096, power of 2
    static constexpr int fifoMask = fifoSize - 1;
    static constexpr int halfSize = fftSize / 2;

    juce::dsp::FFT fft;

    // I/O FIFOs and window
    std::vector<float> inputFifo;
    std::vector<float> outputFifo;
    std::vector<float> hannWindow;

    // FFT work buffers
    std::vector<std::complex<float>> fftIn;
    std::vector<std::complex<float>> fftOut;

    // Phase vocoder state (positive bins 0..halfSize)
    std::vector<float> analysisMag;        // magnitude of current frame
    std::vector<float> prevAnalysisPhase;  // phase of previous analysis frame
    std::vector<float> trueFreqBin;        // instantaneous frequency (bin units)
    std::vector<float> synthPhaseAccum;    // accumulated synthesis phase

    int inputWritePos   = 0;
    int outputReadPos   = 0;
    int outputWritePos  = 0;
    int newSamplesCount = 0;

    std::atomic<float> semitones { 0.0f };
    std::atomic<bool>  enabled   { true };

    static constexpr float passthroughThreshold = 0.1f;
    static constexpr int   fadeSamples          = 1024;
    static constexpr float fadeInc              = 1.0f / fadeSamples;

    enum class FadeState { Passthrough, FadingIn, Active, FadingOut };
    FadeState fadeState    = FadeState::Passthrough;
    float     fadeGain     = 0.0f;
    int       warmupCounter = 0;  // counts down while FFT output is not yet valid

    JUCE_DECLARE_NON_COPYABLE(PitchShifter)
};
