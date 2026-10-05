#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <string>
#include <sstream>

#include "core.cpp"

using namespace mailsec;

void testSpfCheck() {
    MailSecurityChecker checker;
    checker.setDomain("example.com");
    
    // No SPF record
    assert(!checker.checkSpf());
    
    // Invalid SPF record
    checker.addSpfRecord("v=spf2");
    assert(!checker.checkSpf());
    
    // Valid SPF record
    checker.addSpfRecord("v=spf1 mx -all");
    assert(checker.checkSpf());
    
    std::cout << "✓ SPF check tests passed" << std::endl;
}

void testDkimCheck() {
    MailSecurityChecker checker;
    checker.setDomain("example.com");
    
    // No DKIM record
    assert(!checker.checkDkim());
    
    // Invalid DKIM record
    checker.addDkimRecord("selector1", "k=dsa");
    assert(!checker.checkDkim());
    
    // Valid DKIM record with RSA
    checker.addDkimRecord("selector2", "k=rsa; p=abc123");
    assert(checker.checkDkim());
    
    std::cout << "✓ DKIM check tests passed" << std::endl;
}

void testTlsaCheck() {
    MailSecurityChecker checker;
    checker.setDomain("example.com");
    
    // No TLSA record
    assert(!checker.checkTlsa());
    
    // Valid TLSA record
    checker.addTlsaRecord("1 1 1 abc123def456");
    assert(checker.checkTlsa());
    
    std::cout << "✓ TLSA check tests passed" << std::endl;
}

void testTlsCheck() {
    MailSecurityChecker checker;
    checker.setDomain("example.com");
    
    // TLS disabled
    checker.setTlsEnabled(false);
    assert(!checker.checkTls());
    
    // TLS enabled but old version
    checker.setTlsEnabled(true);
    checker.setTlsVersion("TLS1.0");
    assert(!checker.checkTls());
    
    // TLS enabled with TLS1.2
    checker.setTlsVersion("TLS1.2");
    assert(checker.checkTls());
    
    // TLS enabled with TLS1.3
    checker.setTlsVersion("TLS1.3");
    assert(checker.checkTls());
    
    std::cout << "✓ TLS check tests passed" << std::endl;
}

void testSecurityScore() {
    MailSecurityChecker checker;
    checker.setDomain("example.com");
    
    // No security features
    assert(checker.getSecurityScore() == 0);
    assert(checker.getSecurityStatus() == "CRITICAL");
    
    // Only SPF
    checker.addSpfRecord("v=spf1 mx -all");
    assert(checker.getSecurityScore() == 25);
    assert(checker.getSecurityStatus() == "POOR");
    
    // SPF + DKIM
    checker.addDkimRecord("sel", "k=rsa; p=abc");
    assert(checker.getSecurityScore() == 50);
    assert(checker.getSecurityStatus() == "FAIR");
    
    // SPF + DKIM + TLSA
    checker.addTlsaRecord("1 1 1 abc");
    assert(checker.getSecurityScore() == 75);
    assert(checker.getSecurityStatus() == "GOOD");
    
    // All features
    checker.setTlsEnabled(true);
    checker.setTlsVersion("TLS1.3");
    assert(checker.getSecurityScore() == 100);
    assert(checker.getSecurityStatus() == "EXCELLENT");
    
    std::cout << "✓ Security score tests passed" << std::endl;
}

void testConfigAccess() {
    MailSecurityChecker checker;
    checker.setDomain("test.com");
    checker.setHostname("mail.test.com");
    checker.addSpfRecord("v=spf1 -all");
    checker.addDkimRecord("sel", "k=rsa");
    checker.addTlsaRecord("1 1 1 hash");
    checker.setTlsEnabled(true);
    checker.setTlsVersion("TLS1.3");
    checker.addCipherSuite("TLS_AES_256_GCM_SHA384");
    
    const auto& config = checker.getConfig();
    assert(config.domain == "test.com");
    assert(config.hostname == "mail.test.com");
    assert(config.spf.size() == 1);
    assert(config.dkim.size() == 1);
    assert(config.tlsa.size() == 1);
    assert(config.tls_enabled == true);
    assert(config.tls_version == "TLS1.3");
    assert(config.cipher_suites.size() == 1);
    assert(config.cipher_suites[0] == "TLS_AES_256_GCM_SHA384");
    
    std::cout << "✓ Config access tests passed" << std::endl;
}

void testMultipleRecords() {
    MailSecurityChecker checker;
    checker.setDomain("multi.com");
    
    // Multiple SPF records (should still pass if any valid)
    checker.addSpfRecord("invalid");
    checker.addSpfRecord("v=spf1 mx -all");
    assert(checker.checkSpf());
    
    // Multiple DKIM records
    checker.addDkimRecord("sel1", "k=dsa");
    checker.addDkimRecord("sel2", "k=rsa; p=valid");
    assert(checker.checkDkim());
    
    std::cout << "✓ Multiple records tests passed" << std::endl;
}

void testEdgeCases() {
    MailSecurityChecker checker;
    checker.setDomain("");
    
    // Empty domain should not crash
    assert(!checker.checkSpf());
    assert(!checker.checkDkim());
    assert(!checker.checkTlsa());
    assert(!checker.checkTls());
    assert(checker.getSecurityScore() == 0);
    
    std::cout << "✓ Edge case tests passed" << std::endl;
}

int main() {
    std::cout << "=== Mail Security Checker Test Suite ===" << std::endl;
    std::cout << std::endl;
    
    try {
        testSpfCheck();
        testDkimCheck();
        testTlsaCheck();
        testTlsCheck();
        testSecurityScore();
        testConfigAccess();
        testMultipleRecords();
        testEdgeCases();
        
        std::cout << std::endl;
        std::cout << "=== All tests passed successfully ===" << std::endl;
        
        // Demo
        std::cout << std::endl;
        std::cout << "=== Demo: Mail Server Security Check ===" << std::endl;
        
        MailSecurityChecker checker;
        checker.setDomain("example.com");
        checker.setHostname("mail.example.com");
        checker.addSpfRecord("v=spf1 mx -all");
        checker.addDkimRecord("default", "k=rsa; p=MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8A");
        checker.addTlsaRecord("1 1 1 31bf84c6b15b84a50e5f1b0b86c6e0e0");
        checker.setTlsEnabled(true);
        checker.setTlsVersion("TLS1.3");
        checker.addCipherSuite("TLS_AES_256_GCM_SHA384");
        checker.addCipherSuite("TLS_CHACHA20_POLY1305_SHA256");
        
        std::cout << "Domain: " << checker.getConfig().domain << std::endl;
        std::cout << "SPF: " << (checker.checkSpf() ? "PASS" : "FAIL") << std::endl;
        std::cout << "DKIM: " << (checker.checkDkim() ? "PASS" : "FAIL") << std::endl;
        std::cout << "TLSA: " << (checker.checkTlsa() ? "PASS" : "FAIL") << std::endl;
        std::cout << "TLS: " << (checker.checkTls() ? "PASS" : "FAIL") << std::endl;
        std::cout << "Security Score: " << checker.getSecurityScore() << "/100" << std::endl;
        std::cout << "Status: " << checker.getSecurityStatus() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
