# Drift Live

Гранулярный XY-эффект для лайва. Ставится вставкой (insert) на барабаны или любой инструмент.

**Цепочка:** гранулятор → фленжер → пан.

**Радар (XY).** В каждом углу своя сцена: A Clean, B Cloud, C Swirl, D Abyss. Маячок плавно смешивает их между собой.

**Что ещё есть:**
- **Auto motion:** маячок сам ходит по радару (Circle, Eight, Bounce, Random), синхронно с темпом DAW.
- **2 LFO:** можно повесить на любой параметр сцены.
- **Glide и Snap back:** плавность движения маячка и возврат в угол Clean, когда отпускаешь мышь.
- **Mix:** общий dry/wet — большая ручка вверху и первый параметр в списке DAW.

Форматы:
- **Mac:** VST3, AU, Standalone. Universal-сборка, работает на Apple Silicon и Intel.
- **Linux:** VST3, LV2, Standalone.
- **Windows:** VST3, Standalone (64-bit).

## Как собрать (так же, как ML-1)

1. Создай на GitHub новый репозиторий, например `drift-live`.
2. Загрузи туда все файлы из этой папки, сохранив структуру. Обязательно вместе со скрытой папкой `.github`, можно через Codespaces.
3. Открой вкладку **Actions**. Сборка запустится сама и займёт минут 10–20. JUCE скачивается автоматически.
4. Внизу страницы готовой сборки будут три архива: `DriftLive-mac`, `DriftLive-windows` и `DriftLive-linux`.

## Установка

**Mac**
- `Drift Live.vst3` положи в `~/Library/Audio/Plug-Ins/VST3/`.
- `Drift Live.component` положи в `~/Library/Audio/Plug-Ins/Components/`.
- Плагин не подписан, поэтому macOS его заблокирует. Один раз выполни в Терминале:
  `xattr -cr ~/Library/Audio/Plug-Ins/VST3/"Drift Live.vst3" ~/Library/Audio/Plug-Ins/Components/"Drift Live.component"`
- Пересканируй плагины в DAW.

**Windows**
- Папку `Drift Live.vst3` положи в `C:\Program Files\Common Files\VST3\`.
- `Drift Live.exe` — отдельная версия без DAW, запускается сама.
- Если Windows покажет предупреждение SmartScreen, нажми «Подробнее» → «Выполнить в любом случае»: плагин не подписан.
- Пересканируй плагины в DAW.

**Linux**
- `Drift Live.vst3` положи в `~/.vst3/`.
- `Drift Live.lv2` положи в `~/.lv2/`.

## Файлы
- `Source/Params.h` — все параметры и значения сцен по умолчанию.
- `Source/PluginProcessor.cpp` — звук: гранулятор, фленжер, пан, LFO, автодвижение.
- `Source/PluginEditor.cpp` — интерфейс: радар, лампы, потёртый корпус.
