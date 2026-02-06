#include "../include/discord_notifier.hpp"
#include <curl/curl.h>
#include <chrono>
#include <thread>

DiscordNotifier::DiscordNotifier(std::string webhook_url)
: webhook_(std::move(webhook_url)) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    th_ = std::thread([this]{ worker(); });
}

DiscordNotifier::~DiscordNotifier() {
    stop_ = true;
    cv_.notify_all();
    if (th_.joinable()) th_.join();
    curl_global_cleanup();
}

void DiscordNotifier::enqueue(DiscordEvent ev) {
    {
        std::lock_guard lk(mx_);
        q_.push_back(std::move(ev));
    }
    cv_.notify_one();
}

void DiscordNotifier::worker() {
    while (!stop_) {
        DiscordEvent ev;
        {
            std::unique_lock lk(mx_);
            cv_.wait(lk, [&]{ return stop_ || !q_.empty(); });
            if (stop_) break;
            ev = std::move(q_.front());
            q_.pop_front();
        }

        for (int attempt = 0; attempt < 5 && !stop_; ++attempt) {
            if (post_json(ev.json_payload)) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(500 * (attempt + 1)));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

bool DiscordNotifier::post_json(const std::string& payload) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, webhook_.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload.size());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 4000L);

    CURLcode res = curl_easy_perform(curl);

    long code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return (res == CURLE_OK) && (code >= 200 && code < 300);
}
