#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>
#include <memory>

namespace mailsec {

struct DnsRecord {
    std::string type;
    std::string value;
};

struct MailServerConfig {
    std::string domain;
    std::string hostname;
    std::vector<DnsRecord> spf;
    std::vector<DnsRecord> dkim;
    std::vector<DnsRecord> tlsa;
    bool tls_enabled = false;
    std::string tls_version;
    std::vector<std::string> cipher_suites;
};

class MailSecurityChecker {
public:
    MailSecurityChecker() = default;
    ~MailSecurityChecker() = default;

    void setDomain(const std::string& domain) {
        config_.domain = domain;
    }

    void setHostname(const std::string& hostname) {
        config_.hostname = hostname;
    }

    void addSpfRecord(const std::string& value) {
        config_.spf.push_back({"TXT", value});
    }

    void addDkimRecord(const std::string& selector, const std::string& value) {
        config_.dkim.push_back({"TXT", selector + "._domainkey." + config_.domain + " " + value});
    }

    void addTlsaRecord(const std::string& value) {
        config_.tlsa.push_back({"TLSA", value});
    }

    void setTlsEnabled(bool enabled) {
        config_.tls_enabled = enabled;
    }

    void setTlsVersion(const std::string& version) {
        config_.tls_version = version;
    }

    void addCipherSuite(const std::string& suite) {
        config_.cipher_suites.push_back(suite);
    }

    bool checkSpf() const {
        if (config_.spf.empty()) return false;
        for (const auto& rec : config_.spf) {
            if (rec.value.find("v=spf1") != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    bool checkDkim() const {
        if (config_.dkim.empty()) return false;
        for (const auto& rec : config_.dkim) {
            if (rec.value.find("k=rsa") != std::string::npos || 
                rec.value.find("k=ed25519") != std::string::npos) {
                return true;
            }
        }
        return false;
    }

    bool checkTlsa() const {
        return !config_.tlsa.empty();
    }

    bool checkTls() const {
        if (!config_.tls_enabled) return false;
        if (config_.tls_version == "TLS1.2" || config_.tls_version == "TLS1.3") {
            return true;
        }
        return false;
    }

    int getSecurityScore() const {
        int score = 0;
        if (checkSpf()) score += 25;
        if (checkDkim()) score += 25;
        if (checkTlsa()) score += 25;
        if (checkTls()) score += 25;
        return score;
    }

    std::string getSecurityStatus() const {
        int score = getSecurityScore();
        if (score == 100) return "EXCELLENT";
        if (score >= 75) return "GOOD";
        if (score >= 50) return "FAIR";
        if (score >= 25) return "POOR";
        return "CRITICAL";
    }

    const MailServerConfig& getConfig() const {
        return config_;
    }

private:
    MailServerConfig config_;
};

} // namespace mailsec
