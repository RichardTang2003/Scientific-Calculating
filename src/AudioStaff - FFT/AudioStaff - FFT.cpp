#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <thread>
#include "fftw3.h"

// 定义音频参数
const int SAMPLE_RATE = 44100; // 采样率
const int FRAME_SIZE = 1024; // 帧大小
const int WINDOW_SIZE = 1024; // 窗长大小

class AudioAnalyzer {
public:
    AudioAnalyzer(const std::string& filename) : m_audio_data(readAudioFile(filename)) {
        // 初始化FFTW
        fftwf_init_threads();
        fftwf_plan_with_nthreads(std::max(1, static_cast<int>(std::thread::hardware_concurrency() / 2)));

        // 创建FFT计划
        fftwf_complex* in = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * FRAME_SIZE);
        fftwf_complex* out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * FRAME_SIZE);
        m_plan = fftwf_plan_dft_1d(FRAME_SIZE, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

        fftwf_free(in);
        fftwf_free(out);
    }

    ~AudioAnalyzer() {
        // 清理FFTW资源
        fftwf_destroy_plan(m_plan);
        fftwf_cleanup_threads();
    }

    void analyzeAudio() {
        int num_frames = m_audio_data.size() / FRAME_SIZE;
        fftwf_complex* in = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * FRAME_SIZE);
        fftwf_complex* out = (fftwf_complex*)fftwf_malloc(sizeof(fftwf_complex) * FRAME_SIZE);

        for (int i = 0; i < num_frames; ++i) {
            // 将音频数据复制到FFT输入缓冲区
            for (int j = 0; j < FRAME_SIZE; ++j) {
                in[j][0] = m_audio_data[i * FRAME_SIZE + j];
                in[j][1] = 0.0f;
            }

            // 执行FFT
            fftwf_execute(m_plan);

            // 处理FFT结果
            for (int j = 0; j < FRAME_SIZE; ++j) {
                float freq = j * static_cast<float>(SAMPLE_RATE) / FRAME_SIZE;
                float magnitude = std::sqrt(out[j][0] * out[j][0] + out[j][1] * out[j][1]);
                // 对频率和幅值进行处理或分析
                // ...
            }
        }

        fftwf_free(in);
        fftwf_free(out);
    }

private:
    std::vector<float> readAudioFile(const std::string& filename) {
        std::ifstream infile(filename, std::ios::binary);
        if (!infile.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            exit(1);
        }

        // 读取文件头
        char wav_header[44];
        infile.read(wav_header, 44);

        // 读取音频数据
        int num_samples = (static_cast<int>(infile.seekg(0, std::ios::end).tellg()) - 44) / 2;
        std::vector<float> audio_data(num_samples);
        infile.read((char*)&audio_data[0], num_samples * sizeof(float));

        infile.close();

        return audio_data;
    }

    std::vector<float> m_audio_data;
    fftwf_plan m_plan;
};

int main() {
    // 创建AudioAnalyzer对象并分析音频
    std::string filename = "path/to/audio/file.wav";
    AudioAnalyzer analyzer(filename);
    analyzer.analyzeAudio();

    return 0;
}

