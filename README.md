# Drawing Machine Firmware
Arduino sketch + associated source files for running our 2.5D polar plotter.

Currently implemented g-codes:
- **M0:** unconditional stop
- **M17:** enable steppers
- **M18:** disable steppers
- **M84:** stop idle hold
- **M105:** get status
- **M112:** emergency stop
- **G0:** rapid movement
- **G1:** linear movement
- **G4:** dwell
- **G28:** home
- **G90:** absolute mode
- **G91:** relative mode
- **G92:** set position

---

[Demo video](https://www.youtube.com/watch?v=BZjUXDSYovs) and relevant repositories:
- [drawing machine dashboard application](http://github.com/brentyi/drawing_machine_dashboard)
- [drawing machine mechanical design](http://github.com/nanditapiyer/drawing_machine_hardware)

---

Made possible by the [AccelStepper](http://www.airspayce.com/mikem/arduino/AccelStepper/) library.
