#ifndef APPS_H
#define APPS_H

struct App {
  const char* name;
  const char* appCommand;   // Win+R app command
  const char* siteCommand;  // URL or same path if no toggle
};

App apps[] = {
  {"Spotify", "spotify:", "https://open.spotify.com"},
  {"YouTube", "youtube:", "https://www.youtube.com"},
  {"ChatGPT", "https://chatgpt.com", "https://chatgpt.com"},
  {"Discord", "discord:", "https://discord.com"},
  {"Gmail", "https://mail.google.com", "https://mail.google.com"},
  {"Steam", "steam://", "steam://"},
  {"VSCode", "Code", "Code"},
  {"Maker World", "https://makerworld.com", "https://makerworld.com"},
  {"Bambu Studio", "C:\\Program Files\\Bambu Studio\\bambu-studio.exe", "C:\\Program Files\\Bambu Studio\\bambu-studio.exe"},
  {"Whatsapp", "whatsapp:", "https://web.whatsapp.com"}
};

int totalApps = sizeof(apps) / sizeof(App);

#endif
