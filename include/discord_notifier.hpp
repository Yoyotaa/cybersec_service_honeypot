#pragma once
#include <string>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

struct DiscordEvent {
    std::string json_payload; // payload complet prêt à POST
};

class DiscordNotifier {
public:
    explicit DiscordNotifier(std::string webhook_url);
    ~DiscordNotifier();

    void enqueue(DiscordEvent ev);

private:
    void worker();
    bool post_json(const std::string& payload);

    std::string webhook_;
    std::deque<DiscordEvent> q_;
    std::mutex mx_;
    std::condition_variable cv_;
    std::thread th_;
    std::atomic<bool> stop_{false};
};
