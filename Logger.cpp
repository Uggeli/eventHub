#include "Logger.hpp"
#include <iostream>

Logger::Logger(EventType minLevelLog) 
    : IListener({EventType::LOG_DEBUG, EventType::LOG_INFO, EventType::LOG_WARNING}),
      minLevelLog(minLevelLog) {}

void Logger::HandleEvent(const Event* event) {
    if (event->getType() >= minLevelLog) {
        const LogEvent* logEvent = static_cast<const LogEvent*>(event);
        
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        std::cout << std::ctime(&now_c);
        
        switch (logEvent->getType()) {
            case EventType::LOG_DEBUG:
                std::cout << DEBUG << "[DEBUG] ";
                break;
            case EventType::LOG_INFO:
                std::cout << INFO << "[INFO] ";
                break;
            case EventType::LOG_WARNING:
                std::cout << WARNING << "[WARNING] ";
                break;
            default:
                break;
        }
        std::cout << RESET << logEvent->message << std::endl;
    }
}