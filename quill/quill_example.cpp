#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/sinks/ConsoleSink.h"
#include "quill/std/Array.h"

#include <string>
#include <utility>

int main()
{
    // Backend
    quill::BackendOptions backend_options;
    quill::Backend::start(backend_options);

    // Frontend
    auto console_sink = quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1");
    quill::Logger *logger = quill::Frontend::create_or_get_logger("root", std::move(console_sink));

    // Change the LogLevel to print everything
    logger->set_log_level(quill::LogLevel::TraceL3);

    // A log message with number 123
    int a = 123;
    std::string l = "log";
    LOG_INFO(logger, "A {} message with number {}", l, a);

    // libfmt formatting language is supported 3.14e+00
    double pi = 3.141592653589793;
    LOG_INFO(logger, "libfmt formatting language is supported {:.2e}", pi);

    // Logging STD types is supported [1, 2, 3]
    std::array<int, 3> arr = {1, 2, 3};
    LOG_INFO(logger, "Logging STD types is supported {}", arr);

    // Logging STD types is supported [arr: [1, 2, 3]]
    LOGV_INFO(logger, "Logging STD types is supported", arr);

    // A message with two variables [a: 123, b: 3.17]
    double b = 3.17;
    LOGV_INFO(logger, "A message with two variables", a, b);

    for (uint32_t i = 0; i < 10; ++i)
    {
        // Will only log the message once per second
        LOG_INFO_LIMIT(std::chrono::seconds{1}, logger, "A {} message with number {}", l, a);
        LOGV_INFO_LIMIT(std::chrono::seconds{1}, logger, "A message with two variables", a, b);
    }

    LOG_TRACE_L3(logger, "Support for floats {:03.2f}", 1.23456);
    LOG_TRACE_L2(logger, "Positional arguments are {1} {0} ", "too", "supported");
    LOG_TRACE_L1(logger, "{:>30}", std::string_view{"right aligned"});
    LOG_DEBUG(logger, "Debugging foo {}", 1234);
    LOG_INFO(logger, "Welcome to Quill!");
    LOG_WARNING(logger, "A warning message.");
    LOG_ERROR(logger, "An error message. error code {}", 123);
    LOG_CRITICAL(logger, "A critical error.");
}