/*
  * Copyright (C) 2015 Cameron White
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
  
#include "midifile.h"

#include <boost/rational.hpp>
#include <QSettings>

#include <app/settings.h>
#include <score/generalmidi.h>
#include <score/score.h>
#include <score/systemlocation.h>
#include <score/utils.h>

static const int PERCUSSION_CHANNEL = 9;
static const int METRONOME_CHANNEL = PERCUSSION_CHANNEL;
static const int DEFAULT_PPQ = 480;

MidiFile::MidiFile() : myTicksPerBeat(0)
{
}

void MidiFile::load(const Score &score)
{
    myTicksPerBeat = DEFAULT_PPQ;

    MidiEventList master_track;
    MidiEventList metronome_track;
    // TODO - set channel volume at start of track.

    SystemLocation location(0, 0);
    int current_tick = 0;
    while (location.getSystem() < score.getSystems().size())
    {
        const System &system = score.getSystems()[location.getSystem()];
        const Barline *current_bar = ScoreUtils::findByPosition(
            system.getBarlines(), location.getPosition());
        const Barline *next_bar = system.getNextBarline(location.getPosition());

        addTempoEvent(master_track, current_tick, system, current_bar->getPosition(),
                      next_bar->getPosition());

        current_tick = generateMetronome(metronome_track, current_tick, system,
                                         *current_bar, *next_bar, location);

        // Move to the next bar.
        // TODO - handle repeats.
        if (next_bar == &system.getBarlines().back())
        {
            location.setSystem(location.getSystem() + 1);
            location.setPosition(0);
        }
        else
            location.setPosition(next_bar->getPosition());
    }

    myTracks.push_back(master_track);
    myTracks.push_back(metronome_track);

    for (MidiEventList &track : myTracks)
    {
        track.append(MidiEvent::endOfTrack(current_tick));
        track.convertToDeltaTicks();
    }
}

int MidiFile::generateMetronome(MidiEventList &event_list, int current_tick,
                                const System &system,
                                const Barline &current_bar,
                                const Barline &next_bar,
                                const SystemLocation &location)
{
    QSettings settings;
    const uint8_t strong_vel =
        settings.value(Settings::MIDI_METRONOME_STRONG_ACCENT,
                       Settings::MIDI_METRONOME_STRONG_ACCENT_DEFAULT).toUInt();
    const uint8_t weak_vel =
        settings.value(Settings::MIDI_METRONOME_WEAK_ACCENT,
                       Settings::MIDI_METRONOME_WEAK_ACCENT_DEFAULT).toUInt();
    const uint8_t preset =
        Midi::MIDI_PERCUSSION_PRESET_OFFSET +
        settings.value(Settings::MIDI_METRONOME_PRESET,
                       Settings::MIDI_METRONOME_PRESET_DEFAULT).toUInt();

    const TimeSignature &time_sig = current_bar.getTimeSignature();

    const int num_pulses = time_sig.getNumPulses();
    const int beats_per_measure = time_sig.getBeatsPerMeasure();
    const int beat_value = time_sig.getBeatValue();
    const int position = current_bar.getPosition();

    // Figure out the duration of a pulse.
    const int duration = boost::rational_cast<int>(
        boost::rational<int>(4, beat_value) *
        boost::rational<int>(beats_per_measure, num_pulses) * myTicksPerBeat);

    // Check for multi-bar rests, as we need to generate more metronome events
    // to fill the extra bars.
    int num_repeats = 1;
    for (const Staff &staff : system.getStaves())
    {
        for (const Voice &voice : staff.getVoices())
        {
            for (const Position &pos : ScoreUtils::findInRange(
                     voice.getPositions(), current_bar.getPosition(),
                     next_bar.getPosition()))
            {
                if (pos.hasMultiBarRest())
                {
                    num_repeats =
                        std::max(num_repeats, pos.getMultiBarRestCount());
                }
            }
        }
    }

    for (int repeat = 0; repeat < num_repeats; ++repeat)
    {
        for (int i = 0; i < num_pulses; ++i)
        {
            const uint8_t velocity = (i == 0) ? strong_vel : weak_vel;

            event_list.append(MidiEvent::noteOn(current_tick, METRONOME_CHANNEL,
                                                preset, velocity, location));

            current_tick += duration;

            event_list.append(MidiEvent::noteOff(
                current_tick, METRONOME_CHANNEL, preset, location));
        }
    }

    return current_tick;
}

void MidiFile::addTempoEvent(MidiEventList &event_list, int current_tick,
                             const System &system, int bar_start, int bar_end)
{
    auto markers = ScoreUtils::findInRange(system.getTempoMarkers(), bar_start,
                                           bar_end - 1);
    // If multiple tempo markers occur in a bar, just choose the last one.
    if (!markers.empty())
    {
        const TempoMarker &marker = markers.back();

        // Convert the values in the TempoMarker::BeatType enum to a factor that
        // will scale the bpm value to be in terms of quarter notes.
        boost::rational<int> scale(2, 1 << (marker.getBeatType() / 2));
        if (marker.getBeatType() % 2 != 0)
            scale *= boost::rational<int>(3, 2);

        // Compute the number of microseconds per quarter note.
        int tick_duration = boost::rational_cast<int>(
            60000000 / (scale * marker.getBeatsPerMinute()));

        event_list.append(MidiEvent::setTempo(current_tick, tick_duration));
    }
}