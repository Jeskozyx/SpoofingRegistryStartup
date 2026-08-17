#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <cctype>

class PasswordManager {
private:
    std::string m_password;
    std::string m_currentInput;
    int m_maxAttempts;
    int m_attempts;
    
public:
    PasswordManager(const std::string& password = "123", int maxAttempts = 3) 
        : m_password(password), m_currentInput(""), m_maxAttempts(maxAttempts), m_attempts(0) {}
    
    void SetPassword(const std::string& password) {
        m_password = password;
        m_currentInput.clear();
    }

    bool ValidatePassword(const std::string& input) {
        if (m_attempts >= m_maxAttempts) {
            return false;
        }
        if (input == m_password) {
            m_attempts = 0;
            return true;
        }
        m_attempts++;
        return false;
    }

    bool ValidatePassword(const std::wstring& input) {
        std::string str(input.begin(), input.end());
        return ValidatePassword(str);
    }

    bool ValidatePassword(const wchar_t* input) {
        if (!input) return false;
        return ValidatePassword(std::wstring(input));
    }

    bool ProcessKey(DWORD vkCode) {
        char c = 0;
        if (vkCode >= 'A' && vkCode <= 'Z') {
            c = static_cast<char>(std::tolower(static_cast<int>(vkCode)));
        } else if (vkCode >= '0' && vkCode <= '9') {
            c = static_cast<char>(vkCode);
        }

        if (c != 0) {
            m_currentInput += c;
            if (m_currentInput.length() > m_password.length()) {
                m_currentInput.erase(0, m_currentInput.length() - m_password.length());
            }

            if (m_currentInput == m_password) {
                return true;
            }
        }
        return false;
    }
    
    bool IsLocked() {
        return m_attempts >= m_maxAttempts;
    }
    
    void Reset() {
        m_attempts = 0;
        m_currentInput.clear();
    }
};