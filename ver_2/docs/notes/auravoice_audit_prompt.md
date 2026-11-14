
# AURAVOICE-S3

## Expert Engineering Audit, Architecture Optimization & Implementation Specification

### 1. Role and Responsibility

Act as a senior embedded audio DSP engineer, speech synthesis researcher, ESP32-S3 firmware architect, and real-time systems engineer.

You are responsible for critically reviewing and optimizing the proposed AuraVoice-S3 project.

This is not a request to simply improve documentation or agree with the existing design.

Your responsibility is to identify architectural weaknesses, mathematically incorrect assumptions, hardware limitations, computational bottlenecks, and unrealistic performance targets.

Challenge every engineering decision that is not adequately justified.

The final objective is to develop a technically sound, resource-efficient, fully offline Text-to-Speech engine running on the ESP32-S3.

### 2. Project Objective

Develop a custom, proprietary, offline TTS engine for ESP32-S3.

The system must:

* Operate entirely offline.
* Accept text through UART, LoRa, BLE or other interfaces.
* Convert text into intelligible speech.
* Generate audio continuously without buffering entire sentences.
* Require minimal RAM and CPU resources.
* Support customizable voice characteristics.
* Produce understandable, reasonably natural speech.
* Operate alongside other ESP32-S3 application tasks.

The primary objective is intelligibility, reliability and computational efficiency.

Naturalness is a secondary optimization objective.

Do not sacrifice speech intelligibility or real-time reliability for unnecessary DSP complexity.

### 3. Hardware Constraints

Target hardware:

* MCU: ESP32-S3.
* CPU: Dual-core Xtensa LX7.
* Maximum clock: 240 MHz.
* Internal SRAM: Limited; account for actual application and system allocations.
* External PSRAM: Do not assume availability.
* Audio output: Hardware I2S PDM TX.
* External I2S DAC: Not available.
* Speaker output: Requires appropriate filtering and amplification.
* Development environment: ESP-IDF.

Verify hardware capabilities against the ESP32-S3 technical reference manual and the relevant ESP-IDF documentation.

Do not assume that every feature supported by another ESP32 variant is supported identically by the ESP32-S3.

### 4. Critical Architectural Review

Perform a complete review of the existing architecture.

#### A. Text Processing

Evaluate:

* Text normalization.
* Number-to-word conversion.
* Date and time expansion.
* Abbreviation handling.
* Acronym pronunciation.
* Grapheme-to-phoneme conversion.
* Dictionary-based pronunciation.
* Rule-based fallback pronunciation.

Determine whether the proposed phonemizer can handle pronunciation ambiguity.

Recommend an efficient hybrid architecture.

Consider using a compact phoneme dictionary with pronunciation rules rather than relying exclusively on manually written conversion rules.

Define a standard phoneme representation, preferably based on a well-defined phoneme inventory.

#### B. Acoustic Model

Critically evaluate the proposed Rosenberg-Klatt synthesis architecture.

Investigate:

1. Rosenberg glottal excitation.
2. Voiced and unvoiced excitation.
3. Formant resonance generation.
4. Cascade versus parallel formant architectures.
5. Formant bandwidth control.
6. Nasal resonance.
7. Fricative noise generation.
8. Plosive bursts and aspiration.
9. Phoneme transitions.
10. Coarticulation.

Determine whether five cascaded biquad resonators are the best architecture.

Compare:

* Cascade formant synthesis.
* Parallel formant synthesis.
* Hybrid cascade-parallel synthesis.

Select the most appropriate architecture for ESP32-S3.

Do not assume that increasing the number of formants automatically improves speech quality.

Investigate numerical stability, coefficient calculation, filter normalization, state handling and amplitude management.

Provide mathematically correct difference equations and implementation recommendations.

#### C. Prosody Engine

The current proposal uses exponential pitch declination and stress-based pitch adjustments.

Determine whether this is sufficient.

Investigate:

* Fundamental frequency generation.
* Pitch contour interpolation.
* Syllable stress.
* Phoneme duration.
* Sentence-level intonation.
* Question intonation.
* Pause generation.
* Amplitude envelopes.
* Speaking rate.
* Phrase boundaries.

Recommend a computationally inexpensive prosody model.

The model should avoid abrupt pitch and amplitude transitions.

#### D. Real-Time Audio Architecture

Critically evaluate the proposed 256-sample DMA architecture.

Investigate:

* DMA buffer size.
* Number of DMA descriptors.
* PCM generation rate.
* PDM modulation requirements.
* Audio sample rate.
* Processing deadlines.
* Buffer underruns.
* Task synchronization.
* Producer-consumer architecture.
* Audio latency.

Identify inconsistencies between the proposed 10 ms acoustic frames and 256-sample audio blocks at 22,050 Hz.

Calculate actual block duration and recommend an appropriate buffer configuration.

Distinguish between:

* PCM sample rate.
* PDM bit clock.
* DMA transfer size.
* Audio output latency.

Do not claim sub-10 ms latency without a complete latency budget.

### 5. Hardware Audio Output Investigation

The project must work without an external conventional I2S DAC.

Investigate the ESP32-S3 PDM TX peripheral.

Determine:

* Supported operating modes.
* Clock configuration.
* Sample rate limitations.
* PDM clock requirements.
* Hardware filtering requirements.
* GPIO routing.
* ESP-IDF driver configuration.
* DMA integration.
* Output signal characteristics.

Evaluate practical speaker-driving solutions.

Compare:

1. PDM output followed by suitable low-pass filtering and amplification.
2. Other hardware-supported output configurations.
3. External PDM-compatible amplification, if suitable.

Do not confuse digital PDM output with analog speaker output.

Provide a recommended circuit architecture.

### 6. Memory and Computational Optimization

The existing document estimates approximately 1.5 KB of RAM.

Audit this estimate.

Calculate memory requirements for:

* DMA buffers.
* Acoustic parameter queues.
* Filter states.
* Phoneme buffers.
* Text processing.
* Dictionaries.
* Task stacks.
* Temporary processing buffers.

Provide three implementation profiles:

| Profile           | Objective                                          |
| ----------------- | -------------------------------------------------- |
| Minimum footprint | Minimum RAM and CPU consumption                    |
| Balanced          | Good intelligibility and resource efficiency       |
| Quality focused   | Improved speech quality with higher resource usage |

Investigate whether the following optimizations are worthwhile:

* Single-precision floating point.
* Fixed-point DSP.
* Lookup tables.
* Coefficient precomputation.
* ESP-DSP library.
* Xtensa SIMD instructions.
* Memory alignment.
* Cache-aware processing.
* Task core affinity.

Do not introduce SIMD or fixed-point optimization before profiling the actual bottlenecks.

Estimate CPU utilization using realistic computational assumptions.

Clearly distinguish measured performance from theoretical estimates.

### 7. Voice Quality Improvements

Identify the minimum acoustic features necessary for understandable human-like speech.

Prioritize improvements in the following order:

1. Correct phoneme generation.
2. Correct phoneme duration.
3. Clear vowel formants.
4. Consonant intelligibility.
5. Smooth phoneme transitions.
6. Natural pitch variation.
7. Natural amplitude envelopes.
8. Sentence-level prosody.

Investigate whether a hybrid synthesis architecture can improve speech quality without requiring neural inference.

Explore the possibility of using prerecorded phoneme or diphone samples selectively, if they provide meaningful improvements within the available storage and RAM constraints.

Do not recommend neural TTS merely because it produces more natural speech. Evaluate its actual feasibility on ESP32-S3.

### 8. Custom Voice Architecture

The engine should support future custom voice profiles.

Design a voice configuration system that allows changing:

* Fundamental frequency range.
* Formant frequencies.
* Formant bandwidths.
* Vocal tract characteristics.
* Speaking speed.
* Pitch variation.
* Voice brightness.
* Nasality.
* Overall amplitude.

Separate voice configuration data from the synthesis engine.

Determine whether multiple voice profiles can be stored in flash and selected dynamically.

### 9. Reliability and Embedded Systems Considerations

Review the design for:

* Watchdog compatibility.
* FreeRTOS scheduling.
* Task priorities.
* Stack allocation.
* Heap fragmentation.
* DMA memory requirements.
* Thread safety.
* Queue overflow.
* Audio underruns.
* Peripheral initialization.
* Recovery from invalid text input.
* Continuous operation.

The engine must not interfere with LoRa, BLE, UART or other application functions.

Recommend a robust task architecture.

### 10. Required Experimental Validation

Do not rely exclusively on theoretical calculations.

Develop a validation plan covering:

**Hardware tests**

* Verify PDM output.
* Verify clock configuration.
* Measure output waveform.
* Test filtering and amplification.

**DSP tests**

* Generate sine waves.
* Generate glottal pulses.
* Test individual formants.
* Test multiple resonators.
* Verify filter stability.
* Measure processing time.

**Speech tests**

* Test individual vowels.
* Test consonants.
* Test CV and CVC syllables.
* Test common English words.
* Test complete sentences.
* Test numbers and announcements.

**Performance tests**

* Measure RAM consumption.
* Measure CPU utilization.
* Measure first-audio latency.
* Measure audio underruns.
* Run continuous speech tests.

Define measurable acceptance criteria for each phase.

### 11. Implementation Strategy

Reorganize the development roadmap based on technical dependencies.

The recommended initial sequence should be:

Phase 1: Hardware audio output validation.

Phase 2: PCM streaming and DMA architecture.

Phase 3: Basic acoustic excitation generator.

Phase 4: Individual formant implementation and validation.

Phase 5: Complete acoustic synthesis engine.

Phase 6: Phoneme inventory and pronunciation system.

Phase 7: Phoneme duration and coarticulation.

Phase 8: Prosody and intonation.

Phase 9: Text normalization and application integration.

Phase 10: Performance optimization and quality improvements.

You may change this sequence if you identify a technically superior approach.

For every phase, specify:

* Implementation tasks.
* Required source files.
* Dependencies.
* Expected output.
* Test procedure.
* Acceptance criteria.

### 12. Final Deliverables

Produce the following documents:

1. **Technical Audit Report**

   * Existing architectural weaknesses.
   * Incorrect assumptions.
   * Hardware limitations.
   * Mathematical corrections.
   * Computational risks.
2. **Optimized System Architecture**

   * Complete block diagram.
   * Data flow.
   * Task architecture.
   * Audio pipeline.
   * Memory architecture.
3. **Mathematical DSP Specification**

   * Glottal excitation equations.
   * Resonator equations.
   * Filter coefficient calculations.
   * Prosody equations.
   * Parameter interpolation.
4. **Hardware Audio Specification**

   * ESP32-S3 PDM configuration.
   * Clock requirements.
   * GPIO configuration.
   * Filtering and amplification circuit.
5. **Optimized Software Architecture**

   * Directory structure.
   * Module responsibilities.
   * Public APIs.
   * Data structures.
   * FreeRTOS task communication.
6. **Implementation Roadmap**

   * Phases.
   * Milestones.
   * Test procedures.
   * Acceptance criteria.
7. **Resource Budget**

   * RAM.
   * Flash.
   * CPU.
   * DMA memory.
   * Task stack requirements.

### 13. Strict Engineering Rules

Follow these rules throughout the investigation:

1. Do not blindly preserve the existing architecture.
2. Do not assume existing numerical estimates are correct.
3. Do not invent ESP32-S3 hardware capabilities.
4. Verify hardware claims using official documentation.
5. Do not optimize code before identifying actual bottlenecks.
6. Do not confuse audio sample rate with PDM clock frequency.
7. Do not confuse waveform generation with natural speech synthesis.
8. Do not assume five formants guarantee intelligibility.
9. Do not claim production readiness without experimental validation.
10. Prefer simple, deterministic and maintainable embedded algorithms.
11. Preserve full offline functionality.
12. Clearly distinguish experimentally verified results, calculated estimates and engineering assumptions.

### Final Objective

Deliver a revised AuraVoice-S3 architecture that is technically defensible, computationally efficient, modular, maintainable and realistically implementable on the ESP32-S3.

The final design should prioritize:

**Intelligibility → Reliability → Low Resource Consumption → Naturalness → Optimization.**

Do not start writing the complete firmware immediately.

First complete the technical audit, resolve architectural decisions, and produce the optimized engineering specification.

Only after the architecture has been validated should implementation begin.
