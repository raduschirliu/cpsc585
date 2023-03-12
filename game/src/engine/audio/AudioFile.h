#pragma once

#include <AL/al.h>

#include <memory>

struct AudioFile
{
    int number_of_channels_;
    ALuint samples_per_channel;
    ALsizei sample_rate_;
    ALenum format_;
    std::unique_ptr<ALshort> data_;

    ALsizei get_size_bytes()
    {
        ALsizei number_of_samples = samples_per_channel * number_of_channels_;
        ALsizei size_in_bytes = number_of_samples * sizeof(short);
        return size_in_bytes;
    }
};