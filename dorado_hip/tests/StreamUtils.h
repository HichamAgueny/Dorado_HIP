#ifdef _WIN32
#include <io.h>
#define NULL_DEVICE "NUL:"
#define Close _close
#define Dup _dup
#define Dup2 _dup2
#define Fileno _fileno
#else
#include <unistd.h>
#define NULL_DEVICE "/dev/null"
#define Close close
#define Dup dup
#define Dup2 dup2
#define Fileno fileno
#endif

#include <stdio.h>

#include <functional>

namespace ont::test_utils::streams {

namespace details {
template <class T>
void ignore(const T &) {}
}  // namespace details

inline int suppress_stderr() {
    fflush(stderr);
    int fd = Dup(Fileno(stderr));
    auto old_stream = freopen(NULL_DEVICE, "w", stderr);
    details::ignore(old_stream);
    return fd;
}

inline void restore_stderr(int fd) {
    fflush(stderr);
    auto dup_result = Dup2(fd, Fileno(stderr));
    details::ignore(dup_result);
    Close(fd);
}

inline int suppress_stdout() {
    fflush(stdout);
    int fd = Dup(Fileno(stdout));
    auto old_stream = freopen(NULL_DEVICE, "w", stdout);
    details::ignore(old_stream);
    return fd;
}

inline void restore_stdout(int fd) {
    fflush(stdout);
    auto dup_result = Dup2(fd, Fileno(stdout));
    details::ignore(dup_result);
    Close(fd);
}

/// <summary>
/// RAII helper to provide scoped stderr suppression
/// </summary>
class [[nodiscard]] SuppressStderr final {
    const int m_fd;

public:
    SuppressStderr() : m_fd(suppress_stderr()) {}
    ~SuppressStderr() { restore_stderr(m_fd); }

    /// <summary>
    /// Invoke a function with stderr suppressed
    /// </summary>
    static void invoke(std::function<void()> func) {
        SuppressStderr suppression{};
        func();
    }
};

}  // namespace ont::test_utils::streams
