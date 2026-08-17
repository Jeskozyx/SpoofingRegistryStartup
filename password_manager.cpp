#include "password_manager.h"
#include <cctype>

PasswordManager::PasswordManager(const std::string& exitPassword)
    : m_targetPassword(exitPassword), m_currentInput("") {}

void PasswordManager::SetPassword(const std::string& password) {
    m_targetPassword = password;
    m_currentInput.clear();
}

void PasswordManager::Reset() {
    m_currentInput.clear();
}

bool PasswordManager::ProcessKey(DWORD vkCode) {
    // Only handle alphanumeric ASCII keys for password input
    char c = 0;
    if (vkCode >= 'A' && vkCode <= 'Z') {
        c = static_cast<char>(std::tolower(static_cast<int>(vkCode)));
    } else if (vkCode >= '0' && vkCode <= '9') {
        c = static_cast<char>(vkCode);
    }

    if (c != 0) {
        m_currentInput += c;
        if (m_currentInput.length() > m_targetPassword.length()) {
            m_currentInput.erase(0, m_currentInput.length() - m_targetPassword.length());
        }

        if (m_currentInput == m_targetPassword) {
            return true;
        }
    }
    return false;
}
