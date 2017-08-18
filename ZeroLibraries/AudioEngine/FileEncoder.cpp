///////////////////////////////////////////////////////////////////////////////
///
/// Author: Andrea Ellinger
/// Copyright 2017, DigiPen Institute of Technology
///
///////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "stb_vorbis.h"
#include "opus.h"

namespace Audio
{
  struct WavRiffHeader
  {
    char riff_chunk[4];
    unsigned chunk_size;
    char wave_fmt[4];
  };

  struct WavChunkHeader
  {
    char chunk_name[4];
    unsigned chunk_size;
  };

  struct WavFmtData
  {
    unsigned short audio_format;
    unsigned short number_of_channels;
    unsigned sampling_rate;
    unsigned bytes_per_second;
    unsigned short bytes_per_sample;
    unsigned short bits_per_sample;
  };

  // Maximum volume an encoded audio file can have
  static const float MaxVolumeLimit = 0.9f;
  // Used to translate PCM data to floats
  const static float Normalize16Bit = 1 << 15;
  const static float Normalize24Bit = 1 << 23;

  //-------------------------------------------------------------------------------- Audio File Data

  //************************************************************************************************
  void AudioFileData::ReleaseData()
  {
    for (unsigned i = 0; i < Channels; ++i)
      delete[] BuffersPerChannel[i];
    delete[] BuffersPerChannel;
    BuffersPerChannel = nullptr;
  }

  //----------------------------------------------------------------------------------- File Encoder

  //************************************************************************************************
  Audio::AudioFileData FileEncoder::OpenFile(Zero::Status& status, Zero::StringParam fileName)
  {
    AudioFileData data;
    Zero::File file;

    // Open the input file
    file.Open(fileName, Zero::FileMode::Read, Zero::FileAccessPattern::Sequential);
    // If didn't open successfully, set the status and return
    if (!file.IsOpen())
    {
      status.SetFailed(Zero::String::Format("Couldn't open input file %s", fileName.c_str()));
      return data;
    }

    // Read in the header from the input file
    WavRiffHeader header;
    if (file.Read(status, (byte*)(&header), sizeof(header)) < sizeof(header))
    {
      status.SetFailed(Zero::String::Format("Unable to read from file %s", fileName.c_str()));
      return data;
    }

    // If the file starts with RIFF, it's a WAV file
    if (header.riff_chunk[0] == 'R' && header.riff_chunk[1] == 'I')
    {
      // Check WAV ID - if not WAVE, set status and return
      if (header.wave_fmt[0] != 'W' || header.wave_fmt[1] != 'A')
      {
        status.SetFailed(Zero::String::Format("File %s is an unreadable format", fileName.c_str()));
        return data;
      }

      // Reads the audio data into the AudioFileData object
      ReadWav(status, file, fileName, data);
    }
    // If it starts with Ogg, it's an Ogg file
    else if ((header.riff_chunk[0] == 'O' || header.riff_chunk[0] == 'o') &&
      (header.riff_chunk[1] == 'G' || header.riff_chunk[1] == 'g'))
    {
      // Reads the audio data into the AudioFileData object
      ReadOgg(status, file, fileName, data);
    }
    else
    {
      status.SetFailed(Zero::String::Format("File %s was not in WAV or OGG format",
        fileName.c_str()));
      return data;
    }

    return data;
  }

  //************************************************************************************************
  void FileEncoder::WriteFile(Zero::Status& status, Zero::StringParam outputFileName, 
    AudioFileData& data)
  {
    // Open the output file
    Zero::File outputFile;
    outputFile.Open(outputFileName, Zero::FileMode::Write, Zero::FileAccessPattern::Sequential);
    // If didn't open successfully, set the status and return
    if (!outputFile.IsOpen())
    {
      status.SetFailed(Zero::String::Format("Couldn't create output file %s", outputFileName.c_str()));
      return;
    }

    // Make sure the volume of the audio is below the MaxVolumeLimit
    Normalize(data.BuffersPerChannel, data.SamplesPerChannel, data.Channels);

    // If the sample rate of the file is different from the system's sample rate, resample the audio
    if (data.SampleRate != AudioSystemInternal::SampleRate)
    {
      data.SamplesPerChannel = Resample(data.SampleRate, data.Channels, data.SamplesPerChannel, 
        data.BuffersPerChannel);
      data.SampleRate = AudioSystemInternal::SampleRate;
    }

    // Encode the file and write it out
    EncodeFile(status, outputFile, data, data.BuffersPerChannel);
  }

  //************************************************************************************************
  void FileEncoder::ReadWav(Zero::Status& status, Zero::File& file, Zero::StringParam fileName,
    AudioFileData& data)
  {
    // Read in the next chunk header
    WavChunkHeader chunkHeader;
    file.Read(status, (byte*)(&chunkHeader), sizeof(chunkHeader));
    // If this isn't the fmt chunk, keep looking
    while (chunkHeader.chunk_name[0] != 'f' || chunkHeader.chunk_name[1] != 'm'
      || chunkHeader.chunk_name[2] != 't')
    {
      // Skip over this chunk
      file.Seek(chunkHeader.chunk_size, Zero::FileOrigin::Current);
      // Read in the next chunk header
      file.Read(status, (byte*)(&chunkHeader), sizeof(chunkHeader));

      if (status.Failed())
        return;
    }

    // Read in the fmt chunk data
    WavFmtData fmtChunkData;
    file.Read(status, (byte*)(&fmtChunkData), sizeof(fmtChunkData));

    // If the chunk size is larger than the WavFmtData struct, skip ahead
    if (chunkHeader.chunk_size > sizeof(fmtChunkData))
      file.Seek(chunkHeader.chunk_size - sizeof(fmtChunkData), Zero::FileOrigin::Current);

    // Get the data chunk header
    file.Read(status, (byte*)(&chunkHeader), sizeof(chunkHeader));
    // If this isn't the data chunk, keep looking
    while (chunkHeader.chunk_name[0] != 'd' || chunkHeader.chunk_name[1] != 'a'
      || chunkHeader.chunk_name[2] != 't')
    {
      // Skip over this chunk
      file.Seek(chunkHeader.chunk_size, Zero::FileOrigin::Current);
      // Read in the next chunk header
      file.Read(status, (byte*)(&chunkHeader), sizeof(chunkHeader));

      if (status.Failed())
        return;
    }

    // Set the variables on the AudioFileData object
    data.Channels = fmtChunkData.number_of_channels;
    data.SamplesPerChannel = chunkHeader.chunk_size / fmtChunkData.bytes_per_sample;
    data.SampleRate = fmtChunkData.sampling_rate;

    // Create the buffer for reading in data from the file
    byte* rawDataBuffer = new byte[chunkHeader.chunk_size];

    // Read in the audio data 
    size_t sizeRead = file.Read(status, rawDataBuffer, chunkHeader.chunk_size);
    // If the read failed, delete the buffer and return
    if (status.Failed() || sizeRead < chunkHeader.chunk_size)
    {
      status.SetFailed(Zero::String::Format("Couldn't read data from file %s", fileName.c_str()));
      delete[] rawDataBuffer;
      return;
    }

    // Create sample buffers for each channel
    data.BuffersPerChannel = new float*[data.Channels];
    for (unsigned i = 0; i < data.Channels; ++i)
      data.BuffersPerChannel[i] = new float[data.SamplesPerChannel];

    // If the PCM to float translation returns false, it wasn't able to translate the format
    if (!PcmToFloat(rawDataBuffer, data.BuffersPerChannel, data.SamplesPerChannel * data.Channels,
      data.Channels, fmtChunkData.bytes_per_sample / data.Channels))
    {
      status.SetFailed(Zero::String::Format("File %s is in WAV format but is not 16 or 24 bit",
        fileName.c_str()));
    }

    delete[] rawDataBuffer;
  }

  //************************************************************************************************
  void FileEncoder::ReadOgg(Zero::Status& status, Zero::File& file, Zero::StringParam fileName, 
    AudioFileData& data)
  {
    // Reset to beginning of file
    file.Seek(0);
    
    // Save the size of the file
    unsigned fileSize = (unsigned)file.CurrentFileSize();

    // Create the buffer for reading in data from the file
    byte* rawDataBuffer = new byte[fileSize];

    // Read in the entire file
    size_t sizeRead = file.Read(status, rawDataBuffer, fileSize);
    // If the read failed, delete the buffer and return
    if (status.Failed() || sizeRead < fileSize)
    {
      delete[] rawDataBuffer;
      return;
    }

    // Create the vorbis stream
    int error;
    stb_vorbis* oggStream = stb_vorbis_open_memory(rawDataBuffer, fileSize, &error, nullptr);
    // If there was an error, set the failed status and delete the buffer
    if (error != VORBIS__no_error)
    {
      status.SetFailed(Zero::String::Format("Error reading file %s: vorbis error %d",
        fileName.c_str(), error));
      delete[] rawDataBuffer;
      stb_vorbis_close(oggStream);
      return;
    }

    // Get the ogg vorbis file info
    stb_vorbis_info info = stb_vorbis_get_info(oggStream);

    // Set the variables on the AudioFileData object
    data.Channels = info.channels;
    data.SamplesPerChannel = stb_vorbis_stream_length_in_samples(oggStream);
    data.SampleRate = info.sample_rate;

    // Create sample buffers for each channel
    data.BuffersPerChannel = new float*[data.Channels];
    for (unsigned i = 0; i < data.Channels; ++i)
      data.BuffersPerChannel[i] = new float[data.SamplesPerChannel];

    // Translate the ogg data into the buffers of floats
    data.SamplesPerChannel = stb_vorbis_get_samples_float(oggStream, data.Channels, 
      data.BuffersPerChannel, data.SamplesPerChannel);

    // Close the vorbis stream
    stb_vorbis_close(oggStream);
  }

  //************************************************************************************************
  bool FileEncoder::PcmToFloat(byte* inputBuffer, float** samplesPerChannel,
    const unsigned totalSampleCount, const unsigned channelCount, const unsigned bytesPerSample)
  {
    // 16 bit data can be read as shorts
    if (bytesPerSample == 2)
    {
      // Save a variable for accessing the input buffer as shorts
      short* input = (short*)inputBuffer;

      // Step through each sample in the input buffer, also keeping track of audio frames
      // for the buffers of translated floats per channel
      for (unsigned inputIndex = 0, frameIndex = 0; inputIndex < totalSampleCount; ++frameIndex)
      {
        for (unsigned channel = 0; channel < channelCount; ++channel, ++inputIndex)
          samplesPerChannel[channel][frameIndex] = (float)input[inputIndex] / Normalize16Bit;
      }

      return true;
    }
    // 24 bit data
    else if (bytesPerSample == 3)
    {
      // Step through each byte of data in the input buffer, also keeping track of audio frames
      // for the buffers of translated floats per channel
      for (unsigned index = 0, sampleIndex = 0; index < totalSampleCount * bytesPerSample; ++sampleIndex)
      {
        for (unsigned channel = 0; channel < channelCount; index += bytesPerSample, ++channel)
        {
          // Get first byte
          int sample = inputBuffer[index];
          // Get next two bytes
          memcpy(&sample, inputBuffer + index, sizeof(char) * 3);
          // Account for negative numbers
          if (sample & 0x800000)
            sample |= 0xff000000;
          else
            sample &= 0x00ffffff;

          samplesPerChannel[channel][sampleIndex] = (float)sample / Normalize24Bit;
        }
      }

      return true;
    }
    else
      return false;
  }

  //************************************************************************************************
  void FileEncoder::Normalize(float** samplesPerChannel, const unsigned frames, const unsigned channels)
  {
    // Save variables for finding the maximum volume in the audio data
    float maxVolume(0.0f);
    float volume(0.0f);

    // Step through every audio frame and every channel, saving the volume if its higher 
    // than the current maximum 
    for (unsigned i = 0; i < frames; ++i)
    {
      for (unsigned j = 0; j < channels; ++j)
      {
        volume = Math::Abs(samplesPerChannel[j][i]);

        if (volume > maxVolume)
          maxVolume = volume;
      }
    }

    // Check if the maximum volume is higher than the volume limit
    if (maxVolume > MaxVolumeLimit)
    {
      // Find the ratio to use when reducing the volume of all the audio
      float multiplier = MaxVolumeLimit / maxVolume;

      // Step through each frame and channel, applying the volume reduction
      for (unsigned i = 0; i < frames; ++i)
      {
        for (unsigned j = 0; j < channels; ++j)
        {
          samplesPerChannel[j][i] *= multiplier;
        }
      }
    }
  }

  //************************************************************************************************
  unsigned FileEncoder::Resample(unsigned fileSampleRate, unsigned channels, unsigned samplesPerChannel, 
    float**& buffersPerChannel)
  {
    // Get the factor to use while resampling
    double resampleFactor = (double)fileSampleRate / (double)AudioSystemInternal::SampleRate;

    unsigned newFrames(0);
    Zero::Array<float> newSamples;

    // Resample each channel separately
    for (unsigned i = 0; i < channels; ++i)
    {
      // Save a variable for the buffer of input samples
      float* oldSamples = buffersPerChannel[i];
      // Save variables for the indexes
      unsigned index(0);
      double resampleIndex(0);

      // Go through the entire buffer of samples
      while (index + 1 < samplesPerChannel)
      {
        // Save the two sample values
        float firstSample = oldSamples[index];
        float secondSample = oldSamples[index + 1];

        // Add the interpolated value to the array
        newSamples.PushBack(firstSample + (float)((secondSample - firstSample) * (resampleIndex - index)));

        // Move the indexes forward
        resampleIndex += resampleFactor;
        index = (unsigned)resampleIndex;
      }

      // Save the number of resampled audio frames
      newFrames = newSamples.Size();

      // Delete the buffer of input data
      delete[] buffersPerChannel[i];
      // Set the buffer to the data in the array
      buffersPerChannel[i] = newSamples.Data();
      // Release the data from the array
      newSamples.ReleaseData();
    }

    return newFrames;
  }

  //************************************************************************************************
  void FileEncoder::EncodeFile(Zero::Status& status, Zero::File& outputFile, AudioFileData& data, 
    float** buffersPerChannel)
  {
    // Create the buffer for encoded packets
    unsigned char encodedPacket[MaxPacketSize];

    // Set up the file header
    FileHeader fileHeader;
    fileHeader.Channels = data.Channels;
    fileHeader.SamplesPerChannel = data.SamplesPerChannel;

    ErrorIf(fileHeader.Channels <= 0);

    // Write the header to the output file
    outputFile.Write((byte*)&fileHeader, sizeof(fileHeader));

    // Create the packet header object
    PacketHeader packetHeader;

    int error;
    // Create an opus encoder for each channel
    OpusEncoder** encodersPerChannel = new OpusEncoder*[data.Channels];
    for (unsigned i = 0; i < data.Channels; ++i)
    {
      encodersPerChannel[i] = opus_encoder_create(AudioSystemInternal::SampleRate, 1,
        OPUS_APPLICATION_AUDIO, &error);

      // If there was an error creating the encoder, set the failed message and return
      if (error < 0)
        status.SetFailed(Zero::String::Format("Error encoding file: %s\n", opus_strerror(error)));
    }

    // Current buffer position for encoding
    float* buffer(nullptr);
    // Used on the final frame to keep a usable frame size
    float finalBuffer[FrameSize] = { 0 };

    // Encode the samples, in chunks of FrameSize
    for (unsigned inputIndex = 0; inputIndex < data.SamplesPerChannel; inputIndex += FrameSize)
    {
      // Handle each channel separately
      for (unsigned channel = 0; channel < data.Channels; ++channel)
      {
        // If the next FrameSize chunk would go past the end of the audio data, fill in what's
        // available to the finalBuffer, the rest will be zeros
        if (inputIndex + FrameSize > data.SamplesPerChannel)
        {
          buffer = finalBuffer;
          memcpy(buffer, buffersPerChannel[channel] + inputIndex,
            (data.SamplesPerChannel - inputIndex) * sizeof(float));

          // TODO pick closest acceptable frame size instead of full size
        }
        else
          buffer = buffersPerChannel[channel] + inputIndex;

        // Encode a packet for this channel
        packetHeader.Size = opus_encode_float(encodersPerChannel[channel], buffer, FrameSize,
          encodedPacket, MaxPacketSize);
        // If there was an error, set the status 
        if (packetHeader.Size <= 0)
        {
          status.SetFailed(Zero::String::Format("Encode failed: %s\n", opus_strerror(packetHeader.Size)));
          break;
        }

        ErrorIf(packetHeader.Size > MaxPacketSize);

        // Set the channel number on the header
        packetHeader.Channel = channel;

        // Write the header to the file
        outputFile.Write((byte*)&packetHeader, sizeof(packetHeader));
        // Write the encoded packet to the file
        outputFile.Write((byte*)encodedPacket, packetHeader.Size);
      }
    }

    // Destroy the encoders
    for (unsigned i = 0; i < data.Channels; ++i)
      opus_encoder_destroy(encodersPerChannel[i]);
    delete[] encodersPerChannel;
  }

}