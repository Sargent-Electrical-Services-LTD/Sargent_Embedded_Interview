# Exercise

You are inheriting an ESP32 display-board application that simulates sensor readings and shows the current value and status.

In `components/exercise/exercise_app.c`, one main loop initializes the display, samples once per second, retries reads up to three times, logs to serial, and updates a simple LVGL status label.

Sensor input comes from `components/sensor_sim/sensor_sim.c`, which injects deterministic failures (timeout every 20th sample, invalid data every 13th sample).

The starter includes intentional functional bugs in error handling, retry outcomes, and timing behavior, along with weak module boundaries and mixed logging quality.

Your task is to refactor this to improve reliability and timing behavior, clean up structure, make logging more useful, and keep the UI path minimal but solid. The goal is to only spend a few hours on the task, so the additions at the end are entirely optional (but it would be good to discuss how they could be implemented).

## Getting Started

- Target chip: `esp32`
- ESP-IDF version: `>= v5.2`
- Expected monitor baud rate: `115200`

From the repo root, use the standard ESP-IDF flow:

```bash
idf.py set-target esp32
idf.py build
idf.py flash 
idf.py monitor
```
Multiple idf.py commands can be combined into one. For example, `idf.py -p COM4 clean flash monitor` will clean the source tree, then build the project and flash it to the target before running the serial monitor.

You should see `ESP_LOG*` output in the serial monitor. Press `Ctrl+]` to exit monitor.

## Managed Components (BSP and LVGL)

This project pulls common platform pieces through ESP-IDF managed components so you can focus on application behavior instead of low-level display bring-up.

- `esp_bsp_generic` (BSP): board support helpers for display init, backlight control, and LVGL lock/unlock integration (`bsp_display_start`, `bsp_display_backlight_on`, `bsp_display_lock`).
- `lvgl`: the graphics library used for the on-screen status label.

Treat managed components as infrastructure. Use their APIs from your app code, but do not refactor internals of managed components.

## Important Scope Rules

1. Treat the sensor simulator as the input source and use its API (`sensor_sim_read`) from application logic.
2. You may call the simulator as needed from your retry/sampling flow.
3. Do not redesign or replace the simulator component itself.
4. Focus on reliability, timing behavior, and maintainability in the app layer.

## What To Do

1. Read the current implementation and identify the most important defects.
2. Refactor the code into cleaner module boundaries where practical.
3. Fix reliability issues, especially around retry behavior, invalid data handling, failure/state reporting, and long-run stability of periodic sampling.
4. Improve observability using ESP-IDF logging macros (`ESP_LOGI`, `ESP_LOGW`, `ESP_LOGE`) with useful context (for example `seq`, result, and tick/time).
5. Keep the system running during failures (no lockup/crash loops).

## Optional Additions

Heartbeat and health summary:
- Add a periodic health log (for example every 5 seconds).
- Include uptime, samples processed, failures, and current state.

Historical data:
- Keep the last K sample/status entries.
- Print a snapshot on warning/error transitions.

Display:
- Improve the look of the UI using the LVGL components provided.

Touch discussion:
- The board touch path is not supported in the provided generic BSP setup.
- Briefly describe how you would add touch support if this were a product task.

## Submission Format

Preferred:
1. Fork this repository.
2. Push your solution branch to your fork.
3. Open a pull request from your fork branch to this repository and share the PR link.

If you cannot use GitHub:
1. Create a `.zip` of your modified project
2. Send via email

