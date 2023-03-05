#pragma once

#include <AL/al.h>
#include <memory>

struct AudioFile
{
    int number_of_channels_;
    int samples_per_channel;
    int sample_rate_;
    ALenum format_;
    std::unique_ptr<short> data_;
};