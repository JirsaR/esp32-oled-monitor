#pragma once

#include "Arduino.h"
#include "config.h"

// DST Rule structure
struct DSTRules {
  int day;
  int month;
  int year;
  long offset; // offset in seconds
};

class NTPClient {
  private:
    const char*   _poolServerName = NTP_SERVER; // Default time server
    long          _timeOffset     = 0;              // In s (3600 is GMT+1)
    unsigned long _updateInterval = 60000;          // In ms
    unsigned long _lastUpdate     = 0;              // In ms

    struct tm     _timeInfo;                        // Stores the current time
    
    // DST rules management
    DSTRules      _dstRules[10];                    // Array for DST rules (max 10 rules)
    int           _dstRulesCount = 0;               // Current number of DST rules
    
    long          _calculateCurrentOffset();        // Calculate offset based on DST rules
    void          _applyDSTRules();                 // Apply DST rules to current time offset
  
  public:
    NTPClient();

    /**
     * Set time server name
     *
     * @param poolServerName
     */
    void setPoolServerName(const char* poolServerName);

    /**
     * Changes the time offset.
     */
    void setTimeOffset(int timeOffset);

    /**
     * Add a DST (Daylight Saving Time) rule
     * 
     * Example for Czech Republic:
     * ntpClient.addDSTRule(29, 3, 2026, 3600);  // March 29, 2026, GMT+2 (DST starts)
     * ntpClient.addDSTRule(25, 10, 2026, 0);    // October 25, 2026, GMT+1 (DST ends)
     * 
     * @param day Day of month (1-31)
     * @param month Month (1-12)
     * @param year Year (e.g., 2026)
     * @param offset Time offset in seconds (e.g., 3600 for GMT+1, 0 for GMT+0)
     * @return true if rule was added successfully, false if max rules reached
     */
    bool addDSTRule(int day, int month, int year, long offset);

    /**
     * Clear all DST rules
     */
    void clearDSTRules();

    /**
     * Set the update interval to another frequency.
     */
    void setUpdateInterval(unsigned long updateInterval);

    /**
     * This should be called in the main loop of your application. By default an update from the NTP Server is only
     * made every 60 seconds.
     *
     * @return true on success, false on failure
     */
    bool update();

    /**
     * This will force the update from the NTP Server.
     *
     * @return true on success, false on failure
     */
    bool forceUpdate();

    /**
     * @return time formatted like `hh:mm:ss`
     */
    String getFormattedTime() const;

    /**
     * @return time formatted like `DD.MM.YYYY`
     */
    String getFormattedDate() const;
};
