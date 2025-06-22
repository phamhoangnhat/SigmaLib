#ifndef SIGMALIB_H
#define SIGMALIB_H

#ifdef SIGMALIB_LIBRARY
#define SIGMALIB_EXPORT __declspec(dllexport)
#else
#define SIGMALIB_EXPORT __declspec(dllimport)
#endif

class SIGMALIB_EXPORT SigmaLib {
public:
    static SigmaLib& getInstance();
    void start();
    void run();
    void stop();

private:
    SigmaLib();
    ~SigmaLib();
    SigmaLib(const SigmaLib&) = delete;
    SigmaLib& operator=(const SigmaLib&) = delete;
};

#ifdef __cplusplus
extern "C" {
#endif

    SIGMALIB_EXPORT void sigmalib_start();
    SIGMALIB_EXPORT void sigmalib_stop();

#ifdef __cplusplus
}
#endif

#endif
