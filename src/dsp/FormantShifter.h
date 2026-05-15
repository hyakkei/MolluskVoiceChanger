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

// Formant shifter using cepstral liftering:
//   1. FFT → log-magnitude → IFFT → cepstrum
//   2. Lifter (zero out high quefrency) → FFT → smooth spectral envelope
//   3. Shift envelope along frequency axis
//   4. Reconstruct spectrum = original phase/excitation × shifted envelope
//   5. IFFT + overlap-add
class FormantShifter
{
public:
    FormantShifter();

    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    void  setEnabled(bool e)      { enabled.store(e); }
    void  setSemitones(float s)   { semitones.store(s); }
    bool  getEnabled()  const     { return enabled.load(); }
    float getSemitones() const    { return semitones.load(); }

private:
    void processFrame();

    static constexpr int fftOrder    = 10;
    static constexpr int fftSize     = 1 << fftOrder;  // 1024
    static constexpr int hopSize     = fftSize / 4;     // 256
    static constexpr int fifoSize    = fftSize * 4;     // 4096 (power of 2)
    static constexpr int fifoMask    = fifoSize - 1;
    // Low-quefrency lifter cutoff: separates envelope from pitch harmonics.
    // 30 corresponds to periods > fftSize/30 ≈ 34 samples, i.e. F0 < ~1400 Hz.
    static constexpr int lifterOrder = 30;

    juce::dsp::FFT fft;

    std::vector<float> inputFifo;
    std::vector<float> outputFifo;
    std::vector<float> hannWindow;

    std::vector<std::complex<float>> fftBuf;         // general-purpose work buffer
    std::vector<std::complex<float>> specBuf;         // X[k]: original spectrum
    std::vector<std::complex<float>> cepBuf;          // cepstrum processing
    std::vector<float>               envelope;        // E[k]: smooth envelope magnitudes
    std::vector<float>               shiftedEnvelope; // E_shifted[k]

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
    FadeState fadeState     = FadeState::Passthrough;
    float     fadeGain      = 0.0f;
    int       warmupCounter = 0;

    JUCE_DECLARE_NON_COPYABLE(FormantShifter)
};
